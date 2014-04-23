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

#include <SDL.h>
#include "globals.h"// for using the program objects
#include "programinfo.h"// for general info about the program itself
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "gameglobals.h"// for using the game objects
#include "postinstall.h"// for using the functions related to post-installation tasks
#include "resfonts.h"// for accessing to the font resources
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include "resinfoman.h"// for managing the information related with the use of resources, ie: animation scripts
#include <time.h>
#include <stdlib.h>

// The definitions...

// Variables and stuff for language switching
languageEnum currentLanguage;// language currently selected

// Program compiled version info (a string containing such information)
char programVersionInfoString[]=PROGINFO_FULLVERSIONTEXT;
// A string containing the copyright information.
char programCopyrightString[]=PROGINFO_COPYRIGHT;
/*
// Data for the windows management
HWND hwnd;
HINSTANCE hInstance;
int nCmdShow;
*/

// Our directDraw object
directDrawClass DD;

// Our keyboard and mouse objects
diKeyboardClass keyboard;
diMouseClass mouse;

// Data for the console
consoleClass console;
conLoggerClass logger;

// Is the sound currently active? Global for the entire program.
// Notice this boolean doesn't tell if the sound is active or not in
// a given moment, it tells whether sound can or cannot be used
// during the entire execution of the program.
bool programSoundActive=true;
// This other boolean tells, instead, whether the sound could be
// used or not in a given moment, according to the current program
// configuration
bool programSoundEnabled=true;

// Data for the console... conditional compilation depending on the
// developing compilation mode on or off.
#ifdef PROGMODE_DEVELOPER_YES
	bool showConsole=false;
#else// PROGMODE_DEVELOPER_NO
	bool showConsole=false;
#endif// PROGMODE_DEVELOPER_YES

// Our time object
timeWrapperClass timeWrapper;// interesting: emscripten won't work if this is named time

// Frames per second
// This value is the amount of frames that have to be executed
// every second (executeFrame), without caring about the real amount of
// frames (drawFrame) drawn.
const int desiredFramesPerSecond=200;

// These consts store the max amount of frames which can be executed
// without drawing a frame on screen. ByTime is used together with
// desiredFramesPerSecond to know the real amount.
// Both are used to enable a mechanism which avoids program halts
// when executeFrame is very slow, then if the program executes too
// much frames at once, it'll go even slower, which will make the program
// execute even more frames, and so on until falling in a deadlock.
// To avoid this, we put this limit to the frames executed (so, if
// either executeFrame or drawFrame goes too much slow, the amount of
// frames skipped -executed- will stop being proportional to the time
// elapsed for every physic frame).
// This system has been tested and works very well.
const double maxExecutedFramesWithoutDrawingByTime=0.3;// 1 seconds max, change this number according to the use
const int maxExecutedFramesWithoutDrawing=// unchangable... use maxExecutedFramesWithoutDrawingByTime instead
	(int)(maxExecutedFramesWithoutDrawingByTime*desiredFramesPerSecond);
const int msecToCalculateFPS=5000;


// Here follows some variables which store information related to the
// program itself.
mainLoopClass mainLoopObject;// this is the object used for executing the main program loop

// Program sections:
int currentProgramZone=pgzon_emptyZone;// program zone the program is currently in
int currentProgramSubZone=0;// program sub zone the program is currently in
int currentProgramSubSubZone=0;// program sub sub zone the program is currently in
// Previous program sections:
int previousProgramZone=pgzon_invalidZone;// previous program zone the program was in
int previousProgramSubZone=0;// previous program sub zone the program was in
int previousProgramSubSubZone=0;// previous program sub sub zone the program was in

// Some global functions related to the resources in globals.*.

