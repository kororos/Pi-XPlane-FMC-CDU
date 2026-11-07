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

#include <iostream>
#include <regex>
#include <sstream>
#include <algorithm>

#include "ZiboFMC.h"
#include "Screen.h"
#include "FMCManager.h"
#include "LEDs.h"

using namespace std;

ZiboFMC::ZiboFMC(bool isCaptain) {

	side = isCaptain ? "1" : "2";

	// populate keyInfo
	keyInfo[1][1] = "laminar/B738/button/fmc" + side + "_1L";	// sw-1
	keyInfo[2][1] = "laminar/B738/button/fmc" + side + "_2L";	// sw-2
	keyInfo[3][1] = "laminar/B738/button/fmc" + side + "_3L"; // sw-3
	keyInfo[4][1] = "laminar/B738/button/fmc" + side + "_4L"; 	// sw-4
	keyInfo[5][1] = "laminar/B738/button/fmc" + side + "_5L"; 	// sw-5
	keyInfo[6][1] = "laminar/B738/button/fmc" + side + "_6L"; 	// sw-6
	keyInfo[7][1] = "laminar/B738/button/fmc" + side + "_1R"; 	// sw-7
	keyInfo[8][1] = "laminar/B738/button/fmc" + side + "_2R"; 	// sw-8
	keyInfo[1][2] = "laminar/B738/button/fmc" + side + "_3R"; 	// sw-9
	keyInfo[2][2] = "laminar/B738/button/fmc" + side + "_4R";	// sw-10
	keyInfo[3][2] = "laminar/B738/button/fmc" + side + "_5R"; 	// sw-11
	keyInfo[4][2] = "laminar/B738/button/fmc" + side + "_6R"; 	// sw-12
	keyInfo[5][2] = "laminar/B738/button/fmc" + side + "_init_ref"; 	// sw-13
	keyInfo[6][2] = "laminar/B738/button/fmc" + side + "_rte";	// sw-14
	keyInfo[7][2] = "laminar/B738/button/fmc" + side + "_clb"; 	// sw-15
	keyInfo[8][2] = "laminar/B738/button/fmc" + side + "_crz";	// sw-16
	keyInfo[1][3] = "laminar/B738/button/fmc" + side + "_des";	// sw-17
	keyInfo[2][3] = "laminar/B738/button/fmc" + side + "_menu";	// sw-18
	keyInfo[3][3] = "laminar/B738/button/fmc" + side + "_legs"; // sw-19
	keyInfo[4][3] = "laminar/B738/button/fmc" + side + "_dep_app";	// sw-20
	keyInfo[5][3] = "laminar/B738/button/fmc" + side + "_hold";	// sw-21
	keyInfo[6][3] = "laminar/B738/button/fmc" + side + "_prog";	// sw-22
	keyInfo[7][3] = "laminar/B738/button/fmc" + side + "_exec";	// sw-23
	keyInfo[8][3] = "laminar/B738/button/fmc" + side + "_n1_lim"; // sw-24
	keyInfo[1][4] = "laminar/B738/button/fmc" + side + "_fix";	// sw-25
	keyInfo[2][4] = "laminar/B738/button/fmc" + side + "_prev_page";	// sw-26
	keyInfo[3][4] = "laminar/B738/button/fmc" + side + "_next_page"; // sw-27
	keyInfo[4][4] = "laminar/B738/button/fmc" + side + "_A";	// sw-28
	keyInfo[5][4] = "laminar/B738/button/fmc" + side + "_B";	// sw-29
	keyInfo[6][4] = "laminar/B738/button/fmc" + side + "_C";	// sw-30
	keyInfo[7][4] = "laminar/B738/button/fmc" + side + "_D";	// sw-31
	keyInfo[8][4] = "laminar/B738/button/fmc" + side + "_E";	// sw32
	keyInfo[1][5] = "laminar/B738/button/fmc" + side + "_F";	// sw-33
	keyInfo[2][5] = "laminar/B738/button/fmc" + side + "_G";	// sw-34
	keyInfo[3][5] = "laminar/B738/button/fmc" + side + "_H";	// sw-35
	keyInfo[4][5] = "laminar/B738/button/fmc" + side + "_I";	// sw-36
	keyInfo[5][5] = "laminar/B738/button/fmc" + side + "_J";	// sw-37
	keyInfo[6][5] = "laminar/B738/button/fmc" + side + "_K";	// sw-38
	keyInfo[7][5] = "laminar/B738/button/fmc" + side + "_L";	// sw-39
	keyInfo[8][5] = "laminar/B738/button/fmc" + side + "_M";	// sw-40
	keyInfo[1][6] = "laminar/B738/button/fmc" + side + "_N";	// sw-41
	keyInfo[2][6] = "laminar/B738/button/fmc" + side + "_O";	// sw-42
	keyInfo[3][6] = "laminar/B738/button/fmc" + side + "_P";	// sw-43
	keyInfo[4][6] = "laminar/B738/button/fmc" + side + "_Q";	// sw-44
	keyInfo[5][6] = "laminar/B738/button/fmc" + side + "_R";	// sw-45
	keyInfo[6][6] = "laminar/B738/button/fmc" + side + "_S";	// sw-46
	keyInfo[7][6] = "laminar/B738/button/fmc" + side + "_T";	// sw-47
	keyInfo[8][6] = "laminar/B738/button/fmc" + side + "_U";	// sw-48
	keyInfo[1][7] = "laminar/B738/button/fmc" + side + "_V";	// sw-49
	keyInfo[2][7] = "laminar/B738/button/fmc" + side + "_W";	// sw-50
	keyInfo[3][7] = "laminar/B738/button/fmc" + side + "_X";	// sw-51
	keyInfo[4][7] = "laminar/B738/button/fmc" + side + "_Y";	// sw-52
	keyInfo[5][7] = "laminar/B738/button/fmc" + side + "_Z";	// sw-53
	keyInfo[6][7] = "laminar/B738/button/fmc" + side + "_SP";	// sw-54
	keyInfo[7][7] = "laminar/B738/button/fmc" + side + "_del";		// sw-55
	keyInfo[8][7] = "laminar/B738/button/fmc" + side + "_slash";	// sw-56
	keyInfo[1][8] = "laminar/B738/button/fmc" + side + "_clr";		// sw-57
	keyInfo[2][8] = "laminar/B738/button/fmc" + side + "_period";	// sw-58
	keyInfo[3][8] = "laminar/B738/button/fmc" + side + "_0";	// sw-59
	keyInfo[4][8] = "laminar/B738/button/fmc" + side + "_minus";	// sw-60
	keyInfo[5][8] = "laminar/B738/button/fmc" + side + "_7"; 	// sw-61
	keyInfo[6][8] = "laminar/B738/button/fmc" + side + "_8";	// sw-62
	keyInfo[7][8] = "laminar/B738/button/fmc" + side + "_9";	// sw-63
	keyInfo[8][8] = "laminar/B738/button/fmc" + side + "_4";	// sw-64
	keyInfo[1][9] = "laminar/B738/button/fmc" + side + "_5";	// sw-65
	keyInfo[2][9] = "laminar/B738/button/fmc" + side + "_6";	// sw-66
	keyInfo[3][9] = "laminar/B738/button/fmc" + side + "_1";	// sw-67
	keyInfo[4][9] = "laminar/B738/button/fmc" + side + "_2";	// sw-68
	keyInfo[5][9] = "laminar/B738/button/fmc" + side + "_3";	// sw-69

}

