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

#include "carriersai.h"// for managing the AI of a specific type of enemies, the carriers
#include "enemyai.h"// for managing the enemy AI...
#include "genericunits.h"// for managing the generic units...
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "generalmaths.h"// for mathematical functions
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include <math.h>
#include <stdlib.h>

// carrierLinearMovementDoneEvent *******
// This is the code to be executed when a carrier finishes with the
// execution of a linear movement.
bool carrierLinearMovementDoneEvent(
	vectorListNodeStruct<enemyUnitStruct>& unit)
{
	// Pseudo Flocking variables
	// these are used for calculating the positions to use
	const int desiredNum=15;
	int desiredX[desiredNum], desiredY[desiredNum];

	double minDistanceEnemy[desiredNum];
	double minDistanceBullet[desiredNum];

	int maxDistanceDesiredNum;
	double tempMinDistance;

	int i;

	vectorListNodeStruct<enemyUnitStruct>* otherU;
	vectorListNodeStruct<bulletUnitStruct>* otherB;
	vectorListNodeStruct<playerUnitStruct>* onePlayer;

	double otherUX, otherUY;
	register double distance;

	// first of all, check whether we should enable the collision
	// of this object with other ones or not
	if (unit.data.justGoingOut) {
		// now it can collide with both bullets and players
		unit.data.justGoingOut=false;// you are not new anymore
		unit.data.canBeHitByBullets=true;
		unit.data.canBeHitByPlayers=true;
	}

	// calculate some desired coordinates
	for (i=0; i<desiredNum; i++) {
		int l;

		// for i, calculate these coordinates
		if (unit.data.x<160) {
			// [0,160)
			// go high, escape from the screen!
			desiredX[i]=0-
				(constructContainerFrame(&unit.data).w
				-constructContainerFrame(&unit.data).handlerX);
			desiredY[i]=(rand()%300)-250;
		}else{
			// [160, screenWidth)
			int minY, maxY;

			// calculate the limits for our guy
			if (unit.data.x<270) {
				// check if it has any slave units
				if (unit.data.slaveUnit.objectType!=
					FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

					// go very high! don't let the slave unit
					// touch the anti wave generator
					minY=38;
					maxY=350;
				}else{
					// don't care too much... go slightly higher
					minY=38;
					maxY=400;
				}
			}else{
				// go anywhere
				minY=38;
				maxY=462;
			}

			// go somewhere in the middle
			desiredX[i]=int(unit.data.x)-10-(rand()%60);
			desiredY[i]=int(unit.data.y)-(rand()%200)+100;

			// we try not to choose values over the limits, for Y
			for (l=0; l<20 &&
				(desiredY[i]<minY || desiredY[i]>maxY); l++) {

				desiredY[i]=int(unit.data.y)-(rand()%200)+100;
			}
			// over the limits: we have no choice now
			desiredY[i]=desiredY[i]<minY?minY:desiredY[i];
			desiredY[i]=desiredY[i]>maxY?maxY:desiredY[i];
		}

		// changing the resolution of the Y coordinate

		// don't let the distance to travel get too high
		// Rather, set desiredX and desiredY so that they
		// always make the unit travel some fixed distance.
		distance=
			sqrt(SQUARE(unit.data.x-desiredX[i])+
			SQUARE(unit.data.y-desiredY[i]));
		if (distance!=0) {
			desiredX[i]=int(unit.data.x+
				((desiredX[i]-unit.data.x)/(distance/60.0)));
			desiredY[i]=int(unit.data.y+
				((desiredY[i]-unit.data.y)/(distance/60.0)));
		}

		// now check the minimum distance between the choosen
		// point and the goingTo point of some other unit
		minDistanceBullet[i]=SQUARE(gameWorld.wCamera)+
			SQUARE(gameWorld.hCamera);
		for (otherU=enemyList.getFirstNode(); otherU!=NULL;
		otherU=enemyList.getNextNode(otherU)) {

			// never mind about this unit if it's not active, or
			// if it's the same as the one being moved now
			if (!otherU->data.nowActive ||
				otherU->data.id==unit.data.id) continue;

			// never mind about the carried unit either
			if (otherU->data.id==unit.data.slaveUnit.id)
				continue;

			// calculate the pseudo distance
			// AI REVISE LATER BEGIN: here I remove goingtopoint
			if (false && otherU->data.goingToPoint) {
				otherUX=otherU->data.goingToPointX;
				otherUY=otherU->data.goingToPointY;
				if (otherU->data.isPosRelative) {
					otherUX-=gameWorld.xCamera;
					otherUY-=gameWorld.xCamera;
				}
			}else{
				otherUX=otherU->data.x;
				otherUY=otherU->data.y;
			}
			// AI REVISE LATER END
			tempMinDistance=
				SQUARE(desiredX[i]-otherUX)
				+SQUARE(desiredY[i]-otherUY);

			minDistanceEnemy[i]=
				MIN(tempMinDistance, minDistanceEnemy[i]);
		}// for otherU


		// now check the minimum distance between the choosen
		// point and the place some near bullet is
		minDistanceBullet[i]=SQUARE(gameWorld.wCamera)+
			SQUARE(gameWorld.hCamera);
		for (otherB=bulletList.getFirstNode(); otherB!=NULL;
		otherB=bulletList.getNextNode(otherB)) {

			// never mind about this bullet if it's not active, or
			// if it's the same as the one being moved now
			if (!otherB->data.nowActive) continue;

			// get the coordinates...
			otherUX=otherB->data.x;
			otherUY=otherB->data.y;

			// AI REVISE LATER BEGIN
			// never mind about this bullet if it's farther than 160
			// pixels
			if (SQUARE(unit.data.x-otherUX)
				+SQUARE(unit.data.y-otherUY)>(160*160))
				continue;
			// AI REVISE LATER END

			// calculate the pseudo distance
			tempMinDistance=
				SQUARE(desiredX[i]-otherUX)
				+SQUARE(desiredY[i]-otherUY);

			minDistanceBullet[i]=
				MIN(tempMinDistance, minDistanceBullet[i]);
		}// for otherB

	}

	// now choose, between those, the one that is less
	// full of enemies and bullets
	maxDistanceDesiredNum=0;
	for (i=0; i<desiredNum; i++) {
		bool r;// for the inlined function

		// INLINE_FUNCTION_1 BEGIN
		// this is like a function we call, and it returns a bool in a
		// variable named r
		// bool isThisPositionBetter(int thisOne, int theOtherOne) {
		//	int i=thisOne;
		//	int maxDistanceDesiredNum=theOtherOne;
			double value1, value2;

			value1=
				minDistanceEnemy[i]*0.8+
				minDistanceBullet[i]*0.2;
			value2=
				minDistanceEnemy[maxDistanceDesiredNum]*0.8+
				minDistanceBullet[maxDistanceDesiredNum]*0.2;

			r=value1>value2;

		//	return r;
		// }// isThisPositionBetter
		// INLINE_FUNCTION_1 END

		// if (isThisPositionBetter(i, maxDistanceDesiredNum)
		if (r)
			maxDistanceDesiredNum=i;
	}

	// calculate the final data
	unit.data.goingToPointX=desiredX[maxDistanceDesiredNum];
	unit.data.goingToPointY=desiredY[maxDistanceDesiredNum];

	// sometimes, go to the player pos (direct attack!), but
	// only if the attacking unit has a slave
	// (but let's get it deactivated for a while)
	{// block begin
	int x, y;
	onePlayer=playerList.getFirstNode();
	x=int(onePlayer->data.x); y=int(onePlayer->data.y);
	distance=
		SQUARE(x-unit.data.x)
		+SQUARE(y-unit.data.y);
	if (rand()%10==0 &&	distance<SQUARE(200)
		&& (
		ABS(x-getLeftWall(y))<100 ||
		ABS(x-getRightWall(y))<100 ||
		ABS(y-getGroundWall(x))<100)
		) {

		// too near to the borders
		// attack the player directly!
		unit.data.goingToPointX=onePlayer->data.x;
		unit.data.goingToPointY=onePlayer->data.y;
	}
	}// end block

	// FOLLOWERS_TEST_BEGIN
	// check if it's a follower
	{
	enemyUnitStruct* followMe=(enemyUnitStruct*)getUnitPointer(
		unit.data.aiSpecific.flockerData.unitToFollow);

	if (followMe!=NULL) {
		double goX, goY;
		double tmpGoX=unit.data.goingToPointX;
		double tmpGoY=unit.data.goingToPointY;

		if (followMe->goingToPoint) {
			goX=followMe->goingToPointX;
			goY=followMe->goingToPointY;
		}else{
			goX=followMe->x;
			goY=followMe->y;
		}

		// if we aren't too near, set us to go where our followed wants
		// to be
		// OJO!!! a veces los going to son distintos, relativos o no
		distance=
			sqrt(SQUARE(unit.data.x-goX)+
			SQUARE(unit.data.y-goY));
		if (distance>125) {
			// now let's go where our friend is going to...
			unit.data.goingToPoint=true;
			unit.data.goingToPointX=goX;
			unit.data.goingToPointY=goY;

			// don't let the distance to travel get too high
			// Rather, set desiredX and desiredY so that they
			// always make the unit travel some fixed distance.
			distance=
				sqrt(SQUARE(unit.data.x-unit.data.goingToPointY)+
				SQUARE(unit.data.y-unit.data.goingToPointY));
			if (distance!=0) {
				unit.data.goingToPointX=int(unit.data.x+
					((unit.data.goingToPointX-unit.data.x)
					/(distance/80.0)));
				unit.data.goingToPointY=int(unit.data.y+
					((unit.data.goingToPointY-unit.data.y)
					/(distance/80.0)));
			}

			// now mix our going to with the latest one
			unit.data.goingToPointX=(tmpGoX+unit.data.goingToPointX)/2;
			unit.data.goingToPointY=(tmpGoY+unit.data.goingToPointY)/2;
		}
	}
	}
	// FOLLOWERS_TEST_END

	// set the rest of the goingToPoint stuff
	unit.data.goingToPoint=true;
	if (unit.data.isPosRelative) {
		// correct the coordinates to be relative
		unit.data.goingToPointX-=gameWorld.xCamera;
		unit.data.goingToPointY-=gameWorld.yCamera;
	}

	// set the speed for going to such point
	setSpeedForLinealMove(&unit.data,
		unit.data.goingToPointX, unit.data.goingToPointY,
		unit.data.linearSpeed*0.8);
	
	// all ok
	return true;
}// carrierLinearMovementDoneEvent 



// carrierFinishUntouchabilityEvent *******
// This is the code that carriers execute when they finish being
// untouchable, while still being alive.
bool carrierFinishUntouchabilityEvent(
	vectorListNodeStruct<enemyUnitStruct>& unit)
{
	// we used to change the frame at this place...
	// we don't do it anymore
	//genericUnitTypeDataStruct* myType=&(enemyType[unit.data.unitType]);
	//unit.data.currentFrame=myType->firstFrame;// normal frame
	// continue auto framing
	//unit.data.vFramesPassed=0;

	// do what's needed
	unit.data.trembling=false;// don't tremble anymore

	// all ok
	return true;
}// carrierFinishUntouchabilityEvent
