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

#include "enemyai.h"// for managing the enemy AI...
#include "genericunits.h"// for managing the generic units...
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "generalmaths.h"// for mathematical functions
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
/*#include "gamesound.h"// for accessing the functions that convert game events to sound executions*/
#include "sheepai.h"// for managing the AI of a specific type of enemies, the sheeps
#include "humanheadai.h"// for managing the AI of a specific type of enemies, the human heads
#include "carriersai.h"// for managing the AI of a specific type of enemies, the carriers
#include "scriptman.h"// used for accessing to the script manager object
#include "players.h"// for managing the players
#include <stdlib.h>
#include <string.h>

// This function makes an enemy unit start falling
bool unitStartFalling(enemyUnitStruct& enemyData) {
	// frame number, of the frame to be used when the unit is staying
	// at the ground
	int atGroundFrameNum;
	// frame number to be used when falling
	int nextFrameNum;
	// speed for falling (in this unit)
	double fallingSpeed;
	// other temp values
	int minX, maxX;
	int limitX;

	switch(enemyData.unitType) {
	case FILE_GAMEDATA_ENEMY_WHITESHEEP:
		// set some useful extra fields
		enemyData.aiSpecific.sheepData.nowFalling=true;// it's falling!
		fallingSpeed=enemyData.aiSpecific.sheepData.sheepSpeedWhenFalling;// linear speed the unit has when it falls

		atGroundFrameNum=
			getFrameIndexAndListByName("whiteSheepWalking");
		nextFrameNum=
			getFrameIndexAndListByName("whiteSheepDropped");

		// for checking the destine address
		minX=gameWorld.minXFallenWSheeps;
		maxX=gameWorld.maxXFallenWSheeps;
		break;
	case FILE_GAMEDATA_ENEMY_BLACKSHEEP:
		// set some useful extra fields
		enemyData.aiSpecific.sheepData.nowFalling=true;// it's falling!
		fallingSpeed=enemyData.aiSpecific.sheepData.sheepSpeedWhenFalling;// linear speed the unit has when it falls

		atGroundFrameNum=
			getFrameIndexAndListByName("blackSheepWalking");
		nextFrameNum=
			getFrameIndexAndListByName("blackSheepDropped");

		// for checking the destine address
		minX=gameWorld.minXFallenWSheeps;
		maxX=gameWorld.maxXFallenWSheeps;
		break;
	case FILE_GAMEDATA_ENEMY_HUMANHEAD:
		// set some useful extra fields
		enemyData.aiSpecific.headData.fallingHead=true;// it's falling!
		atGroundFrameNum=
			getFrameIndexAndListByName("humanHeadFalling");// common frame
		nextFrameNum=atGroundFrameNum;
		fallingSpeed=enemyData.aiSpecific.headData.headSpeedWhenFalling;// linear speed the unit has when it falls

		// for checking the destine address
		minX=gameWorld.minXFallenHeads;
		maxX=gameWorld.maxXFallenHeads;

		break;
	default:
		// no other kind of unit is allowed to fall this way
		return false;
		break;
	}

	// set the destine address X
	enemyData.goingToPointX=enemyData.x-30;

	// check the destine address X
	limitX=minX+
		getUnitFramesList(&enemyData)[atGroundFrameNum].handlerY;
	if (enemyData.goingToPointX<limitX)
		enemyData.goingToPointX=limitX+(rand()%70);
	limitX=maxX-
		getUnitFramesList(&enemyData)[atGroundFrameNum].cW+
		getUnitFramesList(&enemyData)[atGroundFrameNum].handlerX;
	if (enemyData.goingToPointX>limitX)
		enemyData.goingToPointX=limitX-(rand()%70);

	// set the destine address Y
	enemyData.goingToPointY=
		getGroundWall(int(enemyData.goingToPointX))-
		getUnitFramesList(&enemyData)[atGroundFrameNum].cH+
		getUnitFramesList(&enemyData)[atGroundFrameNum].handlerY;

	// set the stuff all these types have in common
	enemyData.goingToPoint=true;
	if (enemyData.isPosRelative) {
		// correct the coordinates to be relative
		enemyData.goingToPointX-=gameWorld.xCamera;
		enemyData.goingToPointY-=gameWorld.yCamera;
	}

	// set the speed for going to such point
	setSpeedForLinealMove(&enemyData,
		enemyData.goingToPointX, enemyData.goingToPointY,
		fallingSpeed);

	// set the current frame
	enemyData.currentFrame=nextFrameNum;

	return true;// all ok
}// unitStartFalling

