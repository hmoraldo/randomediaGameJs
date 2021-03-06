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

#include "mainloop.h"// for accessing to the main loop object
#include "console.h"
#include "con-logger.h"
#include "introcode.h"// for accesing to the methods related to the program zone corresponding to the intro*/
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
/*#include "gameglobals.h"// for using the game objects*/
#include "globals.h"// for using the program objects
#include "gamecode.h"// for accessing to the code related only with the game
#include "menucode.h"// for accesing to the methods related to the program zone corresponding to the game menues
#include "programinfo.h"// for general info about the program itself
#include <SDL.h>

// MAINLOOPCLASS METHODS
// *********************

// informFramesPerSecond *
// We use this method to receive the amount of frames per second in a
// timely basis, and log it...
bool mainLoopClass::informFramesPerSecond(double fps) {
	logger.logLine("Frames per second: %f", true, true, true, fps);

	return true;
}// informFramesPerSecond

// beforeLoop ************
// Here we put whatever we should do before executing this loop.
bool mainLoopClass::beforeLoop() {
	return true;
}// beforeLoop

// afterLoop *************
// Here we put whatever we should do after executing this loop.
bool mainLoopClass::afterLoop() {
	// Free all the resources in use by switching to the null zone
	if (!switchToNullProgramZone())
		return false;

	return true;
}// afterLoop

// drawFrame *************
// Draw the current frame on screen.
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool mainLoopClass::drawFrame()
{
	// a static vector to be used in the construction of file names
	// for screenshooting
	static char bmpFileName[200];

	// temporary variable for the return value
	bool retval=false;

	// make sure it's all ok before drawing the frame...
	// (if it returns false, it wasn't able to restore it, neither
	// it will, as it's been a fatal error)
	if (!checkLostResources()) {
		logger.logLine("Nonrecover error when restoring resources");
		return false;
	}

	// call the drawFrame function of currentProgramZone in
	// currentProgramSubZone and currentProgramSubSubZone...
	switch(currentProgramZone) {
	case pgzon_actualGameRunning:// actual game is currently running
		retval=GameLoopCode::drawFrame();
		break;
	case pgzon_gameIntro:// program intro is currently being executed
		retval=IntroLoopCode::drawFrame();
		break;
	case pgzon_gameMenues:// program menues are currently being executed
		retval=MenuLoopCode::drawFrame();
		break;
	}
/*
	// draw the console in the screen (above everything else)
	drawConsole();

	// check if we shouldn't be taking a screenshot now
	if (takeASingleScreenshot || keepTakingScreenshots) {
		// take the screenshot
		#ifdef PROGMODE_DEVELOPER_YES
		logger.logLine("Screenshot to be taken next...", true, false);
		#endif// PROGMODE_DEVELOPER_YES

		// construct the bmp file name...
		#define SSHOTFILENAME "sshots\\sshot"
		#define SSHOTFILEEXT ".bmp"
		wsprintf(bmpFileName, "%s%i%s",
			SSHOTFILENAME,
			WinUtilCode::findFreeNumberedFile
				(SSHOTFILENAME, SSHOTFILEEXT),
			SSHOTFILEEXT);

		#ifdef PROGMODE_DEVELOPER_YES
		logger.logLine("Filename for screenshot selected.", true, false);
		#endif// PROGMODE_DEVELOPER_YES

		// save the screenshot itself
		DD.saveAsBmpFile(bmpFileName, true);
		logger.logLine("Screenshot saved as \"%s\".", true, true, true,
			bmpFileName);

		// clear the single screenshooting flag
		takeASingleScreenshot=false;
	}
*/
	// time to flip the screen buffers
	DD.flip();

	return retval;
}// drawFrame

// executeFrame ***********
// Execute a frame, without drawing it on screen. It's what I call
// "virtual frames".
// Only return false for non-recover errors.
bool mainLoopClass::executeFrame(DWORD frameNumber)
{
	bool retval=false;
	bool isLost;// used with dinput

	// Get the input...
	// Mouse...
	if (!mouse.getState(&isLost)) {
		if (isLost) {
			// try getting the info again, after acquiring the
			// device
			logger.logLine("Mouse lost");
			mouse.acquire();
			mouse.getState();
		} else {
			// any other error is bad news
			logger.logLine("Mouse error");
			return false;
		}
	}
	// Keyboard...
	if (!keyboard.getKeys(&isLost)) {
		if (isLost) {
			// try getting the info again, after acquiring the
			// device
			logger.logLine("Keyboard lost");
			keyboard.acquire();
			keyboard.getKeys();
		} else {
			// any other error is bad news
			logger.logLine("Keyboard error");
			return false;
		}
	}

/*	// check if it is time for making screenshots
	if (KEYEVENTDOWN(keyboard.keys, keyboard.latestKeys, DIK_F9)) {
		// take a screenshot
		#ifdef PROGMODE_DEVELOPER_YES
		logger.logLine("Screenshot marked to be taken...", true, false);
		#endif// PROGMODE_DEVELOPER_YES

		// mark this flag to let the drawer know we want a screenshot
		takeASingleScreenshot=true;
	}
*/
	// call the drawFrame function of currentProgramZone in
	// currentProgramSubZone and currentProgramSubSubZone...
	switch(currentProgramZone) {
	case pgzon_actualGameRunning:// actual game is currently running
		retval=GameLoopCode::executeFrame(frameNumber);
		break;
	case pgzon_gameIntro:// program intro is currently being executed
		retval=IntroLoopCode::executeFrame(frameNumber);
		break;
	case pgzon_gameMenues:// program menues are currently being executed
		retval=MenuLoopCode::executeFrame(frameNumber);
		break;
	}

        SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				exitLoop();
				break;
			default:
				break;
		}
	}

	return retval;
}// executeFrame


#ifdef EMSCRIPTEN
// in js we can't loop forever, so this version of the function doesn't loop, it is called once per frame, with callbacks
bool mainLoopClass::executeMainLoop(
	int framesToExecutePerSecond,// This value is the amount of frames that have to be executed every second (executeFrame), without caring about the real amount of frames (drawFrame) drawn.
	int maxExecutedFramesWithoutDrawing,// see above
	DWORD msecToCalculateFPS)// how many milliseconds have to pass before informing the current amount of frames per second
{
	static bool ready = false;
	bool ended=false;

	if (!ready) {
		ready = true;
		if (!setupMainLoop(framesToExecutePerSecond)) return false;
	}

	if (!loopCycle(framesToExecutePerSecond, maxExecutedFramesWithoutDrawing, msecToCalculateFPS, &ended)) {
		return false;
	}

	if (ended) {
		afterLoop();
		return false;// treat quitting in the browser as an error
	}

	return true;
}
#endif
