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

#include "sheepai.h"// for managing the AI of a specific type of enemies, the sheeps
#include "enemyai.h"// for managing the enemy AI...
#include "genericunits.h"// for managing the generic units...
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include "gamesound.h"// for accessing the functions that convert game events to sound executions



// sheepLinearMovementDoneEvent *******
// This is the code to be executed when a sheep finishes with the
// execution of a linear movement.
bool sheepLinearMovementDoneEvent(
	vectorListNodeStruct<enemyUnitStruct>& unit)
{
	int minX, maxX;
	frameDataStruct* headFrame;
	int headX;// x we consider in the head
	vectorListNodeStruct<enemyUnitStruct> *head;
	frameDataStruct* gettingHeadsFrame;

	// update the gettingHeadsFrame now
	if (unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {

		gettingHeadsFrame=
			&(getUnitFramesList(&unit.data)[
				getFrameIndexAndListByName(
				"whiteSheepGettingAHead")]);
	}else{
		gettingHeadsFrame=
			&(getUnitFramesList(&unit.data)[
				getFrameIndexAndListByName(
				"blackSheepGettingAHead")]);
	}

	// to avoid mistakes, update the frame info
	frameDataStruct* myUnitFrame=getUnitFrame(&unit.data);

	// check what kind of switch it was
	if (unit.data.aiSpecific.sheepData.nowFalling) {
		// it was falling, and has just stopped the free fall
		unit.data.aiSpecific.sheepData.nowFalling=false;
		unit.data.aiSpecific.sheepData.nowJumpingForHigh=false;

		// it has finished falling, make it go to the
		// sheepAtGround frame
		if (unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {

			unit.data.currentFrame=
				getFrameIndexAndListByName("whiteSheepAtGround");
		}else{
			unit.data.currentFrame=
				getFrameIndexAndListByName("blackSheepAtGround");
		}
		// continue auto framing
		unit.data.vFramesPassed=0;

	}else{
		// just make the sheep walk and that's all
		if (!makeSheepWalk(unit)) return false;
	}


	// check if we can get a head now
	if (unit.data.slaveUnit.objectType==
		FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

		// SET YOURSELF AS THE OWNER OF A NEAR HEAD !
		// acceptable range for near heads
		minX=int(unit.data.x)+gettingHeadsFrame->adherenceX-3;
		maxX=int(unit.data.x)+gettingHeadsFrame->adherenceX+3;

		// Check all the heads until finding an useful one
		for (head=enemyList.getFirstNode();
			head!=NULL &&
				unit.data.slaveUnit.objectType==// for interrupting as soon as it gets a near head
				FILE_GAMEDATA_GAMEOBJECT_EMPTY;
			head=enemyList.getNextNode(head)) {

			// frame of the head
			if (head->data.unitType==// it's a head
				FILE_GAMEDATA_ENEMY_HUMANHEAD &&
				head->data.nowActive) {

				headFrame=getUnitFrame(&head->data);
				headX=int(head->data.x)+headFrame->adherenceX;
			}

			// filter the unit
			if (head->data.nowActive &&// it's active
				head->data.unitType==// it's a head
				FILE_GAMEDATA_ENEMY_HUMANHEAD &&
				head->data.slaveOf.objectType==// it's not busy already
				FILE_GAMEDATA_GAMEOBJECT_EMPTY &&
				!head->data.aiSpecific.headData.fallingHead &&// it's not falling anymore
				headX>=minX &&// it's near
				headX<=maxX) {

				// then take that head
				// take it
				unit.data.slaveUnit.id=head->data.id;
				unit.data.slaveUnit.listIndex=
					enemyList.getIndex(head);
				unit.data.slaveUnit.objectType=
					head->data.objectType;

				// mark this one as the other's owner
				head->data.slaveOf.id=unit.data.id;
				head->data.slaveOf.listIndex=
					enemyList.getIndex(&unit);
				head->data.slaveOf.objectType=
					unit.data.objectType;

				// and change the head's frame accordingly
				if (unit.data.unitType==
					FILE_GAMEDATA_ENEMY_WHITESHEEP) {

					head->data.currentFrame=
						getFrameIndexAndListByName(
						"humanHeadCarriedBySheep");
					head->data.vFramesPassed=0;
				}else{
					head->data.currentFrame=
						getFrameIndexAndListByName(
						"humanHeadCarriedByBody");
					head->data.vFramesPassed=0;
				}

				// and change the sheep's frame accordingly
				if (unit.data.unitType==
					FILE_GAMEDATA_ENEMY_WHITESHEEP) {

					unit.data.currentFrame=
						getFrameIndexAndListByName(
						"whiteSheepGettingAHead");
					unit.data.vFramesPassed=0;
				}else{
					unit.data.currentFrame=
						getFrameIndexAndListByName(
						"blackSheepGettingAHead");
					unit.data.vFramesPassed=0;
				}

				// and stop moving this one
				unit.data.goingToPoint=false;

				// we only draw it together with the owner
				head->data.nowDrawable=false;
			}
		}
	}


// old, disabled code
/*	if (unit.data.aiSpecific.sheepData.nowJumpingForHigh) {
		// it was jumping, now make it fall
		unit.data.aiSpecific.sheepData.nowJumpingForHigh=false;

		// set the going to fields
		unit.data.goingToPoint=true;
		// goingToPointX by default
		unit.data.goingToPointX=unit.data.x-
			unit.data.aiSpecific.sheepData.jumpHDistance/2;
		unit.data.goingToPointY=
			getGroundWall(int(unit.data.goingToPointX))-
			getUnitFramesList(&unit.data)[quietFrameNum].cH+
			getUnitFramesList(&unit.data)[quietFrameNum].handlerY;

		// the going to X depends on the presence of heads
		if (unit.data.slaveUnit.objectType==
			FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

			// LOOK FOR A NEAR HEAD !
			// acceptable range for near heads
			minX=int(unit.data.x)-
				unit.data.aiSpecific.sheepData.jumpHDistance
				-unit.data.aiSpecific.sheepData.maxVarianceHD
				+myUnitFrame->adherenceX;
			maxX=int(unit.data.x)+myUnitFrame->adherenceX;
			// Check all the heads until finding an useful one
			for (head=enemyList.getFirstNode();
				head!=NULL; head=enemyList.getNextNode(head)) {

				// frame of the head
				if (head->data.unitType==// it's a head
					FILE_GAMEDATA_ENEMY_HUMANHEAD &&
					head->data.nowActive) {

					headFrame=getUnitFrame(&head->data);
					headX=int(head->data.x)+headFrame->adherenceX;
				}

				// filter the unit
				if (head->data.nowActive &&// it's active
					head->data.unitType==// it's a head
					FILE_GAMEDATA_ENEMY_HUMANHEAD &&
					head->data.slaveOf.objectType==// it's not busy already
					FILE_GAMEDATA_GAMEOBJECT_EMPTY &&
					!head->data.aiSpecific.headData.fallingHead &&// it's not falling anymore
					headX>=minX &&// it's near
					headX<=maxX) {

					// then go for such head
					unit.data.goingToPointX=
						headX-myUnitFrame->adherenceX;
				}
			}
		}

		// set another frame
		// (don't!)
		// unit.data.currentFrame=0; ???
		myUnitFrame=getUnitFrame(&unit.data);
	}else{

		// it was falling from a jump, now make it jump again
		unit.data.aiSpecific.sheepData.nowJumpingForHigh=true;

		// check if we can get a head now
		if (unit.data.slaveUnit.objectType==
			FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

			// SET YOURSELF AS THE OWNER OF A NEAR HEAD !
			// acceptable range for near heads
			minX=int(unit.data.x)+myUnitFrame->adherenceX-3;
			maxX=int(unit.data.x)+myUnitFrame->adherenceX+3;

			// Check all the heads until finding an useful one
			for (head=enemyList.getFirstNode();
				head!=NULL &&
					unit.data.slaveUnit.objectType==// for interrupting as soon as it gets a near head
					FILE_GAMEDATA_GAMEOBJECT_EMPTY;
				head=enemyList.getNextNode(head)) {

				// frame of the head
				if (head->data.unitType==// it's a head
					FILE_GAMEDATA_ENEMY_HUMANHEAD &&
					head->data.nowActive) {

					headFrame=getUnitFrame(&head->data);
					headX=int(head->data.x)+headFrame->adherenceX;
				}

				// filter the unit
				if (head->data.nowActive &&// it's active
					head->data.unitType==// it's a head
					FILE_GAMEDATA_ENEMY_HUMANHEAD &&
					head->data.slaveOf.objectType==// it's not busy already
					FILE_GAMEDATA_GAMEOBJECT_EMPTY &&
					!head->data.aiSpecific.headData.fallingHead &&// it's not falling anymore
					headX>=minX &&// it's near
					headX<=maxX) {

					// then take that head
					// take it
					unit.data.slaveUnit.id=head->data.id;
					unit.data.slaveUnit.listIndex=
						enemyList.getIndex(head);
					unit.data.slaveUnit.objectType=
						head->data.objectType;

					// mark this one as the other's owner
					head->data.slaveOf.id=unit.data.id;
					head->data.slaveOf.listIndex=
						enemyList.getIndex(&unit);
					head->data.slaveOf.objectType=
						unit.data.objectType;

					// and change the head's frame accordingly
					if (unit.data.unitType==
						FILE_GAMEDATA_ENEMY_WHITESHEEP) {

						head->data.currentFrame=
							getFrameIndexAndListByName(
							"humanHeadCarriedBySheep");
						head->data.vFramesPassed=0;
					}else{
						head->data.currentFrame=
							getFrameIndexAndListByName(
							"humanHeadCarriedByBody");
						head->data.vFramesPassed=0;
					}

					// we only draw it together with the owner
					head->data.nowDrawable=false;
				}
			}
		}

		// set the going to fields
		unit.data.goingToPoint=true;
		unit.data.goingToPointX=unit.data.x-
			unit.data.aiSpecific.sheepData.jumpHDistance/2
			+rand()%(unit.data.aiSpecific.sheepData.maxVarianceHD+1);// and add the variance here! (not when falling though)
		unit.data.goingToPointY=
			getGroundWall(int(unit.data.goingToPointX))-
			getUnitFramesList(&unit.data)[quietFrameNum].cH+// jumping frame
			getUnitFramesList(&unit.data)[quietFrameNum].handlerY// jumping frame
			-unit.data.aiSpecific.sheepData.jumpVDistance;// distance to fall from the ground

		// set another frame
		unit.data.currentFrame=quietFrameNum;// jumping frame
		myUnitFrame=getUnitFrame(&unit.data);
	}

	// finish setting up the going to stuff
	if (unit.data.isPosRelative) {
		// correct the coordinates to be relative
		unit.data.goingToPointX-=gameWorld.xCamera;
		unit.data.goingToPointY-=gameWorld.yCamera;
	}
	// set the speed for going to such point
	setSpeedForLinealMove(&unit.data,
		unit.data.goingToPointX, unit.data.goingToPointY,
		unit.data.linearSpeed);
*/
	// all ok
	return true;
}// sheepLinearMovementDoneEvent




// makeSheepWalk ***********
// This function makes a given sheep continue walking to the
// ship to the left.
bool makeSheepWalk(
	vectorListNodeStruct<enemyUnitStruct>& unit)
{
	int walkingFrameNum;
	double speed;
	
	// calculate the walking frame and speed
	if (unit.data.unitType==FILE_GAMEDATA_ENEMY_WHITESHEEP) {

		walkingFrameNum=getFrameIndexAndListByName(
			"whiteSheepWalking");

		speed=unit.data.linearSpeed;
	}else{
		walkingFrameNum=getFrameIndexAndListByName(
			"blackSheepWalking");

		speed=unit.data.linearSpeed;
	}


	// set the going to fields
	unit.data.goingToPoint=true;
	unit.data.goingToPointX=unit.data.x-5;
	unit.data.goingToPointY=
		getGroundWall(int(unit.data.goingToPointX))-
		getUnitFramesList(&unit.data)[walkingFrameNum].cH+
		getUnitFramesList(&unit.data)[walkingFrameNum].handlerY;

	// finish setting up the going to stuff
	if (unit.data.isPosRelative) {
		// correct the coordinates to be relative
		unit.data.goingToPointX-=gameWorld.xCamera;
		unit.data.goingToPointY-=gameWorld.yCamera;
	}
	// set the speed for going to such point
	setSpeedForLinealMove(&unit.data,
		unit.data.goingToPointX, unit.data.goingToPointY,
		speed);

	// all ok
	return true;
}// makeSheepWalk
