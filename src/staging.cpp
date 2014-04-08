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

#include "staging.h"// for the management of stages, levels, and the switching between these
#include "programinfo.h"// for general info about the program itself
#include "gameglobals.h"// for using the game objects
#include "globals.h"// for using the program objects
#include "generalmaths.h"// for mathematical functions
/*#include "gamesound.h"// for accessing the functions that convert game events to sound executions*/


// getNextLevel **********************
// This function tells what's the next level, since a given
// pair (level, stage)
int getNextLevel(int fromLevel, gameStagesEnum fromStage)
{
	int level=fromLevel;

	// see if it's time to advance a level, or only a stage...
	switch(fromStage) {
	case gmstg_emptyStage:
		// go to the first level!
		level=1;
		break;
	case gmstg_freeHeads:
		level++;
		break;
	default:
		// for any other stage, it's not time to advance the level num
		break;
	}

	// return the result of our calculations...
	return level;
}// getNextLevel

// getNextStage **********************
// This function tells what's the next stage, since a given
// pair (level, stage)
gameStagesEnum getNextStage(int fromLevel, gameStagesEnum fromStage)
{
	gameStagesEnum stage=fromStage;

	// see what the next stage will be
	switch(stage) {
	case gmstg_emptyStage:
		// start playing
		stage=gmstg_freeSheeps;
		break;
	case gmstg_freeSheeps:
		// time to go to the stage of head hunting
		stage=gmstg_freeHeads;
		break;
	case gmstg_freeHeads:
		// time to go to the stage of sheep hunting
		stage=gmstg_freeSheeps;
		break;
	default:
		// for any other stage, we cannot directly change the stage
		break;
	}

	// return the result of our calculations...
	return stage;
}// getNextStage


// activateCurrentStageLevel **********************
// This function configures the current stage and level, so that the
// game starts playing with the current level information
bool activateCurrentStageLevel()
{
	bool logToConsole;
	vectorListNodeStruct<waveGenUnitStruct> *wgUnit;
	stageLevelPropertiesStruct properties;

	// obtain the stageLevel properties
	properties=getStageLevelProperties(
		gameWorld.currentLevel, gameWorld.currentStage);

	// set the time left as given in the properties
	gameWorld.timeLeftToNextStage=properties.maximumTime;// maximum working time for a stage level, 0 for infinitum

	// check the stage information
	switch(gameWorld.currentStage) {
	case gmstg_freeSheeps:
	case gmstg_freeHeads:
		// time to activate the stage of sheep / head hunting
		// activate the sheeps / heads wave generators, and deactivate
		// all the other ones...
		// that is, activate either sheeps or heads, not both
		for (wgUnit=waveGenList.getFirstNode(); wgUnit!=NULL;
			wgUnit=waveGenList.getNextNode(wgUnit)) {

			// don't even mind about this unit if it's not active now
			if (!wgUnit->data.nowActive) continue;

			// check what kind of wave gen it is...
			switch(wgUnit->data.unitType) {
			case FILE_GAMEDATA_WAVEGEN_WHITESHEEPS:
			case FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS:
				// only keep this unit active, if it's the level of
				// freeing sheeps... otherwise deactive it
				wgUnit->data.generatingNow=
					(gameWorld.currentStage==gmstg_freeSheeps);

				// how many seconds between waves?
				wgUnit->data.distanceBetweenWaves=
					MAX(0.15,
					1-(gameWorld.currentLevel-1)*0.2);
				break;
			case FILE_GAMEDATA_WAVEGEN_HUMANHEADS:
				// only keep this unit active, if it's the level of
				// freeing heads... otherwise deactive it
				wgUnit->data.generatingNow=
					(gameWorld.currentStage==gmstg_freeHeads);

				// how many seconds between waves?
				wgUnit->data.distanceBetweenWaves=
					MAX(0.15,
					0.5-(gameWorld.currentLevel-1)*0.1);
				break;
			default:
				// don't ever mind about the other wave generators
				break;
			}
		}

		break;
	default:
		// for any other stage, we cannot activate something we don't
		// know, or a stage that is supposed to be empty! return an error
		// code
		return false;// error!
		break;
	}

	// log the stage activation, to the logs file, and to the console
	// only if in dev mode
#ifdef PROGMODE_DEVELOPER_YES
	logToConsole=true;
#else// PROGMODE_DEVELOPER_NO
	logToConsole=false;
#endif// PROGMODE_DEVELOPER_NO
	logCurrentStageLevelInfo(logToConsole);

	return true;// all ok
}// activateCurrentStageLevel


// setStageLevel **********************
// This function sets some given level and stage to be the current ones
bool setStageLevel(int level, gameStagesEnum stage)
{
	// set these to be the current values
	gameWorld.currentLevel=level;
	gameWorld.currentStage=stage;

	// activate the current stage and level
	if (!activateCurrentStageLevel()) return false;

	return true;// all ok
}// setStageLevel

