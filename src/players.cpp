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

#include "players.h"// for managing the players
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "generalmaths.h"// for mathematical functions
#include "genericunits.h"// for managing the generic units...
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include <string.h>
//#include "gamesound.h"// for accessing the functions that convert game events to sound executions

// Functions to be called before and after executing the game

// This function sets all up to let players participate in the game.
// Everything related to player initialization is here.
bool createPlayerObjects()
{
	playerUnitStruct playerData;

	// Add our players to the list

	// Drop the players from the list
	playerList.resetList();

	// Basic info
	setupPlayerUnit(playerData, FILE_GAMEDATA_PLAYER_JAVATAR);
	// Set the relative position stuff (relative to camera)
	playerData.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	playerData.xFromCamera=
		-(constructContainerFrame(&playerData).w-
		constructContainerFrame(&playerData).handlerX);
	playerData.yFromCamera=-50;
	// Coordinates
	playerData.x=playerData.xFromCamera;
	playerData.y=playerData.yFromCamera;
	// Set all up for the entrance
	playerData.goingToPoint=true;
	playerData.goingToPointX=200;
	playerData.goingToPointY=(gameWorld.hCamera-150)/2;
	setSpeedForLinealMove(
		&playerData,// player unit
		playerData.goingToPointX, playerData.goingToPointY,// dest pos
		280/double(desiredFramesPerSecond));// linear speed
	// Don't let it shoot for the first portion of a second
	playerData.unarmedTimeLeft=
		int(0.2*double(desiredFramesPerSecond));
	// Speed
	playerData.linearSpeed=200/double(desiredFramesPerSecond);
	// Input info...
	playerData.userControlled=true;
	playerData.keyUp=SDLK_UP;
	playerData.keyDown=SDLK_DOWN;
	playerData.keyLeft=SDLK_LEFT;
	playerData.keyRight=SDLK_RIGHT;
	playerData.keyShoot=KMOD_RCTRL;
	if (gameWorld.playersToStart<=1) {
		// we are alone, let us use all the keyboard
		playerData.playerUseAllKeyboard=true;// if it's set, we can use special key codes for making many keys do for the same function
	}else{
		playerData.playerUseAllKeyboard=false;// if it's set, we can use special key codes for making many keys do for the same function
	}
	// Add this node
	if (!playerList.isValidIndex(playerList.addNode(playerData))) {
		logger.logLine("Failed adding node to playerList");
		return false;
	}

	return true;// all ok
}// createPlayerObjects

// This functions destroys all what's been created in createPlayerObjects.
bool destroyPlayerObjects()
{
	// nothing to do yet...

	return true;// all ok
}// destroyPlayerObjects

// Other useful functions

// This function increments the score of a player
bool playerIncrementScoreForHit(gameObjectPointerStruct& unitP, double extraScore) {
	vectorListNodeStruct<playerUnitStruct>* player;

	// check for unitP actually being a player, if it's not, it's
	// been an error.
	if (unitP.objectType!=FILE_GAMEDATA_GAMEOBJECT_PLAYER)
		return false;

	player=playerList.getNode(unitP.listIndex);

	// Now check for it being the player it's supposed to be, and for
	// it being a correct player to increment the score
	if (player->data.id!=unitP.id || player->data.energyLeft<=0 ||
		!player->data.nowActive) {

		// I can't increment this player score, neither it's been a
		// real error
		return true;// all ok
	}

	// Increment the score
	player->data.score+=extraScore;
	// save the statistic
	gameWorld.scoreInHits+=(int)extraScore;

	return true;// all ok
}// playerIncrementScoreForHit

// allPlayersIncrementScore *******
// This function increments the score of all the players
bool allPlayersIncrementScore(double extraScore) {

	vectorListNodeStruct<playerUnitStruct> *player;

	for (player=playerList.getFirstNode(); player!=NULL;
	player=playerList.getNextNode(player)) {

		// never mind about this unit if it's not active now
		if (!player->data.nowActive) continue;

		// increment its score
		player->data.score+=extraScore;
	}// for unit

	// all ok
	return true;
}// allPlayersIncrementScore

