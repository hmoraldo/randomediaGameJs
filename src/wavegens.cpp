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

#include "wavegens.h"// for managing the wave generators
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "gamedata.h"// data types of this game
#include "generalmaths.h"// for mathematical functions
#include "genericunits.h"// for managing the generic units...
#include "enemyai.h"// for managing the enemy AI...
/*#include "gamesound.h"// for accessing the functions that convert game events to sound executions*/
#include "scriptman.h"// used for accessing to the script manager object
#include <math.h>
#include <stdlib.h>

// Functions to be called before and after executing the game

// This function sets all the wave generators up, in their
// original positions.
// Everything related to wave gens initialization is here.
bool createWaveGenObjects()
{
	waveGenUnitStruct waveGenData;

	// Add our wave generators to the list
	// The distance between waves is set in the Staging Manager

	// Drop the existent wave gens from the list
	waveGenList.resetList();

	// WaveGen that creates white sheep carriers:
	// Basic info
	setupWaveGenUnit(waveGenData, FILE_GAMEDATA_WAVEGEN_WHITESHEEPS);
	// Generation data
	waveGenData.generatingNow=true;
	// Set the relative position stuff (relative to camera)
	waveGenData.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	waveGenData.xFromCamera=
		scriptMan.getIntVariable("wSheepGenX");
	waveGenData.yFromCamera=
				scriptMan.getIntVariable("wSheepGenY");
	// Coordinates
	waveGenData.x=waveGenData.xFromCamera;
	waveGenData.y=waveGenData.yFromCamera;
	// Add this node
	if (!waveGenList.isValidIndex(waveGenList.addNode(waveGenData))) {
		logger.logLine("Failed adding node to waveGenList");
		return false;
	}

	// WaveGen that creates black sheep carriers:
	// Basic info
	setupWaveGenUnit(waveGenData, FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS);
	// Generation data
	waveGenData.generatingNow=true;
	// Set the relative position stuff (relative to camera)
	waveGenData.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	waveGenData.xFromCamera=
		scriptMan.getIntVariable("bSheepGenX");
	waveGenData.yFromCamera=
		scriptMan.getIntVariable("bSheepGenY");
	// Coordinates
	waveGenData.x=waveGenData.xFromCamera;
	waveGenData.y=waveGenData.yFromCamera;
	// Add this node
	if (!waveGenList.isValidIndex(waveGenList.addNode(waveGenData))) {
		logger.logLine("Failed adding node to waveGenList");
		return false;
	}

	// WaveGen that creates human head carriers:
	// Basic info
	setupWaveGenUnit(waveGenData, FILE_GAMEDATA_WAVEGEN_HUMANHEADS);
	// Generation data
	waveGenData.generatingNow=true;
	// Set the relative position stuff (relative to camera)
	waveGenData.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	waveGenData.xFromCamera=
		scriptMan.getIntVariable("headGenX");
	waveGenData.yFromCamera=
		scriptMan.getIntVariable("headGenY");
	// Coordinates
	waveGenData.x=waveGenData.xFromCamera;
	waveGenData.y=waveGenData.yFromCamera;
	// Add this node
	if (!waveGenList.isValidIndex(waveGenList.addNode(waveGenData))) {
		logger.logLine("Failed adding node to waveGenList");
		return false;
	}

	// WaveGen for sheeps to escape by:
	// Basic info
	setupWaveGenUnit(waveGenData, FILE_GAMEDATA_WAVEGEN_SHEEPSOUT);
	// Generation data
	waveGenData.generatingNow=false;
	// Set the relative position stuff (relative to camera)
	waveGenData.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	waveGenData.xFromCamera=
		scriptMan.getIntVariable("sheepsOutGenX");
	waveGenData.yFromCamera=
		scriptMan.getIntVariable("sheepsOutGenY");
	// Coordinates
	waveGenData.x=waveGenData.xFromCamera;
	waveGenData.y=waveGenData.yFromCamera;
	// Add this node
	if (!waveGenList.isValidIndex(waveGenList.addNode(waveGenData))) {
		logger.logLine("Failed adding node to waveGenList");
		return false;
	}

	return true;// all ok
}// createWaveGenObjects

// This functions destroys all what's been created in
// createWaveGenObjects.
bool destroyWaveGenObjects()
{
	// nothing to do yet...

	return true;// all ok
}// destroyWaveGenObjects

