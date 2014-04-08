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

#include "collisions.h"// for functions related to the collisions of units
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "players.h"// for managing the players
#include "generalmaths.h"// for mathematical functions
#include "genericunits.h"// for managing the generic units...
#include "enemyai.h"// for managing the enemy AI...
/*#include "gamesound.h"// for accessing the functions that convert game events to sound executions*/

// enemyUnitHit *******
// This method is called every time an enemy unit is hit by anything.
// Then, it's responsible of making all the necessary changes in the
// touched enemy unit.
bool enemyUnitHit(
	vectorListNodeStruct<enemyUnitStruct>& eUnit,// enemy unit which was hit
	gameObjectPointerStruct& toucher)// the one who touched the enemy unit
{
	vectorListNodeStruct<playerUnitStruct>* player;
	vectorListNodeStruct<bulletUnitStruct>* bullet;

	// if this unit is in untouchable or dead state,
	// nothing can affect it now...
	if (eUnit.data.untouchableTimeLeft>0 || eUnit.data.energyLeft<=0)
		return true;

	// first change the unit to untouchable state.
	eUnit.data.untouchableTimeLeft=eUnit.data.untouchableTimeMax;

	// COLLISION REVISE LATER BEGIN
	// don't let it collide anymore
	eUnit.data.canBeHitByBullets=false;
	eUnit.data.canBeHitByPlayers=false;
	// COLLISION REVISE LATER END

	switch (toucher.objectType) {
	case FILE_GAMEDATA_GAMEOBJECT_PLAYER:
		// it's a player unit
		player=playerList.getNode(toucher.listIndex);

		// SCORING
		// increment the player score
		playerIncrementScoreForHit(toucher, eUnit.data.scoreToGivePerHit);

		// ENERGY
		// Hurt the enemy unit
		if (eUnit.data.decrementableEnergy)
			eUnit.data.energyLeft-=player->data.energyToDrop;
		if (eUnit.data.energyLeft<0) eUnit.data.energyLeft=0;

		break;
	case FILE_GAMEDATA_GAMEOBJECT_BULLET:
		// it's a bullet unit
		bullet=bulletList.getNode(toucher.listIndex);

		// SCORING
		// Increment the toucher player score
		if (bullet->data.owner.objectType==
			FILE_GAMEDATA_GAMEOBJECT_PLAYER) {

			// increment the score of the player
			// who dropped the bullet
			if (!playerIncrementScoreForHit(
				bullet->data.owner,
				eUnit.data.scoreToGivePerHit)) return false;
		}

		// ENERGY
		// Hurt the enemy unit
		if (eUnit.data.decrementableEnergy)
			eUnit.data.energyLeft-=bullet->data.energyToDrop;
		if (eUnit.data.energyLeft<0) eUnit.data.energyLeft=0;

		break;
	}

	// we aren't dead, but have been touched
	if (eUnit.data.untouchableTimeLeft>0) {
		switch(eUnit.data.unitType) {
		case FILE_GAMEDATA_ENEMY_HEADCARRIER:
		case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
			switch (eUnit.data.unitType) {
			case FILE_GAMEDATA_ENEMY_HEADCARRIER:
				eUnit.data.currentFrame=
					getFrameIndexAndListByName("headCarrierTouched");
				break;
			case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
				// frame of touched
				eUnit.data.currentFrame=
					getFrameIndexAndListByName("sheepCarrierTouched");
				break;
			}

			eUnit.data.trembling=true;// and go tremble
			// continue auto framing
			eUnit.data.vFramesPassed=0;

			// FREE SHEEPS (or heads)!
			// and it's time to let sheeps / heads go away
			if (eUnit.data.slaveUnit.objectType!=
				FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

				// carried unit
				vectorListNodeStruct<enemyUnitStruct> *cunit;

				// we only know how to let enemies go away
				if (eUnit.data.slaveUnit.objectType!=
					FILE_GAMEDATA_GAMEOBJECT_ENEMY) return false;

				// obtain the unit and check that's the one we need
				cunit=enemyList.getNode(
					eUnit.data.slaveUnit.listIndex);
				if (cunit==NULL ||
					cunit->data.id!=eUnit.data.slaveUnit.id)
					return false;

				// free that unit
				if (!freeSlavesOf(&eUnit.data)) return false;

				// let the sheep fall
				if (!unitStartFalling(cunit->data)) return false;
			}

			// also, the touched unit now wants to escape fast!
			eUnit.data.goingToPointX=eUnit.data.x-50;
			eUnit.data.goingToPointY=0-
					(constructContainerFrame(&eUnit.data).h
					-constructContainerFrame(&eUnit.data).handlerY);
			eUnit.data.goingToPoint=true;
			if (eUnit.data.isPosRelative) {
				// correct the coordinates to be relative
				eUnit.data.goingToPointX-=gameWorld.xCamera;
				eUnit.data.goingToPointY-=gameWorld.yCamera;
			}
			// set the speed for going to such point
			setSpeedForLinealMove(&eUnit.data,
				eUnit.data.goingToPointX, eUnit.data.goingToPointY,
				eUnit.data.linearSpeed*1.5);

			break;
		}
	}

	return true;// all ok
}// enemyUnitHit