// This function makes a player unit drop a bullet
bool playerDropBullet(vectorListNodeStruct<playerUnitStruct>& unitP) {
	// Unit types
	genericUnitTypeDataStruct* myPlayerType;
	genericUnitTypeDataStruct* myBulletType;

	// Unit frames
	frameDataStruct* myPlayerFrame;
	frameDataStruct* myBulletFrame;

	// Other variables
	bulletUnitStruct unitB;
	long double x, y;

	// set the default values for this bullet...
	if (!setupBulletUnit(unitB, FILE_GAMEDATA_BULLET_PLAYERFLAME))
		return false;

	// Select the unit types and frames
	myPlayerType=&(playerType[unitP.data.unitType]);
	myPlayerFrame=&(myPlayerType->frames[unitP.data.currentFrame]);
	myBulletType=&(bulletType[unitB.unitType]);
	myBulletFrame=&(myBulletType->frames[unitB.currentFrame]);

	// coordinates for the bullet
	x=unitP.data.x+
		(myPlayerFrame->spawnBulletsX);
	y=unitP.data.y+
		(myPlayerFrame->spawnBulletsY);

	// Center vertically the bullet on those coordinates
	// (x is the x coordinate of the bullet handler, while y is the
	// y coordinate of the center of the bullet)
	unitB.x=x;
	unitB.y=y+(myBulletFrame->handlerY)-(myBulletFrame->h)/2;

	// What kind of units does this bullet hit?: enemies only
	unitB.canCollideWithEnemies=true;// can this bullet unit collide with enemies?
	unitB.canCollideWithPlayers=true;// can this bullet unit collide with players?

	// set the owner
	unitB.owner.id=unitP.data.id;
	unitB.owner.listIndex=playerList.getIndex(&unitP);
	unitB.owner.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;

	// speed for the bullet
	unitB.linearSpeed=
		gameWorld.xSpeedCamera+700/double(desiredFramesPerSecond);

	// set the linear movement
	unitB.goingToPoint=true;
	// medium x collision point
	unitB.goingToPointX=getRightWall(int(unitB.y));
	// top x collision point
	unitB.goingToPointX=MIN(unitB.goingToPointX,
		getRightWall(int(unitB.y-(myBulletFrame->handlerY))));
	// bottom x collision point
	unitB.goingToPointX=MIN(unitB.goingToPointX,
		getRightWall(int(unitB.y+(myBulletFrame->h)
		-(myBulletFrame->handlerY))));
	unitB.goingToPointY=unitB.y;

	// check the bullet won't go backwards, and if it will, don't let
	// it do that: make it appear directly where it must to go
	// (this code assumes some specific horizontal direction for the
	// bullet)
	if (unitB.goingToPointX<unitB.x) unitB.x=unitB.goingToPointX;

	setSpeedForLinealMove(
		&unitB,// player unit
		unitB.goingToPointX, unitB.goingToPointY,// dest pos
		unitB.linearSpeed);// linear speed

	// add the bullet itself
	if (!bulletList.isValidIndex(bulletList.addNode(unitB))) {
		logger.logLine("Failed adding node to bulletList");
		return false;
	}
/*
	// do the sound of the bullet
	makeSoundPlayerDropBullet();
*/
	return true;// all ok
}// playerDropBullet

// Tell whether we take current keyboard state as a pressed state of
// the keyCode, according to the playerUseAllKeyboard flag
bool isPlayerKeyPressed(FILE_DIWRAP_KEYS_TYPE* keys, FILE_DIWRAP_SINGLEKEY_TYPE keyCode, bool playerUseAllKeyboard) {
	if (playerUseAllKeyboard) {
		switch (keyCode) {
		case KMOD_LCTRL:// these three are as an only key
		case KMOD_RCTRL:
		case SDLK_SPACE:
			return KEYDOWN(keys, SDLK_SPACE) ||
				KEYDOWN(keys, KMOD_LCTRL) ||
				KEYDOWN(keys, KMOD_RCTRL);
		default:
			// common behaviour for no special keywords
			return KEYDOWN(keys, keyCode);
		}
	}else{
		return KEYDOWN(keys, keyCode);
	}
}// isPlayerKeyPressed