ZiboFMC::~ZiboFMC() {
	// TODO Auto-generated destructor stub
}

std::string ZiboFMC::getName() {

	return side == "1" ? "ZIB0" : "ZIB1";
}

void ZiboFMC::init() {

	Screen::getInstance()->initialize(24);
	Screen::getInstance()->clearScreen();
	{
		std::lock_guard<std::mutex> lock(lineMutex);
		lineParts.clear();
	}
	LEDs::getInstance()->setAllLEDs(0);
	LEDs::getInstance()->setLED(LEDs::LED_BG, 1);
	subscribeDataRefs();
}



void ZiboFMC::subscribe (std::string dataref) {

	FMCManager::getInstance()->subscribeDataRef (dataref);
	//xplaneUDPClient->subscribeDataRef(dataref+"[0][23]", 3);
}

void ZiboFMC::unsubscribe(std::string dataref) {

	FMCManager::getInstance()->unsubscribeDataRef(dataref);
}

// All the parts of the Zibo CDU screen we use:
//   LineNN_L  large line (base)          LineNN_X  small line (base)
//   LineNN_S  small font on large line   LineNN_LX white text on small line
//   LineNN_I  "inverse" text, e.g. LEGS  LineNN_GX green text on small line
//             waypoint names
//   LineNN_M  magenta, LineNN_G green    Line00_C  title, Line04_SI
//   Line_entry scratchpad, Line_entry_I
std::vector<std::string> ZiboFMC::lineDatarefs() {

	std::vector<std::string> refs;
	std::string base = "laminar/B738/fmc" + side + "/";
	char buf[8];
	for (int n = 0; n <= 6; n++) {
		snprintf(buf, sizeof(buf), "Line%02d_", n);
		for (const char * part : { "L", "S", "I", "M", "G" }) {
			refs.push_back(base + buf + part);
		}
		if (n > 0) {
			for (const char * part : { "X", "LX", "GX" }) {
				refs.push_back(base + buf + part);
			}
		}
	}
	refs.push_back(base + "Line00_C");
	refs.push_back(base + "Line04_SI");
	refs.push_back(base + "Line_entry");
	refs.push_back(base + "Line_entry_I");
	return refs;
}

std::string ZiboFMC::execLightDataref() {
	// captain = fmc1; side is "1" or "2", never "0"
	return side == "1" ? "laminar/B738/indicators/fmc_exec_lights"
			: "laminar/B738/indicators/fmc_exec_lights_fo";
}