// playerUnitHit ******
// This method is called every time a player unit is hit by anything.
// Then, it's responsible of making all the necessary changes in the
// touched player.
bool playerUnitHit(
	vectorListNodeStruct<playerUnitStruct>& pUnit,// player unit which was hit
	gameObjectPointerStruct& toucher)// the one who touched the player unit
{
	vectorListNodeStruct<enemyUnitStruct>* enemy;
	vectorListNodeStruct<playerUnitStruct>* player2;
	vectorListNodeStruct<bulletUnitStruct>* bullet;

	// for counting other players
	int counter;
	vectorListNodeStruct<playerUnitStruct> *extraPlayer;

	// if this unit is in untouchable or dead state,
	// nothing can affect it now...
	if (pUnit.data.untouchableTimeLeft>0 || pUnit.data.energyLeft<=0)
		return true;

	// first change the unit to untouchable state.
	pUnit.data.untouchableTimeLeft=pUnit.data.untouchableTimeMax;

	// What kind of thing has hit against the player?
	switch(toucher.objectType) {
	case FILE_GAMEDATA_GAMEOBJECT_ENEMY:
		// it's an enemy unit
		enemy=enemyList.getNode(toucher.listIndex);

		// ENERGY
		// Hurt the player
		// calculate the damage
		if (pUnit.data.decrementableEnergy)
			pUnit.data.energyLeft-=enemy->data.energyToDrop;
		if (pUnit.data.energyLeft<0) pUnit.data.energyLeft=0;

		break;// toucher type
	case FILE_GAMEDATA_GAMEOBJECT_PLAYER:
		// it's a player unit
		player2=playerList.getNode(toucher.listIndex);

		// SCORING
		// Increment the toucher player score
		playerIncrementScoreForHit(
			toucher, pUnit.data.scoreToGivePerHit);

		// ENERGY
		// Hurt the player
		// calculate the damage
		if (pUnit.data.decrementableEnergy)
			pUnit.data.energyLeft-=player2->data.energyToDrop;
		if (pUnit.data.energyLeft<0) pUnit.data.energyLeft=0;

		break;// toucher type
	case FILE_GAMEDATA_GAMEOBJECT_BULLET:
		// it's a bullet unit
		bullet=bulletList.getNode(toucher.listIndex);

		// SCORING
		// Increment the toucher player score
		if (bullet->data.owner.objectType==
			FILE_GAMEDATA_GAMEOBJECT_PLAYER) {

			// increment the score of the player
			// who dropped the bullet
			if (!playerIncrementScoreForHit(
				bullet->data.owner,
				pUnit.data.scoreToGivePerHit)) return false;
		}

		// ENERGY
		// Hurt the player
		if (pUnit.data.decrementableEnergy)
			pUnit.data.energyLeft-=bullet->data.energyToDrop;
		if (pUnit.data.energyLeft<0) pUnit.data.energyLeft=0;

		break;// toucher type
	}// switch unit type

	// check for death...
	if (pUnit.data.energyLeft<=0) {
		// DEAD PLAYER
		// we are dead now...

		// set the player information for death
		switch(pUnit.data.unitType) {
		case FILE_GAMEDATA_PLAYER_JAVATAR:
			pUnit.data.currentFrame=
				getFrameIndexAndListByName("javatarDeath");
			// continue auto framing
			pUnit.data.vFramesPassed=0;
			break;
		}

		// dead people isn't untouchable neither unarmed
		pUnit.data.untouchableTimeLeft=0;
		pUnit.data.unarmedTimeLeft=0;
		pUnit.data.trembling=false;

		// dead players are supposed to fall
		pUnit.data.goingToPoint=true;
		pUnit.data.goingToPointX=110;
		pUnit.data.goingToPointY=
			gameWorld.yCamera-
			(20+constructContainerFrame(&pUnit.data).h)+
			constructContainerFrame(&pUnit.data).handlerY;
		if (pUnit.data.isPosRelative) {
			// correct the coordinates to be relative
			pUnit.data.goingToPointX-=gameWorld.xCamera;
			pUnit.data.goingToPointY-=gameWorld.yCamera;
		}
		setSpeedForLinealMove(
			&pUnit.data,// player unit
			pUnit.data.goingToPointX, pUnit.data.goingToPointY,// dest pos
			160/double(desiredFramesPerSecond));// linear speed

		// EXTRA STUFF FOR DEAD PLAYER
		// set some extra stuff, interesting for dead players
		// firstly check it's the only player
		// Count the active players
		for (extraPlayer=playerList.getFirstNode(), counter=0;
		extraPlayer!=NULL;
		extraPlayer=playerList.getNextNode(extraPlayer)) {
			// don't count inactive units, neither this unit, neither
			// those that haven't any more energy (so they are dying
			// right now)
			if (!extraPlayer->data.nowActive ||
				extraPlayer->data.id==pUnit.data.id ||
				extraPlayer->data.energyLeft<=0)
				continue;
			counter++;
		}
		if (counter<=0 && !gameWorld.showGameOverMessage) {
			// there are no other alive players, do what's needed

			// A game over message should be shown
			gameWorld.showGameOverMessage=true;

			// Scream the game over thing
/*			makeSoundGameOver();*/
		}

		// Execute the sound of the player being death...
/*		makeSoundProtagKilled();*/
	}else{
		// PLAYER TOUCHED (not dead)
		// we aren't dead, but have been touched
		if (pUnit.data.untouchableTimeLeft>0) {
			switch(pUnit.data.unitType) {
			case FILE_GAMEDATA_PLAYER_JAVATAR:
				pUnit.data.currentFrame=
					getFrameIndexAndListByName("javatarTouched");
				pUnit.data.trembling=true;// and go tremble
				// continue auto framing
				pUnit.data.vFramesPassed=0;
				break;
			}
		}

/*		// Execute the sound of the player being touched...
		makeSoundProtagTouched();*/
	}

	return true;// all ok
}// playerUnitHit