// This function moves a player unit according to the data from the
// input devices
bool movePlayerUnit(vectorListNodeStruct<playerUnitStruct>& unit, FILE_DIWRAP_KEYS_TYPE* keys)
{
	genericUnitTypeDataStruct* myPlayerType;
	frameDataStruct* myUnitFrame;

	// we use this to know if the unit was going to a given point
	// before executing the unintelligent lineal movement
	bool wasGoingToPoint=unit.data.goingToPoint;

	// select the player type and current frame
	myPlayerType=&(playerType[unit.data.unitType]);
	myUnitFrame=&(myPlayerType->frames[unit.data.currentFrame]);

	// for counting other players
	int counter;
	vectorListNodeStruct<playerUnitStruct> *extraPlayer;

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
		if (!playerList.removeNode(playerList.getIndex(&unit)))
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
			switch(unit.data.unitType) {
			case FILE_GAMEDATA_PLAYER_JAVATAR:
				unit.data.currentFrame=
					getFrameIndexAndListByName("javatarNormal");// normal frame
				unit.data.trembling=false;// don't tremble anymore
				// continue auto framing
				unit.data.vFramesPassed=0;
				// to avoid mistakes, update the frame info
				myUnitFrame=getUnitFrame(&unit.data);
				break;
			}
		}
	}
	// For being unarmed
	if (unit.data.unarmedTimeLeft>0)
		unit.data.unarmedTimeLeft--;

	// Execute some events now...
	// Check events to know if it's time to let the player shoot
	// again (check for the eventAvatar_weaponReadyAgain event)
	// Don't forget, however, that if the shooting is interrupted
	// before going to the frame that has such event, the only
	// factors that will determine the time that the player will
	// continue being unarmed are unarmedTimeMax and unarmedTimeLeft)
	if (unit.data.vFramesPassed==
		myUnitFrame->autoShowVirtualFrames-1 &&
		myUnitFrame->nextEventName!=NULL &&
		strcmp("eventAvatar_weaponReadyAgain",
		myUnitFrame->nextEventName)==0) {

		// set the guy as not being unarmed anymore
		unit.data.unarmedTimeLeft=0;
	}
	// Check events to know if it's time to add a bullet
	// (check for this frame having an "imgAvatarEvent_shoot"
	// event attached to it)
	if (unit.data.vFramesPassed==
		myUnitFrame->autoShowVirtualFrames-1 &&
		myUnitFrame->nextEventName!=NULL &&
		strcmp("eventAvatar_shoot",
		myUnitFrame->nextEventName)==0) {

		// drop the bullet
		if(!playerDropBullet(unit)) return false;
	}

	// should we move the unit to some specific point?
	// that is, unintelligent movement vs. intelligent movement
	if (unit.data.goingToPoint) {
		if (!unintelligentLinealMove(&unit.data))
			return false;// failed!

		// not going to a point anymore? clear the entering flag!
		if (!unit.data.goingToPoint && unit.data.nowEnteringGame) {
			// stopped entering!
			unit.data.nowEnteringGame=false;

			// execute the next ingame music...
/*			executeIngameMusic(1);*/
		}
	}else{
		// now the usual ai
		// Move in every direction
	
		// Up
		if (isPlayerKeyPressed(keys, unit.data.keyUp,
			unit.data.playerUseAllKeyboard)) {

			// go up
			unit.data.y-=unit.data.linearSpeed;

			// if not currently doing anything strange, set the
			// frame of moving down
			if (myUnitFrame->groupName!=NULL &&
				strcmp("javGrpNormal", myUnitFrame->groupName)==0) {

				// set the frame
				unit.data.currentFrame=
					getFrameIndexAndListByName("javatarGoingUp");
				// continue auto framing
				unit.data.vFramesPassed=0;
				// to avoid mistakes, update the frame info
				myUnitFrame=getUnitFrame(&unit.data);
			}
		}
		// Down
		if (isPlayerKeyPressed(keys, unit.data.keyDown,
			unit.data.playerUseAllKeyboard)) {

			// go down
			unit.data.y+=unit.data.linearSpeed;

			// if not currently doing anything strange, set the
			// frame of moving down
			if (myUnitFrame->groupName!=NULL &&
				strcmp("javGrpNormal", myUnitFrame->groupName)==0) {

				// set the frame
				unit.data.currentFrame=
					getFrameIndexAndListByName("javatarGoingDown");
				// continue auto framing
				unit.data.vFramesPassed=0;
				// to avoid mistakes, update the frame info
				myUnitFrame=getUnitFrame(&unit.data);
			}
		}
		// Left
		if (isPlayerKeyPressed(keys, unit.data.keyLeft,
			unit.data.playerUseAllKeyboard))
			unit.data.x-=unit.data.linearSpeed;
		// Right
		if (isPlayerKeyPressed(keys, unit.data.keyRight,
			unit.data.playerUseAllKeyboard))
			unit.data.x+=unit.data.linearSpeed;


		// Check for the collisions against the camera boundaries
		// Up
		if (unit.data.y-(myUnitFrame->handlerY)+(myUnitFrame->cY)
			<(gameWorld.yCamera)) {

			unit.data.y=(myUnitFrame->handlerY)
				-(myUnitFrame->cY)+(gameWorld.yCamera);
		}
		// Left
		if (unit.data.x-(myUnitFrame->handlerX)+(myUnitFrame->cX)
			<(gameWorld.xCamera)) {

			unit.data.x=(myUnitFrame->handlerX)
				-(myUnitFrame->cX)+(gameWorld.xCamera);
		}
		// Down
		if (unit.data.y+(myUnitFrame->cH)+(myUnitFrame->cY)
			-(myUnitFrame->handlerY)>=
			(gameWorld.yCamera)+gameWorld.hCamera) {

			unit.data.y=(gameWorld.yCamera)+gameWorld.hCamera-
			(myUnitFrame->cH)-(myUnitFrame->cY)+(myUnitFrame->handlerY);
		}
		// Right
		if (unit.data.x+(myUnitFrame->cW)+(myUnitFrame->cX)-
			(myUnitFrame->handlerX)>=(gameWorld.xCamera)+
			gameWorld.wCamera) {

			unit.data.x=(gameWorld.xCamera)+gameWorld.wCamera-
			(myUnitFrame->cW)-(myUnitFrame->cX)+(myUnitFrame->handlerX);
		}

		// Check for collisions against the walls
		// Down against the ground
		if (unit.data.y+(myUnitFrame->cH)+(myUnitFrame->cY)
			-(myUnitFrame->handlerY)>=
			(gameWorld.yCamera)+gameWorld.yGroundForPlayer) {

			unit.data.y=(gameWorld.yCamera)+gameWorld.yGroundForPlayer-
			(myUnitFrame->cH)-(myUnitFrame->cY)+(myUnitFrame->handlerY);
		}
		// Left wall against handler
		if ((unit.data.x)-(myUnitFrame->handlerX)+(myUnitFrame->cX)
			<getLeftWall((int)unit.data.y)) {

			unit.data.x=(myUnitFrame->handlerX)
				-(myUnitFrame->cX)+getLeftWall((int)unit.data.y);
		}
		// Right wall against handler
		if (unit.data.x+(myUnitFrame->cW)+(myUnitFrame->cX)-
			(myUnitFrame->handlerX)>=getRightWall((int)unit.data.y)) {

			unit.data.x=getRightWall((int)unit.data.y)-
			(myUnitFrame->cW)-(myUnitFrame->cX)+(myUnitFrame->handlerX);
		}
		// Next four checks aren't necessary at all, but are useful
		// for making the guy keep outside the wall limits with the
		// entire body.
		// Left wall against upper side
		if (unit.data.x-(myUnitFrame->handlerX)+(myUnitFrame->cX)
			<getLeftWall((int)
				unit.data.y-(myUnitFrame->handlerY)+(myUnitFrame->cY))) {

			unit.data.x=(myUnitFrame->handlerX)
				-(myUnitFrame->cX)+getLeftWall((int)
					unit.data.y-(myUnitFrame->handlerY)+(myUnitFrame->cY));
		}
		// Right wall against upper side
		if (unit.data.x+(myUnitFrame->cW)+(myUnitFrame->cX)-
			(myUnitFrame->handlerX)
			>=getRightWall((int)
				unit.data.y-(myUnitFrame->handlerY)+(myUnitFrame->cY))) {

			unit.data.x=getRightWall((int)
				unit.data.y-(myUnitFrame->handlerY)+(myUnitFrame->cY))-
			(myUnitFrame->cW)-(myUnitFrame->cX)+(myUnitFrame->handlerX);
		}
		// Left wall against lower side
		if (unit.data.x-(myUnitFrame->handlerX)+(myUnitFrame->cX)
			<getLeftWall((int)
				unit.data.y-
				(myUnitFrame->handlerY)+(myUnitFrame->cY)
				+(myUnitFrame->cH))) {

			unit.data.x=(myUnitFrame->handlerX)
				-(myUnitFrame->cX)+getLeftWall((int)
					unit.data.y-(myUnitFrame->handlerY)+
					(myUnitFrame->cY)+(myUnitFrame->cH));
		}
		// Right wall against lower side
		if (unit.data.x+(myUnitFrame->cW)+(myUnitFrame->cX)-
			(myUnitFrame->handlerX)
			>=getRightWall((int)
				unit.data.y-(myUnitFrame->handlerY)
				+(myUnitFrame->cY)+(myUnitFrame->cH))) {

			unit.data.x=getRightWall((int)
				unit.data.y-(myUnitFrame->handlerY)
				+(myUnitFrame->cY)+(myUnitFrame->cH))-
			(myUnitFrame->cW)-(myUnitFrame->cX)+(myUnitFrame->handlerX);
		}
	}// lineal movement vs. typical ai

	// Now check if it's shooting
	// (only while alive)
	if ((unit.data.energyLeft>0) &&
		isPlayerKeyPressed(keys, unit.data.keyShoot,
		unit.data.playerUseAllKeyboard) &&
		unit.data.unarmedTimeLeft<=0 &&
		unit.data.untouchableTimeLeft<=0) {

		// Change to the shooting frame if necessary
		if (unit.data.unitType==FILE_GAMEDATA_PLAYER_JAVATAR) {

			// change the frame
			if (unit.data.unitType==FILE_GAMEDATA_PLAYER_JAVATAR)
				unit.data.currentFrame=
				getFrameIndexAndListByName("javatarShoot");// shooting frame

			// continue auto framing
			unit.data.vFramesPassed=0;

			// to avoid mistakes, update the frame info
			myUnitFrame=getUnitFrame(&unit.data);
		}

		// Make it wait for the next shot
		unit.data.unarmedTimeLeft=unit.data.unarmedTimeMax;
	}

	// AI to execute even when in linear movement
	// check for this unit being out of the screen
	// (while dead)
	if ((unit.data.energyLeft<=0) &&
		((unit.data.x-(myUnitFrame->handlerX)
		>=gameWorld.xCamera+gameWorld.wCamera) ||
		(unit.data.y-(myUnitFrame->handlerY)
		>=gameWorld.yCamera+gameWorld.hCamera) ||
		(unit.data.x+(myUnitFrame->w)-(myUnitFrame->handlerX)<0) ||
		(unit.data.y+(myUnitFrame->h)-(myUnitFrame->handlerY)<0))
		) {

		// UNIT TO BE DISABLED: MISCELLANEOUS WORK
		// now do everything else, that has to be done every time
		// a player is disabled
		// Count the active players
		for (extraPlayer=playerList.getFirstNode(), counter=0;
		extraPlayer!=NULL;
		extraPlayer=playerList.getNextNode(extraPlayer)) {
			// don't count inactive units, neither this unit
			// Units with no energy are useful here, though (as we'll
			// deal with these as soon as they finish going out of the
			// screen)
			if (!extraPlayer->data.nowActive ||
				extraPlayer->data.id==unit.data.id)
				continue;
			counter++;
		}
		if (counter<=0) {
			// there are no other alive players, do what's needed
			// ... but nothing is needed
		}

		// Now we finished scoring this unit we can disable it
		// for ever
		unit.data.nowActive=false;
	}

	// update the *FromCamera variables if necessary
	if (unit.data.isPosRelative) {
		unit.data.xFromCamera=unit.data.x-gameWorld.xCamera;
		unit.data.yFromCamera=unit.data.y-gameWorld.yCamera;
	}

	// let's move the unit with the owner unit, if any
	if (!moveWithOwner(&unit.data)) return false;


	return true;// all ok
}// movePlayerUnit

// This function moves the players according to the data from the
// input devices
bool movePlayers(FILE_DIWRAP_KEYS_TYPE* keys)
{
	vectorListNodeStruct<playerUnitStruct> *unit;
	int i=0;

	// move all the players in the game world...
	for (unit=playerList.getFirstNode(); unit!=NULL; unit=playerList.getNextNode(unit)) {
		// don't move this unit if it's not active now
		if (!unit->data.nowActive) continue;

		// move this unit
		if (!movePlayerUnit(*unit, keys)) return false;
	}// for unit

	return true;// all ok
}// movePlayers