void ZiboFMC::subscribeDataRefs() {

	// These are the datarefs that we need when we're active.
	for (const std::string & ref : lineDatarefs()) {
		subscribe(ref);
	}
	subscribe("laminar/B738/fmc/fmc_message");
	subscribe(execLightDataref());
}


void ZiboFMC::deInit () {

	for (const std::string & ref : lineDatarefs()) {
		unsubscribe(ref);
	}
	unsubscribe("laminar/B738/fmc/fmc_message");
	unsubscribe(execLightDataref());
}

void ZiboFMC::keyPressEvent(int row, int col) {
	auto rowV = keyInfo.find(row);
	if (rowV == keyInfo.end()) {
		cerr << "Invalid row " << row << endl;
		return;
	}

	auto colV = rowV->second.find(col);
	if (colV == rowV->second.end()) {
		cerr << "Invalid col " << col << endl;
		return;
	}

	cerr << "zibo keypress on r=" << rowV->first << " c=" << colV->first
			<< " \"" << colV->second << "\"" << endl;

	FMCManager::getInstance()->sendCommand(colV->second);
}

void ZiboFMC::keyReleaseEvent(int row, int col) {

	// do nothing?
}

static bool hasText(const std::string & s) {
	return s.find_first_not_of(' ') != std::string::npos;
}

// Redraw one screen line from its stored parts: the base part first
// (spaces erase), then each overlay in order (spaces are transparent).
// Caller holds lineMutex.
void ZiboFMC::redrawLine(int screenLine, const std::string & basePart,
		const std::vector<std::pair<std::string, char>> & overlays) {

	std::map<std::string, std::string> & parts = lineParts[screenLine];

	// zibo sends " " when it means clear the entire line, so pad to full width
	std::string base = parts[basePart];
	base.resize(24, ' ');
	Screen::getInstance()->drawLine(0, screenLine, base);

	for (const auto & ov : overlays) {
		auto it = parts.find(ov.first);
		if (it != parts.end() && hasText(it->second)) {
			Screen::getInstance()->drawLine(0, screenLine, it->second, false, ov.second);
		}
	}
}


void ZiboFMC::receiveDataRef(std::string type, std::string dataref,
		std::string value) {

	ostringstream buf;
	buf << "ZiboFMC got [" << dataref << "|" << value << "]" << endl;
	cerr << buf.str() << endl;
	syslog (LOG_INFO, "%s", buf.str().c_str());


	// Each screen line is built from several parts (see lineDatarefs()).
	// The Zibo only re-sends the parts that changed, and drawing a base part
	// erases the row, so keep every part and redraw the whole row whenever
	// any part changes. Otherwise text in one part (e.g. a green "ON", or
	// LEGS waypoint names sent in _I) vanishes when another part changes.
	static const std::vector<std::pair<std::string, char>> largeOverlays = {
		{ "S", 'W' }, { "I", 'W' }, { "SI", 'W' }, { "C", 'W' }, { "M", 'M' }, { "G", 'G' } };
	static const std::vector<std::pair<std::string, char>> smallOverlays = {
		{ "LX", 'W' }, { "GX", 'G' } };
	static const std::vector<std::pair<std::string, char>> entryOverlays = {
		{ "I", 'W' } };

	regex r("^laminar/B738/fmc" + side + "/Line(\\d+)_([A-Z]+)$");
	smatch m;
	if (regex_match(dataref, m, r)) {

		int zline = stoi(m[1]);
		string part = m[2];

		// zibo uses '*' for the square entry box. we don't have that
		// so we use '_' instead.
		replace (value.begin(), value.end(), '*', '_');

		std::lock_guard<std::mutex> lock(lineMutex);
		if (part == "X" || part == "LX" || part == "GX") {
			// small line above large line zline
			int screenLine = 1 + ((zline - 1) * 2);
			lineParts[screenLine][part] = value;
			redrawLine(screenLine, "X", smallOverlays);
		} else {
			int screenLine = zline * 2;
			lineParts[screenLine][part] = value;
			redrawLine(screenLine, "L", largeOverlays);
		}
	}

	else if (dataref == "laminar/B738/fmc" + side + "/Line_entry"
			|| dataref == "laminar/B738/fmc" + side + "/Line_entry_I") {
		replace (value.begin(), value.end(), '*', '_');
		std::lock_guard<std::mutex> lock(lineMutex);
		bool inv = dataref.compare(dataref.size() - 2, 2, "_I") == 0;
		lineParts[13][inv ? "I" : "E"] = value;
		redrawLine(13, "E", entryOverlays);
	}


	else if (dataref == "laminar/B738/indicators/fmc_exec_lights" || dataref == "laminar/B738/indicators/fmc_exec_lights_fo") {
		LEDs::getInstance()->setLED(LEDs::LED_EXEC, value != "0");
	}

	else if (dataref == "laminar/B738/fmc/fmc_message") {
		LEDs::getInstance()->setLED(LEDs::LED_MSG, value != "0");
	}

}