// bulletUnitHit *******
// This method is called every time a bullet unit is hit by anything.
// Then, it's responsible of making all the necessary changes in the
// touched bullet unit.
bool bulletUnitHit(
	vectorListNodeStruct<bulletUnitStruct>& bUnit,// enemy unit which was hit
	gameObjectPointerStruct& toucher)// the one who touched the enemy unit
{
	bool wasToucherAWall=false;

	if (bUnit.data.nowExploding) {
		// it's already exploding, it hasn't to do anything different
		// once it's touched
		return true;
	}

	switch (toucher.objectType) {
	case FILE_GAMEDATA_GAMEOBJECT_ENEMY:
		// do the sound of the bullet hitting an enemy
/*		makeSoundFlameBulletHitEnemy();*/
		break;
	case FILE_GAMEDATA_GAMEOBJECT_PLAYER:
		// There is nothing to do here
		break;
	case FILE_GAMEDATA_GAMEOBJECT_BULLET:
		// There is nothing to do here
		break;
	case FILE_GAMEDATA_GAMEOBJECT_VIRTUALWALL:
		// do the sound of the bullet hitting a wall
/*		makeSoundFlameBulletHitWall();*/

		// remember it was a wall
		wasToucherAWall=true;
		break;
	}

	// Now it's time for this bullet to start his explosion.
	bUnit.data.currentFrame=
		getFrameIndexAndListByName("flameBulletExploding");
	// continue auto framing
	bUnit.data.vFramesPassed=0;

	if (false) {
		// The exploding bullet can't move but can keep colliding
		bUnit.data.speedX=0;
		bUnit.data.speedY=0;
		bUnit.data.goingToPoint=false;
		// the unit is now marked as 'exploding'
		bUnit.data.nowExploding=true;
	}else{
		// we want the bullet to stop colliding with other units
		bUnit.data.canCollideWithBullets=false;
		bUnit.data.canCollideWithEnemies=false;
		bUnit.data.canCollideWithPlayers=false;

		// the bullet now goes away
		bUnit.data.goingToPointX=bUnit.data.x
			-(wasToucherAWall?200:70);
		bUnit.data.goingToPointY=0-
			(constructContainerFrame(&bUnit.data).h
			-constructContainerFrame(&bUnit.data).handlerY);
		bUnit.data.goingToPoint=true;
		if (bUnit.data.isPosRelative) {
			// correct the coordinates to be relative
			bUnit.data.goingToPointX-=gameWorld.xCamera;
			bUnit.data.goingToPointY-=gameWorld.yCamera;
		}
		// set the speed for going to such point
		setSpeedForLinealMove(&bUnit.data,
			bUnit.data.goingToPointX, bUnit.data.goingToPointY,
			260/double(desiredFramesPerSecond));
	}

	return true;// all ok
}// bulletUnitHit

