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
#include "ddwrap.h"
#include "console.h"
#include "con-logger.h"
#include "globals.h"// for using the program objects
#include "postinstall.h"// for using the functions related to post-installation tasks
#include "programinfo.h"// for general info about the program itself
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "gameglobals.h"// for using the game objects

#include <string.h>

#ifdef EMSCRIPTEN
	#include "emscripten.h"

const int emscripten_fps = desiredFramesPerSecond;
void runFrame() {
	static bool running = false;
	static int counter = 0;
	int counter2 = counter;
	counter++;

	if (!running) {
		running = true;
		mainLoopObject.executeMainLoop(desiredFramesPerSecond,
			maxExecutedFramesWithoutDrawing, msecToCalculateFPS);
		running = false;
	}
}
#endif

/*
#include <windows.h>
#include <stdio.h>
#include "diwrap.h"
#include "audiowrap.h"
#include "gamecode.h"// for accessing to the code related only with the game
#include "resmanager.h"// for using the functions which manage the resources and operate their loading and unloading
#include "winutil.h"// for accessing to some useful functions related to low level windows specific code
#include "staging.h"// for the management of stages, levels, and the switching between these
#include "introcode.h"// for accesing to the methods related to the program zone corresponding to the intro
#include "menucode.h"// for accesing to the methods related to the program zone corresponding to the game menues

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	timeWrapperClass* timePtr;// for using in the multiplier command

	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_F12:// extremely fast exit
			DestroyWindow(hwnd);
			return 0;
		}
		break;
	case WM_CHAR:
		// manage the showing / hiding the console
		if (wParam=='\t') {
			showConsole=!showConsole;

			break;// don't let this key go to the console
		}

		// manage the high score name typing
		if (!showConsole && isCurrentlyTypingNameHS) {
			if (!executeHighScoreTyping(wParam)) {
				// error, just log it
				logger.logLine(
					"Error while managing the score name typing");
			}
		}

		if (!showConsole) break;

		// filter some control keys
		if (wParam==VK_ESCAPE) break;

		console.keyUp(wParam);

		if (wParam=='\r') {
			// check the command
			char* parammie;

			if (!console.getTextParam(parammie)) return 0;
			parammie=strlwr(parammie);

			// CLIPSPRITES
			// Command for deciding whether the sprites are
			// clipped or not. Useful for test purposes.
			if (strcmp("clipsprites", parammie)==0) {
				// change the clip sprites flag
				dbgClipSprites=!dbgClipSprites;

				// inform the change
				logger.logLine("Clip sprites from game world: %s",
					true, true, true, dbgClipSprites?"true":"false");
			}

			// ADVANCESTAGE
			// Command for advancing once in the stageLevels succession
			if (strcmp("advancestage", parammie)==0) {
				// tell in advance what we'll do
				logger.logLine("advanceStage command executed:");

				// advance the stage level!
				if (!advanceStage()) {
					// error, notice it
					logger.logLine("Couldn't advance the stageLevel.");
				}
			}

			// DONTEVERDIE
			// Command for deciding whether the player's energy is going
			// to be decrementable or not
			if (strcmp("donteverdie", parammie)==0) {
				bool last;

				// change the inmortality flag on the players
				vectorListNodeStruct<playerUnitStruct>* nodeP;
				for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
					nodeP->data.decrementableEnergy=!
						nodeP->data.decrementableEnergy;
					last=nodeP->data.decrementableEnergy;
				}

				// inform the change
				if (last) {
					logger.logLine("DontEverDie: player won't ever die");
				}else{
					logger.logLine("DontEverDie: player's energy decrementability is on");
				}
			}

			// FULLENERGY
			// Command for giving full energy to every player.
			if (strcmp("fullenergy", parammie)==0) {
				// add much energy to every player
				vectorListNodeStruct<playerUnitStruct>* nodeP;
				for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
					nodeP->data.energyLeft=1000;
				}

				// inform it
				logger.logLine("FullEnergy command executed");
			}

			// ONELIFE
			// Command for making the player have an only life
			if (strcmp("onelife", parammie)==0) {
				// make every player be almost death
				vectorListNodeStruct<playerUnitStruct>* nodeP;
				for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
					nodeP->data.energyLeft=1;
				}

				// inform it
				logger.logLine("OneLife command executed");
			}

			// WAVEGENACTIVE
			// Command for switching the "nowGenerating" flag of a given
			// wave generator (1 and 2 for sheeps, 3 for brains)
			if (strcmp("wavegenactive", parammie)==0) {
				int wavegenNum;

				// get the wave generator number
				if (!console.getIntegerParam(wavegenNum)) {
					logger.logLine("WaveGenActive requires a numeric parameter");
					break;
				}

				if (wavegenNum<1 && wavegenNum>3) wavegenNum=1;

				// add much energy to every player
				vectorListNodeStruct<waveGenUnitStruct>* nodeWG;
				for (nodeWG=waveGenList.getFirstNode(); nodeWG!=NULL; nodeWG=waveGenList.getNextNode(nodeWG)) {
					switch(wavegenNum) {
					case 1:// white sheeps
						if (nodeWG->data.unitType==
							FILE_GAMEDATA_WAVEGEN_WHITESHEEPS) {

							nodeWG->data.generatingNow=
								!nodeWG->data.generatingNow;
						}
						break;
					case 2:// black sheeps
						if (nodeWG->data.unitType==
							FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS) {

							nodeWG->data.generatingNow=
								!nodeWG->data.generatingNow;
						}
						break;
					case 3:// brains
						if (nodeWG->data.unitType==
							FILE_GAMEDATA_WAVEGEN_HUMANHEADS) {

							nodeWG->data.generatingNow=
								!nodeWG->data.generatingNow;
						}
						break;
					}
				}

				// inform it
				logger.logLine("waveGenActive command executed for %i",
					true, true, true, wavegenNum);
			}

			// STOPWAVEGENS
			// Command for making all the wave generators stop generating
			// stuff
			if (strcmp("stopwavegens", parammie)==0) {
				// stop all the wave gens
				vectorListNodeStruct<waveGenUnitStruct>* nodeWG;
				for (nodeWG=waveGenList.getFirstNode(); nodeWG!=NULL; nodeWG=waveGenList.getNextNode(nodeWG)) {
						nodeWG->data.generatingNow=false;
				}

				// stop the automatic level switching too
				gameWorld.timeLeftToNextStage=0;

				// inform it
				logger.logLine("StopWaveGens command executed");
			}

			// GOZOOM
			// Command for starting the screen zoom
			if (strcmp("gozoom", parammie)==0) {
				// execute zooming if it's not yet activate
				if (gameWorld.zoomed) {
					// no more zoom
					gameWorld.zoomed=false;
				}else{
					// activate the zoom
					gameWorld.zoomed=true;
					gameWorld.zoomCX=DD.getWidth()/2;
					gameWorld.zoomCY=DD.getHeight()/2;
					gameWorld.zoomBy=2;
				}

				// inform it
				logger.logLine("GoZoom command executed");
			}

			// TREMBLERS
			// Command for switching on and of the letSpritesTremble flag
			if (strcmp("tremblers", parammie)==0) {
				// switch the letSpritesTremble value
				letSpritesTremble=
					!letSpritesTremble;

				if (letSpritesTremble) {
					logger.logLine("Tremblers: trembling sprites active");
				}else{
					logger.logLine("Tremblers: trembling sprites inactive");
				}
			}

			// GOINGTOLINES
			// Command for switching on and off the
			// drawGoingToLines flag
			if (strcmp("goingtolines", parammie)==0
				|| strcmp("gtl", parammie)==0) {
				// switch the drawGoingToLines value
				drawGoingToLines=
					!drawGoingToLines;

				if (drawGoingToLines) {
					logger.logLine("goingToLines: 'going to' lines will be drawn");
				}else{
					logger.logLine("goingToLines: 'going to' lineswon't be drawn");
				}
			}

			// FOLLOWINGLINES
			// Command for switching on and off the
			// drawFollowingLines flag
			if (strcmp("followinglines", parammie)==0
				|| strcmp("fl", parammie)==0) {
				// switch the drawGoingToLines value
				drawFollowingLines=
					!drawFollowingLines;

				if (drawFollowingLines) {
					logger.logLine("followingLines: 'following' lines will be drawn");
				}else{
					logger.logLine("followingLines: 'following' lineswon't be drawn");
				}
			}

			// HANDLERPOINTS
			// Command for switching on and off the
			// drawHandlerPoints flag
			if (strcmp("handlerpoints", parammie)==0
				|| strcmp("hp", parammie)==0) {
				// switch the drawHandlerPoints value
				drawHandlerPoints=
					!drawHandlerPoints;

				if (drawHandlerPoints) {
					logger.logLine("handlerPoints: handler points will be drawn");
				}else{
					logger.logLine("handlerPoints: handler points won't be drawn");
				}
			}

			// BOUNDRECTS
			// Command for switching on and off the
			// drawBoundingRectangles flag
			if (strcmp("boundrects", parammie)==0) {
				// switch the drawBoundingRectangles value
				drawBoundingRectangles=
					!drawBoundingRectangles;

				if (drawBoundingRectangles) {
					logger.logLine("BoundRects: bounding rects will be drawn");
				}else{
					logger.logLine("BoundRects: bounding rects won't be drawn");
				}
			}

			// DRAWWAVEGENS
			// Command for switching on and off the drawWaveGenerators
			// flag, which tells whether we should draw the wave
			// generators (ie, the foreground) or not
			if (strcmp("drawwavegens", parammie)==0) {
				// switch the drawWaveGenerators value
				drawWaveGenerators=
					!drawWaveGenerators;

				if (drawWaveGenerators) {
					logger.logLine("DrawWaveGens: wave generators will be drawn");
				}else{
					logger.logLine("DrawWaveGens: wave generators won't be drawn");
				}
			}

			// DRAWVIRTUALWALLS
			// Command for switching on and off the drawVirtualWalls
			// flag, which tells whether we should draw the virtual
			// walls (that is, their boundaries) or not
			if (strcmp("drawvirtualwalls", parammie)==0 ||
				strcmp("dvw", parammie)==0) {

				// switch the drawVirtualWalls value
				drawVirtualWalls=
					!drawVirtualWalls;

				if (drawVirtualWalls) {
					logger.logLine("DrawVirtualWalls: virtual walls will be drawn");
				}else{
					logger.logLine("DrawVirtualWalls: virtual walls won't be drawn");
				}
			}

			// DRAWFALLINGLIMITS
			// Command for switching on and off the drawFallingLimits
			// flag, which tells whether we should draw the
			// limits for the falling objets or not
			if (strcmp("drawfallinglimits", parammie)==0 ||
				strcmp("dfl", parammie)==0) {
				// switch the drawFallingLimits value
				drawFallingLimits=
					!drawFallingLimits;

				if (drawFallingLimits) {
					logger.logLine("DrawFallingLimits: falling limits will be drawn");
				}else{
					logger.logLine("DrawFallingLimits: falling limits won't be drawn");
				}
			}

			// VIDEOCAPTURE
			// Command for switching on and off the video capture
			// (continuous screenshooting)
			if (strcmp("videocapture", parammie)==0) {
				// switch the keepTakingScreenshots value
				keepTakingScreenshots=
					!keepTakingScreenshots;

				if (keepTakingScreenshots) {
					logger.logLine("VideoCapture: started saving images to disk");
				}else{
					logger.logLine("BoundRects: stopped saving images to disk");
				}
			}

			// MULTIPLIER
			// Command for changing the value of the time multiplier
			if (strcmp("multiplier", parammie)==0) {
				timePtr=mainLoopObject.getTimerObject();
				if (timePtr->getMultiplier()==1) {
					// change the multiplier value
					timePtr->setMultiplier(0.25);// slow

					// inform it
					logger.logLine("Multiplier set to 0.25");
				}else if (timePtr->getMultiplier()==0.25) {
					// change the multiplier value
					timePtr->setMultiplier(4);// fast

					// inform it
					logger.logLine("Multiplier set to 4");
				}else{// for 4
					// change the multiplier value
					timePtr->setMultiplier(1);// common

					// inform it
					logger.logLine("Multiplier set to 1");
				}
			}

			// WAYSLOW
			// Command for setting the time multiplier to an extremely
			// slow speed
			if (strcmp("wayslow", parammie)==0 ||
				strcmp("ws", parammie)==0) {

				timePtr=mainLoopObject.getTimerObject();

				// change the multiplier value
				timePtr->setMultiplier(0.05);

				// inform the change
				logger.logLine("WaySlow: Multiplier set to %f",
					true, true, true,
					timePtr->getMultiplier());
			}

			// ULTRAFAST
			// Command for setting the time multiplier to an extremely
			// fast speed
			if (strcmp("ultrafast", parammie)==0) {
				timePtr=mainLoopObject.getTimerObject();

				// change the multiplier value
				timePtr->setMultiplier(20);

				// inform the change
				logger.logLine("UltraFast: Multiplier set to %f",
					true, true, true,
					timePtr->getMultiplier());
			}

			// LANGSWITCH
			// Command for switching the language currently active
			if (strcmp("langswitch", parammie)==0) {
				switch(currentLanguage) {
				case language_spanish:
					// switch it...
					currentLanguage=language_english;

					// inform the change
					logger.logLine("Language switched to english...");
					break;
				case language_english:
					// switch it...
					currentLanguage=language_spanish;

					// inform the change
					logger.logLine("Language switched to spanish...");
					break;
				}

				// set the current language in lua
				scriptMan.setCurrentLanguageInLua(currentLanguage);

				// save the change in the options file
				optionsFileObject.getOptionsPointer()->selectedLanguage=
					currentLanguage;
				optionsFileObject.save();
			}

			// STATEOFRESOURCES
			// Command for retrieving the state of all the resources
			if (strcmp("stateofresources", parammie)==0) {
				// execute the action
				logAllResourcesState();

				// inform it
				logger.logLine("stateOfResources command executed");
			}

			// LUACMD
			// Command for executing a lua command
			if (strcmp("luacmd", parammie)==0) {
				char* command;

				if (!console.getNoSpacedTextParam(command)) {
					logger.logLine("This command requires an extra text parameter");
				}else{
					// execute the lua command
					if (!scriptMan.doString(command)) {
						logger.logLine(
							"Failed executing the lua command");
					}
				}
			}

			// COMMANDLIST
			// Command for obtaining a list of commands
			if (strcmp("commandlist", parammie)==0) {
				// give the list of commands
				logger.logLine("1: clipSprites fullEnergy oneLife waveGenActive");
				logger.logLine("2: langSwitch stateOfResources");
				logger.logLine("3: goZoom tremblers boundRects drawWaveGens");
				logger.logLine("4: videoCapture multiplier waySlow commandList");
				logger.logLine("5: dontEverDie drawVirtualWalls ultraFast");
				logger.logLine("6: goingToLines advanceStage stopWaveGens");
				logger.logLine("7: followingLines handlerPoints drawFallingLimits");
				logger.logLine("8: luaCmd");
			}
		}
		return 0;
    case WM_SETCURSOR:
		// i don't want a cursor
 	    SetCursor(NULL);// set cursor...
		// further processing halted
	    return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ACTIVATEAPP:
		// Mark as active app or inactive app
		globalSetActiveState(BOOL(wParam)==TRUE);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
*/

