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
#ifndef PIXPLANEFMCCDU_SRC_ROTARYENCSCANNER_H_
#define PIXPLANEFMCCDU_SRC_ROTARYENCSCANNER_H_

#include <syslog.h>
#include <vector>
#include <stdlib.h>

class RotaryEncScanner {
private:
	RotaryEncScanner(); // private so it cannot be called

	RotaryEncScanner(RotaryEncScanner const &) {};

	RotaryEncScanner & operator=(RotaryEncScanner const &) {
		abort();
	}	 // assignment operator is private

protected:
	static RotaryEncScanner * instance;

	// Alps EC11E18244A5: A, B and the push switch all switch to GND,
	// so the inputs use pull-ups.
	static const int PIN_A = 28;	// p.38 BCM.20 ROTA
	static const int PIN_B = 25;	// p.37 BCM.26 ROTB
	static const int PIN_SW = 26;	// p.32 BCM.12 push switch

	// Quadrature transitions per detent ("click"). If one click moves the
	// brightness by two steps, set this to 4; if it takes two clicks per
	// step, set it to 1.
	static const int TRANSITIONS_PER_DETENT = 2;

	// Set to -1 to reverse the direction of rotation.
	static const int DIRECTION = 1;

	// Push switch must be stable this many polls (1 ms each) to count.
	static const int SWITCH_DEBOUNCE_MS = 20;

	int detentCount;

	bool isRunning;
	bool stopRequested;

public:

	static RotaryEncScanner * getInstance() {
		if (!instance) {
			syslog(LOG_INFO, "Intantiating RotaryEncScanner");
			instance = new RotaryEncScanner();
		}
		return instance;
	}

	void launchThread();
	void mainLoop();

	virtual ~RotaryEncScanner();

};

#endif /* PIXPLANEFMCCDU_SRC_ROTARYENCSCANNER_H_ */