// This function does whatever has to be done every time an enemy unit
// is freed (as sheeps and heads are freed, for example)...
// To be freed, is to be taken off, by the anti wave generator
bool enemyUnitFreed(enemyUnitStruct* enemyData) {

	int unitScore;// score to add to the players, from the unit
	int* statisticToUse;// either scoreInFreeWhiteSheeps, scoreInFreeBlackSheeps or scoreInFreeHeads

	// calculate the score to increment
	switch (enemyData->unitType) {
	case FILE_GAMEDATA_ENEMY_BLACKSHEEP:
		// calculate the score for the sheeps, but first check for
		// slave heads...
		if (enemyData->slaveUnit.objectType==
			FILE_GAMEDATA_GAMEOBJECT_ENEMY &&
			getUnitPointer(enemyData->slaveUnit)!=NULL &&
			getUnitPointer(enemyData->slaveUnit)->unitType==
				FILE_GAMEDATA_ENEMY_HUMANHEAD) {

			// it carries a head, act accordingly
			// calculate the score
			unitScore=enemyData->aiSpecific.sheepData.
				sheepScoreToGiveWhenFreedWithAHead;
		}else{
			// it doesn't carry any head, act accoridingly
			// calculate the score
			unitScore=enemyData->aiSpecific.sheepData.
				sheepScoreToGiveWhenFreed;
		}

		// set the statistic to use, to the right one
		statisticToUse=&gameWorld.scoreInFreeBlackSheeps;
		break;
	case FILE_GAMEDATA_ENEMY_WHITESHEEP:
		// calculate the score for the sheeps, but first check for
		// slave heads...
		if (enemyData->slaveUnit.objectType==
			FILE_GAMEDATA_GAMEOBJECT_ENEMY &&
			getUnitPointer(enemyData->slaveUnit)!=NULL &&
			getUnitPointer(enemyData->slaveUnit)->unitType==
				FILE_GAMEDATA_ENEMY_HUMANHEAD) {

			// it carries a head, act accordingly
			// calculate the score
			unitScore=enemyData->aiSpecific.sheepData.
				sheepScoreToGiveWhenFreedWithAHead;
		}else{
			// it doesn't carry any head, act accoridingly
			// calculate the score
			unitScore=enemyData->aiSpecific.sheepData.
				sheepScoreToGiveWhenFreed;
		}

		// set the statistic to use, to the right one
		statisticToUse=&gameWorld.scoreInFreeWhiteSheeps;
		break;
	case FILE_GAMEDATA_ENEMY_HUMANHEAD:
		// calculate the score for human heads
		unitScore=enemyData->aiSpecific.headData.headScoreToGiveWhenFreed;
		// set the statistic to use, to the right one
		statisticToUse=&gameWorld.scoreInFreeHeads;
		break;
	default:
		unitScore=0;
		break;
	}

	// increment such score in all the players
	allPlayersIncrementScore(unitScore);
	// save the statistics...
	(*statisticToUse)+=unitScore;

	return true;// all ok
}// enemyUnitFreed