// checkCollisions *******
// Check the collisions between all the game objects..
bool checkCollisions()
{
	// Nodes lists
	vectorListNodeStruct<enemyUnitStruct>* node;
	vectorListNodeStruct<playerUnitStruct>* nodeP;
	vectorListNodeStruct<playerUnitStruct>* nodeP2;
	vectorListNodeStruct<bulletUnitStruct>* nodeB;
	vectorListNodeStruct<bulletUnitStruct>* nodeB2;
	
	// Unit types
	genericUnitTypeDataStruct* myEnemyType;
	genericUnitTypeDataStruct* myPlayerType;
	genericUnitTypeDataStruct* myPlayerType2;
	genericUnitTypeDataStruct* myBulletType;
	genericUnitTypeDataStruct* myBulletType2;

	// Unit frames
	frameDataStruct* myEnemyFrame;
	frameDataStruct* myPlayerFrame;
	frameDataStruct* myPlayerFrame2;
	frameDataStruct* myBulletFrame;
	frameDataStruct* myBulletFrame2;

	// the rectangles of collision
	int pX, pY, pX2, pY2;// player unit, or rather first unit
	int eX, eY, eX2, eY2;// enemy unit, or rather second unit

	// other stuff
	gameObjectPointerStruct objPointer;


	// COLLISIONS PLAYER vs. ENEMIES and PLAYERS
	// for every player
	for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
		// don't check this unit if it's not active now
		if (!nodeP->data.nowActive) continue;

		// load the player data
		myPlayerType=&(playerType[nodeP->data.unitType]);
		myPlayerFrame=&(myPlayerType->frames[nodeP->data.currentFrame]);
		pX=myPlayerFrame->cX-myPlayerFrame->handlerX+(int)(nodeP->data.x);
		pY=myPlayerFrame->cY-myPlayerFrame->handlerY+(int)(nodeP->data.y);
		pX2=pX+myPlayerFrame->cW-1; pY2=pY+myPlayerFrame->cH-1;

		// COLLISIONS PLAYER vs. ENEMIES
		// for every enemy
		for (node=enemyList.getFirstNode(); node!=NULL; node=enemyList.getNextNode(node)) {
			// don't check this unit if it's not active now
			if (!node->data.nowActive) continue;

			// check for the validity of such collision
			if (!node->data.canBeHitByPlayers) continue;

			// load the enemy data
			myEnemyType=&(enemyType[node->data.unitType]);
			myEnemyFrame=&(myEnemyType->frames[node->data.currentFrame]);
			eX=myEnemyFrame->cX-myEnemyFrame->handlerX+(int)(node->data.x);
			eY=myEnemyFrame->cY-myEnemyFrame->handlerY+(int)(node->data.y);
			eX2=eX+myEnemyFrame->cW-1; eY2=eY+myEnemyFrame->cH-1;

			// check for intersecting rectangles
			if (INTERSECTING_NOTNULL_RECTANGLES
				(pX, pY, pX2, pY2, eX, eY, eX2, eY2)) {

				// Notify both the enemy and the player unit...

				// Enemy
				// Load objPointer with info about the player
				objPointer.id=nodeP->data.id;
				objPointer.listIndex=playerList.getIndex(nodeP);
				objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;
				// Notice the enemy unit
				if (!enemyUnitHit(*node, objPointer))
					return false;

				// Player
				// Load objPointer with info about the enemy
				objPointer.id=node->data.id;
				objPointer.listIndex=enemyList.getIndex(node);
				objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_ENEMY;
				// Notice the player unit
				if (!playerUnitHit(*nodeP, objPointer))
					return false;
			}
		}// for node

		// COLLISIONS PLAYER vs. PLAYERS
		// for every player

		for (nodeP2=playerList.getFirstNode(); nodeP2!=NULL; nodeP2=playerList.getNextNode(nodeP2)) {
			// don't check this unit if it's not active now
			if (!nodeP2->data.nowActive) continue;

			// don't check it either if it's the same that nodeP
			if (nodeP==nodeP2) continue;

			// load the player data
			myPlayerType2=&(playerType[nodeP2->data.unitType]);
			myPlayerFrame2=&(myPlayerType2->frames[nodeP2->data.currentFrame]);
			eX=myPlayerFrame2->cX-myPlayerFrame2->handlerX+(int)(nodeP2->data.x);
			eY=myPlayerFrame2->cY-myPlayerFrame2->handlerY+(int)(nodeP2->data.y);
			eX2=eX+myPlayerFrame2->cW-1; eY2=eY+myPlayerFrame2->cH-1;

			// check for intersecting rectangles
			if (INTERSECTING_NOTNULL_RECTANGLES
				(pX, pY, pX2, pY2, eX, eY, eX2, eY2)) {

				// Notify both player units...

				// Player 1
				// Load objPointer with info about the player 2
				objPointer.id=nodeP2->data.id;
				objPointer.listIndex=playerList.getIndex(nodeP2);
				objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;
				// Notice the first player unit
				if (!playerUnitHit(*nodeP, objPointer))
					return false;

				// Player 2
				// Load objPointer with info about the player 1
				objPointer.id=nodeP->data.id;
				objPointer.listIndex=playerList.getIndex(nodeP);
				objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;
				// Notice the second player unit
				if (!playerUnitHit(*nodeP2, objPointer))
					return false;
			}
		}// for nodeP2
	}// for nodeP

	// COLLISIONS BULLETS vs. ENEMIES, PLAYERS and BULLETS
	// for every bullet
	for (nodeB=bulletList.getFirstNode(); nodeB!=NULL; nodeB=bulletList.getNextNode(nodeB)) {
		// don't check this unit if it's not active now
		if (!nodeB->data.nowActive) continue;

		// load the bullet data
		myBulletType=&(bulletType[nodeB->data.unitType]);
		myBulletFrame=&(myBulletType->frames[nodeB->data.currentFrame]);
		pX=myBulletFrame->cX-myBulletFrame->handlerX+(int)(nodeB->data.x);
		pY=myBulletFrame->cY-myBulletFrame->handlerY+(int)(nodeB->data.y);
		pX2=pX+myBulletFrame->cW-1; pY2=pY+myBulletFrame->cH-1;

		// COLLISIONS BULLET vs. PLAYERS
		if (nodeB->data.canCollideWithPlayers) {
			// for every player
			for (nodeP=playerList.getFirstNode(); nodeP!=NULL; nodeP=playerList.getNextNode(nodeP)) {
				// don't check this unit if it's not active now
				if (!nodeP->data.nowActive) continue;

				// load the player data
				myPlayerType=&(playerType[nodeP->data.unitType]);
				myPlayerFrame=&(myPlayerType->frames[nodeP->data.currentFrame]);
				eX=myPlayerFrame->cX-myPlayerFrame->handlerX+(int)(nodeP->data.x);
				eY=myPlayerFrame->cY-myPlayerFrame->handlerY+(int)(nodeP->data.y);
				eX2=eX+myPlayerFrame->cW-1; eY2=eY+myPlayerFrame->cH-1;

				// check for intersecting rectangles
				if (INTERSECTING_NOTNULL_RECTANGLES
					(pX, pY, pX2, pY2, eX, eY, eX2, eY2)) {

					// Check for collisions with the owner
					if (!nodeB->data.canCollideWithTheOwner &&
						nodeB->data.owner.id==nodeP->data.id)
						continue;

					// Notify both units...

					// Bullet
					// Load objPointer with info about the player
					objPointer.id=nodeP->data.id;
					objPointer.listIndex=playerList.getIndex(nodeP);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;
					// Notice the bullet unit
					if (!bulletUnitHit(*nodeB, objPointer))
						return false;

					// Player
					// Load objPointer with info about the bullet
					objPointer.id=nodeB->data.id;
					objPointer.listIndex=bulletList.getIndex(nodeB);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_BULLET;
					// Notice the player unit
					if (!playerUnitHit(*nodeP, objPointer))
						return false;
				}
			}// for nodeP
		}

		// COLLISIONS BULLET vs. ENEMIES
		if (nodeB->data.canCollideWithEnemies) {
			// for every enemy
			for (node=enemyList.getFirstNode(); node!=NULL; node=enemyList.getNextNode(node)) {
				// don't check this unit if it's not active now
				if (!node->data.nowActive) continue;

				// check for the validity of such collision
				if (!node->data.canBeHitByBullets) continue;

				// load the enemy data
				myEnemyType=&(enemyType[node->data.unitType]);
				myEnemyFrame=&(myEnemyType->frames[node->data.currentFrame]);
				eX=myEnemyFrame->cX-myEnemyFrame->handlerX+(int)(node->data.x);
				eY=myEnemyFrame->cY-myEnemyFrame->handlerY+(int)(node->data.y);
				eX2=eX+myEnemyFrame->cW-1; eY2=eY+myEnemyFrame->cH-1;

				// check for intersecting rectangles
				if (INTERSECTING_NOTNULL_RECTANGLES
					(pX, pY, pX2, pY2, eX, eY, eX2, eY2)) {

					// Check for collisions with the owner
					if (!nodeB->data.canCollideWithTheOwner &&
						nodeB->data.owner.id==node->data.id)
						continue;

					// Notify both units...

					// Bullet
					// Load objPointer with info about the enemy
					objPointer.id=node->data.id;
					objPointer.listIndex=enemyList.getIndex(node);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_ENEMY;
					// Notice the bullet unit
					if (!bulletUnitHit(*nodeB, objPointer))
						return false;

					// Enemy
					// Load objPointer with info about the bullet
					objPointer.id=nodeB->data.id;
					objPointer.listIndex=bulletList.getIndex(nodeB);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_BULLET;
					// Notice the enemy unit
					if (!enemyUnitHit(*node, objPointer))
						return false;
				}
			}// for node
		}

		// COLLISIONS BULLET vs. BULLETS
		if (nodeB->data.canCollideWithBullets) {
			// for every bullet
			for (nodeB2=bulletList.getFirstNode(); nodeB2!=NULL; nodeB2=bulletList.getNextNode(nodeB2)) {
				// don't check this unit if it's not active now
				if (!nodeB2->data.nowActive) continue;

				// don't check it if it's the same that nodeB
				if (nodeB==nodeB2) continue;

				// load the bullet data
				myBulletType2=&(bulletType[nodeB2->data.unitType]);
				myBulletFrame2=&(myBulletType2->frames[nodeB2->data.currentFrame]);
				eX=myBulletFrame2->cX-myBulletFrame2->handlerX+(int)(nodeB2->data.x);
				eY=myBulletFrame2->cY-myBulletFrame2->handlerY+(int)(nodeB2->data.y);
				eX2=eX+myBulletFrame2->cW-1; eY2=eY+myBulletFrame2->cH-1;

				// check for intersecting rectangles
				if (INTERSECTING_NOTNULL_RECTANGLES
					(pX, pY, pX2, pY2, eX, eY, eX2, eY2)) {

					// Notify both units...

					// Bullet 1
					// Load objPointer with info about second bullet
					objPointer.id=nodeB2->data.id;
					objPointer.listIndex=bulletList.getIndex(nodeB2);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_BULLET;
					// Notice the bullet unit
					if (!bulletUnitHit(*nodeB, objPointer))
						return false;

					// Bullet 2
					// Load objPointer with info about the first bullet
					objPointer.id=nodeB->data.id;
					objPointer.listIndex=bulletList.getIndex(nodeB);
					objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_BULLET;
					// Notice the bullet unit
					if (!bulletUnitHit(*nodeB2, objPointer))
						return false;
				}
			}// for nodeB2
		}
	}// for nodeB

	return true;// all ok
}// checkCollisions