// addEnemiesWaveGen ******
// This method makes a given wave generator add units to the game world
bool addEnemiesWaveGen(
	DWORD frameNumber,// current frame
	vectorListNodeStruct<waveGenUnitStruct>& unit)// wavegen unit to move
{
	int i;
	frameDataStruct* enemyFrame;
	frameDataStruct* wgFrame;
	frameDataStruct combinedFrame;

	// value to multiply the speed of units (for progressive difficulty)
	double speedMultiplier;

	// add an enemy wave
	enemyUnitStruct enemyData;
	enemyUnitStruct carriedUnit;

	// pointer to the unit once it's been created
	genericUnitStruct* eUnitPtr;

	// tells whether carriedUnit should be added to the enemy list as
	// well, and set as the slave unit of enemyData
	bool shouldISlavize;

	// tells how many units to generate...
	int unitsToGenerate;

	// node index numbers
	DWORD enemyIdx, carriedIdx;

	// get the current frame
	wgFrame=
		&(waveGenType[unit.data.unitType].frames
		[unit.data.currentFrame]);

	// calculate speed to multiply now
	speedMultiplier=1+(gameWorld.currentLevel-1)*0.20;

	// calculate unitsToGenerate first
	switch (unit.data.unitType) {
	case FILE_GAMEDATA_WAVEGEN_WHITESHEEPS:
		unitsToGenerate=1;
		break;
	case FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS:
		unitsToGenerate=1;
		break;
	case FILE_GAMEDATA_WAVEGEN_HUMANHEADS:
		unitsToGenerate=rand()%4+1;
		break;
	default:
		// never mind...
		unitsToGenerate=0;
		break;
	}

/*	// execute a sound if some unit will be generated
	if (unitsToGenerate>0) {
		makeSoundGeneratedUnit();
	}
*/
	// execute the addition of units now
	for (i=0; i<unitsToGenerate; i++) {
		// initialize shouldISlavize to false, most of the units
		// won't have any slaves...
		shouldISlavize=false;

		// calculate the creation of units, for every wave generator
		// UNITS CREATION, FIRST PART BEGIN
		// The following code is devoted to the creation of units, and
		// their respective slaves. Coordinates of the spawned units
		// aren't still known.
		switch (unit.data.unitType) {
		case FILE_GAMEDATA_WAVEGEN_WHITESHEEPS:
			// create the units...
			// random says: create a sheep carrier for white sheeps
			setupEnemyUnit(enemyData,
				FILE_GAMEDATA_ENEMY_SHEEPCARRIER);

			// set this unit as just going out, so untouchable
			// until the next ai execution time
			// (that is, we don't want this unit to collide with
			// anything until it's somewhat away from the wave
			// generator)
			enemyData.justGoingOut=true;
			enemyData.canBeHitByBullets=false;
			enemyData.canBeHitByPlayers=false;

			// set some other things
			if (rand()%4<=2) {// probabilities: 3 vs 1
				// the unit will have a slave
				// set the current frame to be the one of carrying
				enemyData.currentFrame=
					getFrameIndexAndListByName(
					"sheepCarrierCarryingSheep");

				// set it to be carrying a sheep
				setupEnemyUnit(carriedUnit,
					FILE_GAMEDATA_ENEMY_WHITESHEEP);
				shouldISlavize=true;// we'll need a slave unit
				// typical speed for sheeps
				carriedUnit.linearSpeed=
					66/double(desiredFramesPerSecond);
				// typical speed for falling in sheeps
				carriedUnit.aiSpecific.sheepData.sheepSpeedWhenFalling=
					carriedUnit.linearSpeed*2;
				// set the being carried frame for this sheep
				carriedUnit.currentFrame=
					getFrameIndexAndListByNameInSheeps(
						unit.data.unitType==
							FILE_GAMEDATA_ENEMY_WHITESHEEP,
						"SheepCarried");
			}else{
				// the unit won't have slaves
				// let the current frame to be the initial one (when
				// it isn't carrying anything)
			}

			// get the current frame
			enemyFrame=
				&(enemyType[enemyData.unitType].frames
				[enemyData.currentFrame]);

			// typical speed
			enemyData.linearSpeed=120/double(desiredFramesPerSecond);
			// set it according to the current level difficulty
			enemyData.linearSpeed*=speedMultiplier;

			break;
		case FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS:
			// random says: create a sheep carrier for black sheeps
			setupEnemyUnit(enemyData,
				FILE_GAMEDATA_ENEMY_SHEEPCARRIER);

			// set this unit as just going out, so untouchable
			// until the next ai execution time
			// (that is, we don't want this unit to collide with
			// anything until it's somewhat away from the wave
			// generator)
			enemyData.justGoingOut=true;
			enemyData.canBeHitByBullets=false;
			enemyData.canBeHitByPlayers=false;

			// set some other things
			if (rand()%4<=2) {// probabilities: 3 vs 1
				// the unit will have a slave
				// set the current frame to be the one of carrying
				enemyData.currentFrame=
					getFrameIndexAndListByName(
					"sheepCarrierCarryingBody");

				// set it to be carrying a sheep
				setupEnemyUnit(carriedUnit,
					FILE_GAMEDATA_ENEMY_BLACKSHEEP);
				shouldISlavize=true;// we'll need a slave unit
				// typical speed for sheeps
				carriedUnit.linearSpeed=
					66/double(desiredFramesPerSecond);
				// typical speed for falling in sheeps
				carriedUnit.aiSpecific.sheepData.sheepSpeedWhenFalling=
					carriedUnit.linearSpeed*2;
				// set the being carried frame for this sheep
				carriedUnit.currentFrame=
					getFrameIndexAndListByNameInSheeps(
						unit.data.unitType==
							FILE_GAMEDATA_ENEMY_WHITESHEEP,
						"SheepCarried");
			}else{
				// the unit won't have slaves
				// let the current frame to be the initial one (when
				// it isn't carrying anything)
			}

			// get the current frame
			enemyFrame=
				&(enemyType[enemyData.unitType].frames
				[enemyData.currentFrame]);

			// typical speed
			enemyData.linearSpeed=120/double(desiredFramesPerSecond);
			// set it according to the current level difficulty
			enemyData.linearSpeed*=speedMultiplier;

			break;
		case FILE_GAMEDATA_WAVEGEN_HUMANHEADS:
			// create the units...
			// random says: create a head carrier
			setupEnemyUnit(enemyData,
				FILE_GAMEDATA_ENEMY_HEADCARRIER);

			// set this unit as just going out, so untouchable
			// until the next ai execution time
			// (that is, we don't want this unit to collide with
			// anything until it's somewhat away from the wave
			// generator)
			enemyData.justGoingOut=true;
			enemyData.canBeHitByBullets=false;
			enemyData.canBeHitByPlayers=false;

			// set some other things
			if (rand()%4<=2) {// probabilities: 3 vs 1
				// the unit will have a slave
				// set the current frame to be the one of carrying
				enemyData.currentFrame=
					getFrameIndexAndListByName("headCarrierCarrying");

				// set it to be carrying a sheep
				setupEnemyUnit(carriedUnit,
					FILE_GAMEDATA_ENEMY_HUMANHEAD);
				shouldISlavize=true;// we'll need a slave unit
				// typical speed for heads
				carriedUnit.linearSpeed=
					100/double(desiredFramesPerSecond);
				// typical speed for falling in heads
				carriedUnit.aiSpecific.headData.headSpeedWhenFalling=
					carriedUnit.linearSpeed*2;
			}else{
				// the unit won't have slaves
				// let the current frame to be the initial one (when
				// it isn't carrying anything)
			}

			// get the current frame
			enemyFrame=
				&(enemyType[enemyData.unitType].frames
				[enemyData.currentFrame]);

			// typical speed
			enemyData.linearSpeed=120/double(desiredFramesPerSecond);
			// set it according to the current level difficulty
			enemyData.linearSpeed*=speedMultiplier;

			break;
		case FILE_GAMEDATA_WAVEGEN_SHEEPSOUT:
			// sheeps don't generate, this should not be executed
			return false;
			break;
		default:
			return false;// that type of wave gen isn't known to us
			break;
		}
		// UNITS CREATION, FIRST PART END

		// GENERAL GENERATION CODE FIRST PART BEGIN
		// The following code adds the created units to their respective
		// lists, and calculates the real coordinates of those.
		// shouldISlavize 1 of 2
		if (shouldISlavize) {
			// slaver
			enemyData.slaveUnit.id=carriedUnit.id;
			enemyData.slaveUnit.objectType=carriedUnit.objectType;
			// slave
			carriedUnit.slaveOf.id=enemyData.id;
			carriedUnit.slaveOf.objectType=enemyData.objectType;
			carriedUnit.nowDrawable=false;// to be drawn only together to its owner
			// set carriedUnit as the slave unit of enemyData
			// (everything, but list indexes)
			carriedIdx=enemyList.addNode(carriedUnit);
			if (!enemyList.isValidIndex(carriedIdx)) {
				logger.logLine("Failed adding node to enemyList");
				return false;
			}
		}

		enemyIdx=enemyList.addNode(enemyData);
		if (!enemyList.isValidIndex(enemyIdx)) {
			logger.logLine("Failed adding node to enemyList");
			return false;
		}

		// obtain a pointer to the added unit
		eUnitPtr=&(enemyList.getNode(enemyIdx)->data);

		// shouldISlavize 2 of 2
		if (shouldISlavize) {
			// (only list indexes now)
			eUnitPtr->slaveUnit.listIndex=
				carriedIdx;
			// slave...
			enemyList.getNode(carriedIdx)->data.slaveOf.listIndex=
				enemyIdx;
		}

		// create a frame information with the information of the unit
		// together with its slaves (if any)
		//if (!constructSlaverFullFrame(eUnitPtr, &combinedFrame))
		//	return false;
		// that's history: now we do it directly with the current
		// frame of that unit, usually the first one in the sequence,
		// a special marker of size
		combinedFrame=*(getUnitFrame(eUnitPtr));

		// Set coordinates
		// X: just at the first pixel of the spawning rectangle
		// wgFrame->cX: the spawning rectangle x
		// +unit.data.x-(wgFrame->handlerX): plus the direction of
		// the first pixel of the wave generator
		// +enemyFrame->handlerX: plus the enemy frame handler x
		eUnitPtr->x=
			wgFrame->cX+unit.data.x-(wgFrame->handlerX)
			+combinedFrame.handlerX;
		// Y: just at the first pixel of the spawning rectangle
		// wgFrame->cY: the spawning rectangle y
		// +unit.data.y-(wgFrame->handlerY): plus the direction of
		// the first pixel of the wave generator
		// +enemyFrame->handlerY: plus the enemy frame handler y
		// +(rand()%(wgFrame->cW-enemyFrame->cW))
		//  : the random y position
		eUnitPtr->y=
			wgFrame->cY+unit.data.y-(wgFrame->handlerY)
			+combinedFrame.handlerY+
			// now we have a division by zero check here too
			((wgFrame->cH-combinedFrame.h)==0?0:(rand()%(wgFrame->cH-combinedFrame.h)));
		// GENERAL GENERATION CODE FIRST PART END

		// FOLLOWERS_TEST_BEGIN
		// no followers for now for a while...
		if (false &&
			(eUnitPtr->unitType==FILE_GAMEDATA_ENEMY_HEADCARRIER ||
			eUnitPtr->unitType==FILE_GAMEDATA_ENEMY_SHEEPCARRIER)
			&& rand()%1==0) {
			vectorListNodeStruct<enemyUnitStruct> *unit;
			enemyUnitStruct *eUnit=(enemyUnitStruct*)eUnitPtr;
			int someNum;
			int i, j;
			int distance;

			// look for the unit that is in that position of the list
			unit=enemyList.getFirstNode();
			if (unit!=NULL && unit->data.nowActive) {
				unit=NULL;// this way the first unit isn't ignored in the first pass

				distance=500;
				for (j=0;
				j<30 && (unit==NULL ||
				((unit->data.unitType!=FILE_GAMEDATA_ENEMY_HEADCARRIER
				&& unit->data.unitType!=FILE_GAMEDATA_ENEMY_SHEEPCARRIER) ||
				distance>=200));
				j++) {
					someNum=1+(rand()%100);

					for (i=0; i<someNum; i++) {
						if (unit!=NULL)
							unit=enemyList.getNextNode(unit);

						if (unit==NULL)
							unit=enemyList.getFirstNode();
					}

					distance=(int)
						(sqrt(SQUARE(unit->data.x-eUnit->x)+
						SQUARE(unit->data.y-eUnit->y)));
				}

				if (distance<200 &&
					(unit->data.unitType==
					FILE_GAMEDATA_ENEMY_HEADCARRIER ||
					unit->data.unitType==
					FILE_GAMEDATA_ENEMY_SHEEPCARRIER)) {

					// assign that one
					eUnit->aiSpecific.flockerData.unitToFollow.objectType=
						unit->data.objectType;
					eUnit->aiSpecific.flockerData.unitToFollow.id=
						unit->data.id;
					eUnit->aiSpecific.flockerData.unitToFollow.listIndex=
						enemyList.getIndex(unit);
				}
			}
		}
		// FOLLOWERS_TEST_END

		// UNITS CREATION, SECOND PART BEGIN
		// The following code is to allow the setting of properties
		// in the units, once these have been created, and once the
		// real position (coordinates) of these is known.
		switch (unit.data.unitType) {
		case FILE_GAMEDATA_WAVEGEN_WHITESHEEPS:
			// set the going to fields
			eUnitPtr->goingToPoint=true;
			eUnitPtr->goingToPointX=eUnitPtr->x-100;
			eUnitPtr->goingToPointY=eUnitPtr->y;
			break;
		case FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS:
			// set the going to fields
			eUnitPtr->goingToPoint=true;
			eUnitPtr->goingToPointX=eUnitPtr->x-100;
			eUnitPtr->goingToPointY=eUnitPtr->y;//+(rand()%60)-30;
			break;
		case FILE_GAMEDATA_WAVEGEN_HUMANHEADS:
			// set the going to fields
			eUnitPtr->goingToPoint=true;
			eUnitPtr->goingToPointX=eUnitPtr->x-100;
			eUnitPtr->goingToPointY=eUnitPtr->y;//+(rand()%60)-30;
			break;
		case FILE_GAMEDATA_WAVEGEN_SHEEPSOUT:
			// sheeps don't generate, this should not be executed
			return false;
			break;
		default:
			return false;// that type of wave gen isn't known to us
			break;
		}
		// UNITS CREATION, SECOND PART END

		// GENERAL GENERATION CODE SECOND PART BEGIN
		// The following generic code finishes the creation of the units
		// the code below worked with, by setting the latest data of
		// these.
		if (eUnitPtr->isPosRelative) {
			// correct the coordinates to be relative
			eUnitPtr->goingToPointX-=gameWorld.xCamera;
			eUnitPtr->goingToPointY-=gameWorld.yCamera;
		}

		// now do whatever is needed after having calculated
		// the initial coordinates
		if (eUnitPtr->goingToPoint) {

			// set the speed for going to such point
			setSpeedForLinealMove(eUnitPtr,
				eUnitPtr->goingToPointX, eUnitPtr->goingToPointY,
				eUnitPtr->linearSpeed);
		}
		// GENERAL GENERATION CODE SECOND PART END
	}

	return true;// all ok
}// addEnemiesWaveGen

