/*

Copyright (c) 2003 - 2014 Horacio Hernan Moraldo

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held liable
for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "resmanager.h"// for using the functions which manage the resources and operate their loading and unloading
#include "globals.h"// for using the program objects
#include "introcode.h"// for accesing to the methods related to the program zone corresponding to the intro
#include "menucode.h"// for accesing to the methods related to the program zone corresponding to the game menues
#include "gamecode.h"// for accessing to the code related only with the game
#include "programinfo.h"// for general info about the program itself

// switchToProgramZone *******
// This function manages the switch from a program zone to another one.
bool switchToProgramZone(
	programZonesEnum programZone,// program zone to look at
	DWORD programSubZone,// program sub zone to look at
	DWORD programSubSubZone,// program sub sub zone to look at
	bool reInitializeAllResources)// if true, set up more slowly, but keep memory fragmentation to a minimum
{
	// switch it in all the cases
	logger.logLine("Switching to program zone %i",
		true, false, true, programZone);

	// first, pause the clock so that we don't use the clock time for
	// the load of data and all that stuff
	if (mainLoopObject.getTimerObject()->isReady() &&
		!mainLoopObject.getTimerObject()->pause()) return false;

	// call the afterLoop function of currentProgramZone in
	// currentProgramSubZone and currentProgramSubSubZone...
	switch(currentProgramZone) {
	case pgzon_actualGameRunning:// actual game is currently running
		if (!GameLoopCode::afterLoop()) return false;
		break;
	case pgzon_gameIntro:// program intro is currently being executed
		if (!IntroLoopCode::afterLoop()) return false;
		break;
	case pgzon_gameMenues:// program menues are currently being executed
		if (!MenuLoopCode::afterLoop()) return false;
		break;
	}

	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Program zone switch after loop ok", true, false);
	#endif// PROGMODE_DEVELOPER_YES

	// set up the resources for this game zone
	if (!setUpResources(programZone)) return false;

	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Program zone switch set up resources ok", true, false);
	#endif// PROGMODE_DEVELOPER_YES

	// save the previous program zone data
	previousProgramZone=currentProgramZone;
	previousProgramSubZone=currentProgramSubZone;
	previousProgramSubSubZone=currentProgramSubSubZone;
	// update the current zone data
	currentProgramZone=programZone;
	currentProgramSubZone=programSubZone;
	currentProgramSubSubZone=programSubSubZone;

	// call the beforeLoop function of programZone in
	// programSubZone and programSubSubZone...
	switch(programZone) {
	case pgzon_actualGameRunning:// actual game is currently running
		if (!GameLoopCode::beforeLoop()) return false;
		break;
	case pgzon_gameIntro:// program intro is currently being executed
		if (!IntroLoopCode::beforeLoop()) return false;
		break;
	case pgzon_gameMenues:// program menues are currently being executed
		if (!MenuLoopCode::beforeLoop()) return false;
		break;
	}

	// let the timer object continue working now
	if (mainLoopObject.getTimerObject()->isReady() &&
		!mainLoopObject.getTimerObject()->play()) return false;

	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Program zone switch ok", true, false);
	#endif// PROGMODE_DEVELOPER_YES

	return true;// all ok
}// switchToProgramZone


// switchToNullProgramZone *******
// This function manages the switch to the null program zone.
bool switchToNullProgramZone()
{
	// do the switch...
	return switchToProgramZone(
		pgzon_emptyZone, 0, 0, true);
}// switchToNullProgramZone