// logCurrentStageLevelInfo **********************
// A function for logging the current stage level information
bool logCurrentStageLevelInfo(
	bool toConsole)// log to the console as well, or only to the file?
{
	char* strStage;

	// look for the right string for the stage
	switch(gameWorld.currentStage) {
	case gmstg_emptyStage:
#ifdef PROGMODE_DEVELOPER_YES
		strStage="gmstg_emptyStage";
#else// PROGMODE_DEVELOPER_NO
		strStage="Empty Stage";
#endif// PROGMODE_DEVELOPER_NO
		break;
	case gmstg_freeSheeps:
#ifdef PROGMODE_DEVELOPER_YES
		strStage="gmstg_freeSheeps";
#else// PROGMODE_DEVELOPER_NO
		strStage="Free Sheeps Stage";
#endif// PROGMODE_DEVELOPER_NO
		break;
	case gmstg_freeHeads:
#ifdef PROGMODE_DEVELOPER_YES
		strStage="gmstg_freeHeads";
#else// PROGMODE_DEVELOPER_NO
		strStage="Free Heads Stage";
#endif// PROGMODE_DEVELOPER_NO
		break;
	default:
		strStage="unknown-stage";
		break;
	}

	// log it
	if (!logger.logLine("In stage '%s', level: %i, index: %i",
		true, toConsole, true,// log to file, and maybe to console
		strStage,// stage name
		gameWorld.currentLevel,// level number
		getStageLevelIndex(// stage level index
			gameWorld.currentLevel, gameWorld.currentStage)
		)) {

		return false;// error
	}

	return true;// all ok
}// logCurrentStageLevelInfo

// advanceStage **********************
// This function advances the current stage and level, as they should...
bool advanceStage()
{
	int level;
	gameStagesEnum stage;
	int idx;

	// calculate the next stage and level
	level=getNextLevel(gameWorld.currentLevel, gameWorld.currentStage);
	stage=getNextStage(gameWorld.currentLevel, gameWorld.currentStage);

	// calculate the idx
	idx=getStageLevelIndex(level, stage);

/*	// execute the sound of stage advanced, in the case that happens
	if (stage==gmstg_freeHeads) {
		// execute such sound
		makeSoundAdvanceStage();
	}
*/
/*	// execute the next ingame music, if that's the time for that
	switch (idx) {
	case 2:
		executeIngameMusic(2);
		break;
	case 4:
		executeIngameMusic(3);
		break;
	}
*/

	// set the calculated stage and level up
	if (!setStageLevel(level, stage)) return false;

	return true;// all ok
}// advanceStage


// getStageLevelIndex **********************
// This function maps every pair (level, stage) with a number that tells
// the index of such pair in the stageLevels sequence.
// The stageLevel index is always as follows:
// (1, gmstg_freeSheeps)=0
// (1, gmstg_freeHeads)=1
// (2, gmstg_freeSheeps)=2
// (2, gmstg_freeHeads)=3
// (n, gmstg_freeSheeps)=(n-1)*2+0
// (n, gmstg_freeHeads)=(n-1)*2+1
// Other strange levels: some strange value
int getStageLevelIndex(int level, gameStagesEnum stage)
{
	int stageNum;

	// assign a value to stageNum
	switch(stage) {
	case gmstg_freeSheeps:
		stageNum=0;
		break;
	case gmstg_freeHeads:
		stageNum=1;
		break;
	default:
		// extra stages have other numbers
		stageNum=0;
		break;
	}

	// return the calculated stageLevel index
	return stageNum+(level-1)*2;
}// getStageLevelIndex


// getStageLevelProperties **********************
// This function returns a structure filled with the properties of a
// given stage and level.
stageLevelPropertiesStruct getStageLevelProperties(
	int level, gameStagesEnum stage)// level and stage to get the properties of
{
	stageLevelPropertiesStruct properties;
	int stIndex;
	double timeInSec;

	// obtain the stageLevel index for this (level, stage)
	stIndex=getStageLevelIndex(level, stage);

	// fill the properties

	// maximumTime:
	// maximum working time for a stage level, 0 for infinitum
	// Load in timeInSec the amount in seconds
	if (stage==gmstg_freeHeads) {
		// heads
		timeInSec=2+(level-1)*1.2;// 2 to infinitum seconds, 1.2 seconds more every time
		timeInSec=MIN(timeInSec, 6);
	}else{
		// sheeps or some other level
		timeInSec=20;
	}
	properties.maximumTime=int(timeInSec*desiredFramesPerSecond);

	// return the final properties
	return properties;
}// getStageLevelProperties

// getTimeLeftInSeconds **********************
// This is a small function that tells how many seconds are left for the
// switch of stages.
double getTimeLeftInSeconds()
{
	// return it in seconds
	return gameWorld.timeLeftToNextStage/double(desiredFramesPerSecond);
}// getTimeLeftInSeconds

// manageAutomaticStaging **********************
// This function manages the work of the automatic staging, it's to
// be executed exactly once per frame, so it can check whether it's
// time to advance the stage or not, and it can also decrement the
// time counters.
bool manageAutomaticStaging(
	DWORD frameNumber,// current frame number
	bool* didItSwitch)// pointer to a boolean where to put the information about whether the stage was changed or not
{
	// did we advance a level?
	bool tempDidItSwitch=false;

	bool errorCode=true;// all ok

	// check for infinite time left
	if (gameWorld.timeLeftToNextStage!=0) {
		// decrement the counter
		gameWorld.timeLeftToNextStage--;

		// check if it's time to advance
		if (gameWorld.timeLeftToNextStage<=0) {
			// automatic stage advance
			if (!advanceStage())
				errorCode=false;// failed

			// we advanced a level
			tempDidItSwitch=true;
		}
	}

	// set the didItSwitch value
	if (didItSwitch!=NULL) *didItSwitch=tempDidItSwitch;

	// go away
	return errorCode;
}// manageAutomaticStaging