// checkWaveAntigeneration ********
// A function used to check if a given unit is inside a wave
// antigenerator, and to do some management related to that.
// This function is used for heads and sheeps.
// It's important to note that no unit will be called if it's a slave
// of another one... this means that our units won't be losing
// 'slaves' before their score is well computed
bool checkWaveAntigeneration(
	vectorListNodeStruct<enemyUnitStruct>& unit,
	bool* wasRemoved)
{
	vectorListNodeStruct<waveGenUnitStruct>* wgUnit;

	frameDataStruct combFrame;
	frameDataStruct* wgFrame;

	// we use wasRemoved to inform about the existential status of the
	// given unit after this function finishes its execution.
	if (wasRemoved==NULL) return false;
	*wasRemoved=false;

	// traverse the wave generators list
	for (wgUnit=waveGenList.getFirstNode(); wgUnit!=NULL;
		wgUnit=waveGenList.getNextNode(wgUnit)) {

		// never mind about inactive wave gens
		if (!wgUnit->data.nowActive) continue;

		if (wgUnit->data.unitType==FILE_GAMEDATA_WAVEGEN_SHEEPSOUT) {

			// construct the frame info containing all the slaves
			if (!constructSlaverFullFrame(&unit.data, &combFrame))
				return false;

			// get the frame info of the wave generator
			wgFrame=getUnitFrame(&wgUnit->data);
			if (wgFrame==NULL) return false;

			// Checks for the unit being entirely inside the
			// wave antigenerator
			if (RECTINRECTANGLE(
				unit.data.x-(combFrame.handlerX),
				unit.data.y-(combFrame.handlerY),
				unit.data.x+(combFrame.w)-(combFrame.handlerX)-1,
				unit.data.y+(combFrame.h)-(combFrame.handlerY)-1,
				wgUnit->data.x-(wgFrame->handlerX)+(wgFrame->cX),
				wgUnit->data.y-(wgFrame->handlerY)+(wgFrame->cY),
				wgUnit->data.x-(wgFrame->handlerX)+
					(wgFrame->cX)+(wgFrame->cW)-1,
				wgUnit->data.y-(wgFrame->handlerY)+
					(wgFrame->cY)+(wgFrame->cH)-1)) {

				// you know, it's going to be freed
				// act accordingly...
				if (!enemyUnitFreed(&unit.data)) return false;

				// it's inside, remove the unit...
				// free the slaves of this unit
				if (!freeSlavesOf(&unit.data)) return false;
				// remove the unit
				if (!enemyList.removeNode(enemyList.getIndex(&unit)))
					return false;
/*				// execute the escaped unit sound
				makeSoundFriendlyUnitEscaped();*/
				// the unit has been removed, we cannot continue here
				*wasRemoved=true;
				return true;
			}
		}
	}

	return true;// all ok
}// checkWaveAntigeneration


