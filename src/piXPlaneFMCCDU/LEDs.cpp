/*
 This file is part of Pi-XPlane-FMC-CDU
 A Raspberry Pi-based External FMC for XPlane

 Copyright (C) 2017-2018 shahada abubakar
 <shahada@abubakar.net>

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

#include <unistd.h>
#include <wiringPi.h>
#include <syslog.h>
#include <sys/stat.h>
#include <cmath>
#include <cstdlib>
#include <fstream>

#include "LEDs.h"

// initialize statics
LEDs * LEDs::instance = NULL;

// PWM range for the backlight (duty cycle 0..PWM_RANGE).
// 19.2 MHz / clock 4 / range 1024 = ~4.7 kHz, well above visible flicker.
static const int PWM_RANGE = 1024;
static const int PWM_CLOCK = 4;

LEDs::LEDs() {

	// Hardware PWM needs direct register access, which wiringPi only has
	// as root. When not root it uses /dev/gpiomem and would *exit* on the
	// first PWM call, so only use PWM when it's safe.
	hwPwm = (geteuid() == 0) && (getenv("WIRINGPI_GPIOMEM") == NULL);
#ifndef __arm__
	hwPwm = false;	// x86 simulated build
#endif

	pinMode(LED_EXEC, OUTPUT);
	pinMode(LED_DSPY, OUTPUT);
	pinMode(LED_FAIL, OUTPUT);
	pinMode(LED_MSG, OUTPUT);
	pinMode(LED_OFST, OUTPUT);
	if (hwPwm) {
		pinMode(LED_BG, PWM_OUTPUT);
		pwmSetMode(PWM_MODE_MS);
		pwmSetRange(PWM_RANGE);
		pwmSetClock(PWM_CLOCK);
	} else {
		pinMode(LED_BG, OUTPUT);
	}
	syslog(LOG_INFO, "Backlight: %s", hwPwm ? "hardware PWM (dimmable)" : "on/off only (not root)");

	loadBacklightLevel();

	// lamp test
	digitalWrite(LED_EXEC, 1);
	digitalWrite(LED_DSPY, 1);
	digitalWrite(LED_FAIL, 1);
	digitalWrite(LED_MSG, 1);
	digitalWrite(LED_OFST, 1);
	setLED(LED_BG, 1);

	sleep(1);

	digitalWrite(LED_EXEC, 0);
	digitalWrite(LED_DSPY, 0);
	digitalWrite(LED_FAIL, 0);
	digitalWrite(LED_MSG, 0);
	digitalWrite(LED_OFST, 0);

}

LEDs::~LEDs() {

	digitalWrite(LED_EXEC, 0);
	digitalWrite(LED_DSPY, 0);
	digitalWrite(LED_FAIL, 0);
	digitalWrite(LED_MSG, 0);
	digitalWrite(LED_OFST, 0);
	setLED(LED_BG, 0);

	pinMode(LED_EXEC, INPUT);
	pinMode(LED_DSPY, INPUT);
	pinMode(LED_FAIL, INPUT);
	pinMode(LED_MSG, INPUT);
	pinMode(LED_OFST, INPUT);
	pinMode(LED_BG, INPUT);
}

void LEDs::setLED(LEDs::LedTypes led, bool on) {

	if (led == LED_BG) {
		std::lock_guard<std::recursive_mutex> lock(backlightMutex);
		backlightWanted = on;
		applyBacklight();
		return;
	}
	digitalWrite(led, on);
}

void LEDs::setAllLEDs(bool on) {

	setLED(LED_EXEC, on);
	setLED(LED_DSPY, on);
	setLED(LED_FAIL, on);
	setLED(LED_MSG, on);
	setLED(LED_OFST, on);
	setLED(LED_BG, on);
}

// Write the current backlight state to the pin. Caller holds backlightMutex.
void LEDs::applyBacklight() {

	int level = backlightWanted ? backlightLevel : 0;

	if (hwPwm) {
		// perceived brightness is roughly logarithmic: use a gamma curve so
		// each step looks like a similar change.
		int duty = 0;
		if (level > 0) {
			duty = (int) lround(PWM_RANGE * pow((double) level / BACKLIGHT_STEPS, 2.2));
			if (duty < 1) duty = 1;
		}
		pwmWrite(LED_BG, duty);
	} else {
		digitalWrite(LED_BG, level > 0);
	}
}

void LEDs::changeBacklight(int delta) {

	std::lock_guard<std::recursive_mutex> lock(backlightMutex);
	int level = backlightLevel + delta;
	if (level < 0) level = 0;
	if (level > BACKLIGHT_STEPS) level = BACKLIGHT_STEPS;
	if (level == backlightLevel) return;
	backlightLevel = level;
	if (level > 0) lastNonZeroLevel = level;
	syslog(LOG_INFO, "Backlight level %d/%d", backlightLevel, BACKLIGHT_STEPS);
	applyBacklight();
	saveBacklightLevel();
}

void LEDs::toggleBacklight() {

	std::lock_guard<std::recursive_mutex> lock(backlightMutex);
	backlightLevel = (backlightLevel > 0) ? 0 : lastNonZeroLevel;
	syslog(LOG_INFO, "Backlight toggled, level %d/%d", backlightLevel, BACKLIGHT_STEPS);
	applyBacklight();
	saveBacklightLevel();
}

int LEDs::getBacklightLevel() {

	std::lock_guard<std::recursive_mutex> lock(backlightMutex);
	return backlightLevel;
}

// The level is kept in /var/lib/piXPlaneFMCCDU/backlight (or in the user's
// home directory if that isn't writable) so it survives restarts.
void LEDs::loadBacklightLevel() {

	mkdir("/var/lib/piXPlaneFMCCDU", 0755);
	levelFile = "/var/lib/piXPlaneFMCCDU/backlight";
	if (access("/var/lib/piXPlaneFMCCDU", W_OK) != 0 && getenv("HOME") != NULL) {
		levelFile = std::string(getenv("HOME")) + "/.piXPlaneFMCCDU-backlight";
	}

	std::ifstream in(levelFile);
	int level, last;
	if (in >> level >> last) {
		if (level >= 0 && level <= BACKLIGHT_STEPS) backlightLevel = level;
		if (last > 0 && last <= BACKLIGHT_STEPS) lastNonZeroLevel = last;
	}
	syslog(LOG_INFO, "Backlight level %d/%d (from %s)", backlightLevel, BACKLIGHT_STEPS, levelFile.c_str());
}

void LEDs::saveBacklightLevel() {

	std::ofstream out(levelFile, std::ios::trunc);
	if (out) {
		out << backlightLevel << " " << lastNonZeroLevel << std::endl;
	}
}