int main(int argc, char* argv[])
{
/*	// we save the data we got...
	::hInstance=hInstance;
	::nCmdShow=nCmdShow;
*/
	// let's see what parameters are here for us...
	// -FINISHINSTALL is used for making the program go the
	// finishing installation mode
//	if (argc>1 && strcmp(strlwr(argv[1]), "-finishinstall")==0) {
	if (argc>1 && strcmp(argv[1], "-finishinstall")==0) {
		// it's time to finish the installation and go away
		logger.init("data/logs.txt");
		logger.logLine("");
		logger.logLine("************");
		logDateAndTime("Just installed at %s %s, executing post installation");

		if (!executePostInstallTasks(false))
			logger.logLine("Bad post installation process...");

		logger.logLine("***** INSTALLATION READY *****");
		logger.end();
		return 0;
	}

	// -NEVERSOUND is used for making the program execute in an
	// absolutely silent mode, not only temporarily disabling sound
	// but also never letting the program active the sound again
	// for the entire session.
//	if (strcmp(strlwr(szCmdLine), "-neversound")==0) {
	if (argc>1 && strcmp(argv[1], "-neversound")==0) {
		// go to silent mode
		programSoundActive=false;
	}

	// initialize all what's needed
	if (!initializeFirst()) {
		return false;
	}

	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Initialize first ok", true, false);
	#endif// PROGMODE_DEVELOPER_YES

	// Go to the first zone to be executed
	if (!switchToProgramZone(pgzon_gameIntro, 0, 0, true)) {
		// time to go away...
		closeAll("Could not set up first program zone...");
		return false;
	}

	#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Switch to first program zone ok", true, false);
	#endif// PROGMODE_DEVELOPER_YES

#ifdef EMSCRIPTEN
	emscripten_set_main_loop(runFrame, emscripten_fps, 1);
#else
	// go start the game
	if (!mainLoopObject.executeMainLoop(desiredFramesPerSecond, maxExecutedFramesWithoutDrawing, msecToCalculateFPS)) {
		logger.logLine("Bad game execution!");
	}

	// Terminate...
	closeAll("Time to go away...");
#endif

	return 0;
}