// executeUnitAI ******
// This method executes the individual AI of a unit.
bool executeUnitAI(
	DWORD frameNumber,// current frame
	vectorListNodeStruct<enemyUnitStruct>& unit)// enemy unit to move
{
	genericUnitTypeDataStruct* myType=&(enemyType[unit.data.unitType]);
	frameDataStruct* myUnitFrame=&(myType->frames[unit.data.currentFrame]);

	// we use this to know if the unit was going to a given point
	// before executing the unintelligent lineal movement
	bool wasGoingToPoint=unit.data.goingToPoint;

	// AUTOFRAMING BEGIN
	// auto frame it
	if (!autoFrameUnit(&(unit.data))) return false;

	// check for negative frame numbers (they use to have a meaning,
	// attached)
	switch(unit.data.currentFrame) {
	// BULLET SPECIFIC CODE BEGIN
	case -1:
		// frame -1 means: this unit should stop existing!
		// free the slaves of this unit
		if (!freeSlavesOf(&unit.data)) return false;
		// remove the unit
		if (!enemyList.removeNode(enemyList.getIndex(&unit)))
			return false;
		// the unit has been removed, we cannot continue here
		return true;
	// BULLET SPECIFIC CODE END
	default:
		// no problem here...
		break;
	}
	// AUTOFRAMING END

	// to avoid mistakes, update the frame info
	myUnitFrame=getUnitFrame(&unit.data);


	// if this unit is a slave of any other, no more work should be
	// done here
	if (unit.data.slaveOf.objectType!=FILE_GAMEDATA_GAMEOBJECT_EMPTY)
		return true;

	// move the unit with the camera if it's necessary
	if (unit.data.isPosRelative) {
		unit.data.x=gameWorld.xCamera+unit.data.xFromCamera;
		unit.data.y=gameWorld.yCamera+unit.data.yFromCamera;
	}

	// before anything, decrease the counters if needed
	// For being untouchable
	if (unit.data.untouchableTimeLeft>0) {
		unit.data.untouchableTimeLeft--;

		if (unit.data.untouchableTimeLeft==0 && unit.data.energyLeft>0) {
			// go back to the usual frame...
			// ENEMIES SPECIFIC CODE BEGIN
			switch(unit.data.unitType) {
			case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
			case FILE_GAMEDATA_ENEMY_HEADCARRIER:
				if (!carrierFinishUntouchabilityEvent(unit))
					return false;
				break;
			}

			// to avoid mistakes, update the frame info
			myUnitFrame=getUnitFrame(&unit.data);
			// ENEMIES SPECIFIC CODE END
		}
	}
	// For being unarmed
	if (unit.data.unarmedTimeLeft>0)
		unit.data.unarmedTimeLeft--;

	// should we move the unit to some specific point?
	// that is, unintelligent movement vs. intelligent movement
	if (unit.data.goingToPoint) {
		if (!unintelligentLinealMove(&unit.data))
			return false;// failed!
	}else{
		// UNIT AI
		// ENEMIES SPECIFIC CODE BEGIN
		// move it as preferred by the unit
		// nothing of interest to be done here
		// ENEMIES SPECIFIC CODE END
	}

	// ENEMIES SPECIFIC CODE BEGIN
	// it ain't easy being a white sheep
/*	// execute the characteristic sheep sound now, once every
	// half a second in average, of being at ground
	if (unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {
		// all the sheeps feel compelled to scream 'meeeeeeee',
		// once every four seconds on average
		if ((rand()%(desiredFramesPerSecond*4))==0) {
			makeSoundHappySheep();
		}
	}
*/	// ENEMIES SPECIFIC CODE END

	// check for switches in the linear movement...
	if (wasGoingToPoint && !unit.data.goingToPoint) {
		// it has switched from lineal movement to other ai... do
		// whatever is necessary to continue

		// ENEMIES SPECIFIC CODE BEGIN
		// switch in stuff carriers
		if (unit.data.unitType==FILE_GAMEDATA_ENEMY_HEADCARRIER ||
			unit.data.unitType==FILE_GAMEDATA_ENEMY_SHEEPCARRIER) {

			// execute the switch event itself
			if (!carrierLinearMovementDoneEvent(unit))
				return false;
		}

		// switch in human heads
		if (unit.data.unitType==FILE_GAMEDATA_ENEMY_HUMANHEAD) {

			// execute the switch event itself
			if (!headLinearMovementDoneEvent(unit))
				return false;
		}

		// switch in sheeps
		if (unit.data.unitType==FILE_GAMEDATA_ENEMY_BLACKSHEEP ||
			unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {

			// execute the switch event itself
			if (!sheepLinearMovementDoneEvent(unit))
				return false;
		}
		// ENEMIES SPECIFIC CODE END
	}else{
		// ENEMIES SPECIFIC CODE BEGIN
		// SHEEP SPECIFIC CODE BEGIN
		// it's doing NOTHING right now... sometimes it's useful
		if (unit.data.unitType==FILE_GAMEDATA_ENEMY_BLACKSHEEP ||
			unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {

			char* event=getUnitFrame(&unit.data)->nextEventName;
			int framesAmount=
				getUnitFrame(&unit.data)->autoShowVirtualFrames;

			// for white sheeps
			if (unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP &&
				event!=NULL &&
				(int(unit.data.vFramesPassed)==framesAmount-1) &&
				strcmp(event, "eventWSheep_startWalking")==0) {

				// make the sheep start walking to its home
				if (!makeSheepWalk(unit)) return false;
			}

			// for black sheeps (bodies)
			if (unit.data.unitType==FILE_GAMEDATA_ENEMY_BLACKSHEEP &&
				event!=NULL &&
				(int(unit.data.vFramesPassed)==framesAmount-1) &&
				strcmp(event, "eventBSheep_startWalking")==0) {

				// make the sheep start walking to its home
				if (!makeSheepWalk(unit)) return false;
			}

		}
		// SHEEP SPECIFIC CODE END
		// ENEMIES SPECIFIC CODE END
	}

	// to avoid mistakes, update the frame info
	myUnitFrame=getUnitFrame(&unit.data);

	// update the *FromCamera variables if necessary
	if (unit.data.isPosRelative) {
		unit.data.xFromCamera=unit.data.x-gameWorld.xCamera;
		unit.data.yFromCamera=unit.data.y-gameWorld.yCamera;
	}

	// let's move the unit with the owner unit, if any
	if (!moveWithOwner(&unit.data)) return false;

	// ENEMIES SPECIFIC CODE BEGIN
	// Time to check for sheeps and heads inside the wave antigenerators
	switch (unit.data.unitType) {
	case FILE_GAMEDATA_ENEMY_BLACKSHEEP:
	case FILE_GAMEDATA_ENEMY_WHITESHEEP:
	case FILE_GAMEDATA_ENEMY_HUMANHEAD:
		bool wasRemoved;

		// do such check for this unit
		if (!checkWaveAntigeneration(unit, &wasRemoved)) return false;

		// if the unit was removed, we cannot continue here
		if (wasRemoved) return true;
		break;
	}
	// ENEMIES SPECIFIC CODE END

	// VISIBILITY CHECK BEGIN
	// Checks for visibility after the unit moved...
	if (isUnitInsideTheScreen(&unit.data)) {

		// It's inside the screen, zero the counter
		unit.data.timeOutsideTheScreenLeft=0;
	}else{
		// The unit is outside the screen.
		if (unit.data.timeOutsideTheScreenLeft<=0) {
			// That's the first time now, set to the max
			unit.data.timeOutsideTheScreenLeft=
				unit.data.timeOutsideTheScreenMax;
		}else{
			// It's not the first time, decrement the counter first
			unit.data.timeOutsideTheScreenLeft--;
		}

		// Now see if it hasn't been too much time out there
		if (unit.data.timeOutsideTheScreenLeft<=0) {
			// It's been too much time out there, remove it.
			// free the slaves of this unit
			if (!freeSlavesOf(&unit.data)) return false;
			// remove the unit
			if (!enemyList.removeNode(enemyList.getIndex(&unit)))
				return false;
			// the unit has been removed, we cannot continue here
			return true;
		}
	}
	// END VISIBILITY CHECK

	return true;// all ok
}// executeUnitAI

// executeEnemyAI *****
// This method executes the individual AI of all the enemies in the
// game world (executed once per game frame).
bool executeEnemyAI(DWORD frameNumber)
{
	vectorListNodeStruct<enemyUnitStruct> *unit;

	// Manage the AI of all the enemies on screen...
	for (unit=enemyList.getFirstNode(); unit!=NULL; unit=enemyList.getNextNode(unit)) {
		// don't move this unit if it's not active now
		if (!unit->data.nowActive) continue;

		// execute the ai of this unit
		if (!executeUnitAI(frameNumber, *unit)) return false;
	}// for unit

	return true;// all ok
}// executeEnemyAI


