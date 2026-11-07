/*
 This file is part of Pi-XPlane-FMC-CDU
 A Raspberry Pi-based External FMC for XPlane

 Copyright (C) 2017-2018 shahada abubakar
 <shahada@abubakar.net>

File contributed by Lefteris Kororos <lkororos@gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

#include <wiringPi.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <unistd.h>

#include "RotaryEncScanner.h"
#include "FMCManager.h"

using namespace std;

// initialize statics
RotaryEncScanner * RotaryEncScanner::instance = NULL;

RotaryEncScanner::RotaryEncScanner () {

	detentCount = 0;

	// inputs with pull-up enabled (encoder contacts switch to GND)
	for (int pin : { PIN_A, PIN_B, PIN_SW }) {
		pinMode (pin, INPUT);
		pullUpDnControl (pin, PUD_UP);
	}

	isRunning = false;

}

RotaryEncScanner::~RotaryEncScanner() {

}

void RotaryEncScanner::launchThread() {

	cerr << "In RotaryEncScanner::launchThread" << endl;
	if (!isRunning) {
		std::thread t(&RotaryEncScanner::mainLoop, this);
		t.detach();
	}
	cerr << "Finished RotaryEncScanner::launchThread" << endl;
}

void RotaryEncScanner::mainLoop() {

	cerr << "In RotaryEncScanner::mainLoop" << endl;

	// Quadrature decoding with a transition table: index is
	// (previous state << 2) | new state, where state = (A << 1) | B.
	// Valid single steps give +1/-1. No change, and "impossible" jumps
	// where both contacts changed at once (bounce or a missed sample),
	// give 0, so contact bounce cancels out instead of producing
	// spurious or reversed steps.
	static const int table[16] = {
		 0, -1, +1,  0,
		+1,  0,  0, -1,
		-1,  0,  0, +1,
		 0, +1, -1,  0
	};

	int state = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);
	int accum = 0;

	int swStable = digitalRead(PIN_SW);
	int swLast = swStable;
	int swCount = 0;

	isRunning = true;
	stopRequested = false;

	while (!stopRequested) {

		// --- rotation ---
		int newState = (digitalRead(PIN_A) << 1) | digitalRead(PIN_B);
		if (newState != state) {
			accum += table[(state << 2) | newState];
			state = newState;

			if (accum >= TRANSITIONS_PER_DETENT || accum <= -TRANSITIONS_PER_DETENT) {
				int step = (accum > 0 ? 1 : -1) * DIRECTION;
				accum = 0;
				detentCount += step;
				syslog(LOG_INFO, "Rotary encoder %+d (count %d)", step, detentCount);
				FMCManager::getInstance()->rotaryEncTurnEvent(detentCount, step);
			}
		}

		// --- push switch (active low), debounced ---
		int sw = digitalRead(PIN_SW);
		if (sw != swLast) {
			swLast = sw;
			swCount = 0;
		} else if (sw != swStable && ++swCount >= SWITCH_DEBOUNCE_MS) {
			swStable = sw;
			if (swStable == 0) {
				syslog(LOG_INFO, "Rotary encoder pressed");
				FMCManager::getInstance()->rotaryEncPressEvent();
			}
		}

		// 1 ms is fast enough for a hand-turned encoder and avoids
		// spinning the CPU.
		usleep(1000);
	}

	cerr << "Finished RotaryEncScanner::mainLoop" << endl;
}