// initializeFirst ******
// initialize the first resources needed (on program start)
bool initializeFirst()
{
	// initialize our logger (only for files for a while)
	logger.init("data/logs.txt");
	logger.logLine("");
	logger.logLine("************");
	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Logging in developer mode.", true, false);
	#endif// PROGMODE_DEVELOPER_YES

	// Log the compilation information
	logger.logLine("Executable info: %s",
		true, true, true,// file, console, noUnselError
		programVersionInfoString);
	// tell it was started without sound
	if (!programSoundActive) logger.logLine("Sound disabled");
	// Log the date and time...
	logDateAndTime("New execution at %s, %s");

	// Initialize the random number generator
	srand(time(NULL));

	// Initialize the game script manager
	if (!scriptMan.init()) {
		closeAll("Script manager initialization failed");
		return false;
	}


	// Game initialization
	// Let's set the data structures up first...
	if (!setupGameDataStructures()) {
		closeAll("setupGameDataStructures failed");
		return false;
	}
/*
	// Create a directory for screenshots
	CreateDirectory("sshots", NULL);

	// We create a window first.
	::hInstance=hInstance;
	hwnd=WinUtilCode::createWindow(
		hInstance, nCmdShow, &WndProc,
		PROGINFO_WINCLASSNAME,// class name
		PROGINFO_WINDOWTEXT);// windows text
*/

	SDL_Init(SDL_INIT_VIDEO);

	// Init DirectDraw.
	if (!DD.init()) {
		// error
		closeAll("DD.init failed.");
		return false;
	}

	// set 800x600x16 video mode, with an only backbuffer.
	if (!DD.setVideoMode(
		scriptMan.getIntVariable("screenWidth"),
		scriptMan.getIntVariable("screenHeight"),
		scriptMan.getIntVariable("screenBpp"), 1)) {

		// error
		closeAll("DD.setVideoMode failed.");
		return false;
	}
	// let's fill all with black
	DD.fillAllDword(0);
	DD.flip();

	// Time to do all those tasks that are remaining just after
	// installation, showing the messages that are necessary...
	if (!executePostInstallTasks(true)) {
		// error
		closeAll("executePostInstallTasks failed.");
		return false;
	}
/*
	// let's initialize the audio system
	// taking the parameters from a wave file...
	if (programSoundActive) {
		if (!generalAudioClass::initializeAudioFromWave(
			hwnd, FILE_FILENAMES_WAV_INITWAV, true)) {
			// error
			if (true) {
				// don't close the program, rather deactivate the
				// sound execution
				logger.logLine("generalAudioClass::initializeAudioFromWave failed. Going never sound instead.");
				programSoundActive=false;

				// set the current sound state in lua
				scriptMan.setSoundEnabledInLua(
				programSoundEnabled, programSoundActive);
			}else{
				// we used to close the program... now we are nicer
				closeAll("generalAudioClass::initializeAudioFromWave failed.");
				return false;
			}
		}
	}
*/
	if (!keyboard.init()) {
		// error
		closeAll("keyboard.init failed.");
		return false;
	}

	if (!mouse.init()) {
		// error
		closeAll("mouse.init failed.");
		return false;
	}

	if (!mouse.setUpMouse() || !keyboard.setUpKeyboard()) {
		// error
		closeAll("setUpMouse and keyword failed.");
		return false;
	}

	if (!console.init(400, 100)) {
		// error
		closeAll("console.init failed.");
		return false;
	}

	// initialize our time object
	if (!timeWrapper.init()) {
		// error
		closeAll("timeWrapper.init failed.");
		return false;
	}
/*
	// attach the console to our logger
	if (!logger.attachConsole(&console)) {
		logger.logLine("con-logger attach failed.");
	}

	// Log the hardware capabilities
	logger.logLine("Hardware info:");
	logger.logLine("Hardware blts: %s", true, true, true,
		(DD.doesHardwareBlt()?"Yes":"No"));
	logger.logLine("Hardware transparent blts: %s", true, true, true,
		(DD.doesHardwareTransparentBlt()?"Yes":"No"));
	logger.logLine("Hardware stretch x: %s", true, true, true,
		(DD.doesHardwareStretchX()?"Yes":"No"));
	logger.logLine("Hardware stretch y: %s", true, true, true,
		(DD.doesHardwareStretchY()?"Yes":"No"));
	logger.logLine("Hardware mirror x: %s", true, true, true,
		(DD.doesHardwareMirrorX()?"Yes":"No"));
	logger.logLine("Hardware mirror y: %s", true, true, true,
		(DD.doesHardwareMirrorY()?"Yes":"No"));

	// let's see what's the color info this time
	logger.logLine("Palettized: %s", true, true, true,
		(DD.isPalettized()?"Yes":"No"));
	logger.logLine("Red info: mask %i, offset %i, size %i",
		true, true, true,// log everywhere
		DD.rMask, DD.rOff, DD.rSize);
	logger.logLine("Green info: mask %i, offset %i, size %i",
		true, true, true,// log everywhere
		DD.gMask, DD.gOff, DD.gSize);
	logger.logLine("Blue info: mask %i, offset %i, size %i",
		true, true, true,// log everywhere
		DD.bMask, DD.bOff, DD.bSize);
*/
	// Set up the fonts
	if (!setUpProgramFonts(false)) {
		logger.logLine("set up of program fonts failed");
	}

	return true;// all ok
}// initializeFirst

