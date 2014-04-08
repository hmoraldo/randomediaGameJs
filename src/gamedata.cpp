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

#include "gamedata.h"
#include "globals.h"// for using the program objects
#include "resmanager.h"// for using the functions which manage the resources and operate their loading and unloading
#include "gameglobals.h"// for using the game objects
#include "players.h"// for managing the players
#include "wavegens.h"// for managing the wave generators
#include "staging.h"// for the management of stages, levels, and the switching between these
/*#include "soundman.h"// for using the functions related to sound management*/
#include "scriptman.h"// used for accessing to the script manager object
/*#include "gamesound.h"// for accessing the functions that convert game events to sound executions*/

// WORLDDATACLASS methods

// This method sets all up to start playing in a zone.
bool worldDataClass::initializeGame(int gameZoneNumber)
{
	logger.logLine("Initializing game world data");

	// set the basic stuff up
	isGamePaused=false;// don't pause it yet

	// Stage & level counting
	currentStage=gmstg_emptyStage;// current stage, cyclic value
	currentLevel=0;// current level, there are two stages per level
	timeLeftToNextStage=0;// time left (in virtual frames), when it's zero, it's time to advance to the next stageLevel
	// timeLeftToNextStage can be anything actually, as it's changed later

	// Restart the game object ids counter before creating new object
	nextGameObjectID=FILE_GAMEDATA_FIRST_ID;

	// before starting, stop all the playing sounds (belonging to other
	// program zones)
/*	if (!stopAllSoundResources()) return false;

	// Play the music for the game starting now
	executeIngameMusic(0);*/

	// set the graphical stuff
	srfGameScreen=&DD;
	srfBg=getSurfaceByResourceName("srfClassicLevelBg");
	xCamera=0; yCamera=0;
	screenX=0; screenY=0;
	wCamera=scriptMan.getIntVariable("screenWidth");
	hCamera=scriptMan.getIntVariable("screenHeight");
	bgOffX=0; bgOffY=0;// background offset...

	// set the speed camera and background stuff
	xSpeedCamera=0;
	ySpeedCamera=0;
	bgSpeedX=0; bgSpeedY=0;// speed of the background offset

	// Set the zoom related stuff
	zoomed=false;// tells whether we zoom the screen or not
	zoomBy=1;// tells the multiplier used for zooming
	zoomCX=0; zoomCY=0;// tells the coordinates where zooming is centered

	// Data for the game over
	maxGameFinishedTimeLeft=int(1*double(desiredFramesPerSecond));// value to set gameFinishedTimeLeft every time the game is finished
	gameFinishedTimeLeft=-1;// time left (in virtual frames) before the game is supposed to finish, -1 for 'not finishing'
	showGameOverMessage=false;// is time to show the game over message?

	// Data for statistics
	scoreInHits=0;// how much of the score was got from hits to stuff carriers and other stuff?
	scoreInFreeWhiteSheeps=0;// how much of the score was got from free white sheeps?
	scoreInFreeBlackSheeps=0;// how much of the score was got from free black sheeps?
	scoreInFreeHeads=0;// how much of the score was got from free heads?

	// Set other in-game data
	yGround=// distance from the top to the ground
		scriptMan.getIntVariable("yGround");
	yGroundForPlayer=// max distance from the top to the feet of the player
		scriptMan.getIntVariable("yGroundForPlayer");

	// Set the data for falling objects
	minXFallenWSheeps=// minimum x for falling white sheeps
		scriptMan.getIntVariable("minXFallenWSheeps");
	maxXFallenWSheeps=// maximum x for falling white sheeps
		scriptMan.getIntVariable("maxXFallenWSheeps");
	minXFallenBSheeps=// minimum x for falling black sheeps
		scriptMan.getIntVariable("minXFallenBSheeps");
	maxXFallenBSheeps=// maximum x for falling white sheeps
		scriptMan.getIntVariable("maxXFallenBSheeps");
	minXFallenHeads=// minimum x for falling heads
		scriptMan.getIntVariable("minXFallenHeads");
	maxXFallenHeads=// maximum x for falling heads
		scriptMan.getIntVariable("maxXFallenHeads");

	// Set up the players...
	if (!createPlayerObjects()) {
		deinitializeGame();
		return false;
	}

	// Set up the wave generators...
	if (!createWaveGenObjects()) {
		deinitializeGame();
		return false;
	}

	// Reset enemies and other game objects
	// Enemies
	if (!enemyList.resetList()) {
		deinitializeGame();
		return false;
	}
	// Bullets
	if (!bulletList.resetList()) {
		deinitializeGame();
		return false;
	}

	// And now, start in the first stageLevel
	if (!advanceStage()) {
		deinitializeGame();
		return false;
	}

	return true;// all ok
}// initializeGame

// Deinitialize everything (less resources) which was initialized in
// startGame.
bool worldDataClass::deinitializeGame()
{
/*	// musical stuff first
	waveFileClass* muswavIcyMusic;
	// load the sound data...
	muswavIcyMusic=getWaveSoundByResourceName("muswavIcyMusic");
	if (muswavIcyMusic==NULL) {
		logger.logLine("muswavIcyMusic couldn't be get for playing");
			return 0;
	}
	// stop the sounds
	muswavIcyMusic->stop();
*/
	// free the players data...
	destroyPlayerObjects();

	// free the wave generators data...
	destroyWaveGenObjects();

	return true;// all ok
}// deinitializeGame

