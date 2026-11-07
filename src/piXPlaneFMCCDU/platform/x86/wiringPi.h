/*
 This file is part of Pi-XPlane-FMC-CDU
 A Raspberry Pi-based External FMC for XPlane

 Stand-in for wiringPi used only by the x86 (simulated) build.

 The real wiringPi refuses to run on anything that isn't a Raspberry Pi
 (wiringPiSetup() exits), so the simulated build compiles against these
 no-op versions instead. LEDs are logged to stderr so their state can still
 be followed when running on a PC.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 */

#ifndef PIXPLANEFMCCDU_X86_WIRINGPI_STUB_H_
#define PIXPLANEFMCCDU_X86_WIRINGPI_STUB_H_

#include <iostream>
#include <unistd.h>

#define INPUT  0
#define OUTPUT 1

#define LOW  0
#define HIGH 1

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

inline int wiringPiSetup(void) { return 0; }
inline void pinMode(int, int) {}
inline void pullUpDnControl(int, int) {}
inline void digitalWrite(int pin, int value) {
	std::cerr << "[gpio] pin " << pin << " = " << value << std::endl;
}
// Inputs read as "not pressed" (pull-ups) on the PC.
inline int digitalRead(int) { return HIGH; }
inline void delay(unsigned int ms) { usleep(ms * 1000); }
inline void delayMicroseconds(unsigned int us) { usleep(us); }

#endif /* PIXPLANEFMCCDU_X86_WIRINGPI_STUB_H_ */