// checkLostResources ******
// Check lost surfaces and buffers, and take care of them...
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool checkLostResources()
{
/*	// "Return true" is used here when the resources can't be restored
	// yet, but will be in the future (seemingly)

	// GRAPHIC RESOURCES
	// graphic surfaces
	if (DD.isReady() && DD.isLost()) {
		logger.logLine("Screen lost");
		if (!DD.restore()) return true;
	}
	if (console.isReady() && console.isLost())
		if (!console.restore()) return true;

	// SOUND RESOURCES
	// Restore the main buffer
	if (programSoundActive) {
		if (generalAudioClass::isAudioSystemReady() &&
			generalAudioClass::isLostMain()) {

			logger.logLine("Audio lost");
			if (!generalAudioClass::restoreMain()) return true;
		}
	}

	// Reload all the resources in the resources object list.
	if (!reloadAllLostResources()) return false;
*/
	// all ok
	return true;
}// checkLostResources

// closeAll *************
// destroy all the opened resources
bool closeAll(char* errorText)
{
	// log date and time first
	logDateAndTime("Closing at %s, %s");

	if (errorText!=NULL) logger.logLine(errorText);

	timeWrapper.end();

	// game stuff...
	playerList.end();
	enemyList.end();
	bulletList.end();
	deinitializeVirtualWalls();
	closeResourcesInfo();
	destroyAllResourcesData();

	// general stuff
	scriptMan.close();
	setUpProgramFonts(true);// destroy the program fonts
	console.end();
	mouse.end();
	keyboard.end();
/*	if (programSoundActive) generalAudioClass::closeAudio();
*/
	// shut down the screen...
	DD.closeVideoMode();
	DD.end();

	// finish the logger, at last...
	logger.logLine("And closed with no errors", true, false, true);
	logger.end();

	return true;// all ok
}// closeAll

// shows the console on screen
bool drawConsole()
{
	// let's show the console
	if (showConsole)
		if (!console.draw(DD, 10, 10)) return false;

	return true;// all ok
}// showConsole


// GLOBAL LOOP MODIFIERS
// Some globals functions which modify the software state, whatever the
// loop in execution is.

// globalExit *****************
// exits the program whatever its state is
bool globalExit() {
	// switch to the null zone

	// now quit the loop itself
	mainLoopObject.exitLoop();

	return true;
}// globalExit

// globalSetActiveState *******
// sets the paused state of the entire software (useful for alt tabs)
bool globalSetActiveState(bool actived) {
	mainLoopObject.setActiveState(actived);

	return true;
}// globalPause

// Other useful functions

// logDateAndTime ******
// This function logs the date and time together with a text...
// String to use has to contain %s first, for the date, and %s, for the
// time strings to be added there.
bool logDateAndTime(char* stringToUse)
{
	return true;// all ok
}// logDateAndTime