// executeWaveGenAI ******
// This method executes the individual AI of a wave generator unit.
bool executeWaveGenAI(
	DWORD frameNumber,// current frame
	vectorListNodeStruct<waveGenUnitStruct>& unit)// wavegen unit to move
{
	if (!unit.data.generatingNow) return true;// nothing to do now...

	// This function works by putting a virtual wave at some distance
	// from the last enemy wave added, and then moving it as if it were
	// an object. When the virtual enemy wave object is detected to be
	// just inside the screen, a bunch of enemies is added and a new
	// virtual wave object is put.

	// Start moving the virtual wave object
	unit.data.nextEnemyWaveX-=unit.data.virtualEnemyWavesSpeedX;

	// see if it's time for adding an enemy wave
	if (unit.data.nextEnemyWaveX<=gameWorld.xCamera+gameWorld.wCamera) {
		// Add the wave
		if (!addEnemiesWaveGen(frameNumber, unit)) return false;

		// Setup next virtual wave
		unit.data.nextEnemyWaveX=gameWorld.xCamera+gameWorld.wCamera+
			unit.data.distanceBetweenWaves;
	}

	return true;// all ok
}// executeUnitAI

// A function to execute all the enemy waves (or rather those that are
// active)
bool executeEnemyWaves(DWORD frameNumber)
{
	vectorListNodeStruct<waveGenUnitStruct> *unit;

	// Manage the AI of all the wave generators on screen...
	for (unit=waveGenList.getFirstNode(); unit!=NULL; unit=waveGenList.getNextNode(unit)) {
		// don't move this unit if it's not active now
		if (!unit->data.nowActive) continue;

		// execute the ai of this unit
		if (!executeWaveGenAI(frameNumber, *unit)) return false;
	}// for unit

	return true;// all ok
}// executeEnemyWaves
