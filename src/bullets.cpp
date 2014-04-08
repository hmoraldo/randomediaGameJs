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

#include "bullets.h"// for managing the bullets
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "generalmaths.h"// for mathematical functions
#include "genericunits.h"// for managing the generic units...
#include "collisions.h"// for functions related to the collisions of units

// moveBulletUnit *****
// This method moves an individual bullet unit.
bool moveBulletUnit(
	DWORD frameNumber,// current frame
	vectorListNodeStruct<bulletUnitStruct>& unit)// bullet unit to move
{
	genericUnitTypeDataStruct* myType=&(bulletType[unit.data.unitType]);
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
		if (!bulletList.removeNode(bulletList.getIndex(&unit)))
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

	// should we move the unit to some specific point?
	// that is, unintelligent movement vs. intelligent movement
	if (unit.data.goingToPoint) {
		if (!unintelligentLinealMove(&unit.data))
			return false;// failed!
	}else{
		// typical ai
		// move it
		switch (unit.data.unitType) {
		case FILE_GAMEDATA_BULLET_PLAYERFLAME:
			// move always
			unit.data.x+=unit.data.speedX;
			unit.data.y+=unit.data.speedY;
			break;
		}
	}

	// check if it has just stopped going to a point
	if (wasGoingToPoint && !unit.data.goingToPoint) {
		gameObjectPointerStruct objPointer;

		// kill the bullet
		objPointer.id=FILE_GAMEDATA_SOMEWALL_ID;
		objPointer.listIndex=0;// it's not in any list
		objPointer.objectType=FILE_GAMEDATA_GAMEOBJECT_VIRTUALWALL;

		// collided!
		bulletUnitHit(unit, objPointer);
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
			if (!bulletList.removeNode(bulletList.getIndex(&unit)))
				return false;
			// the unit has been removed, we cannot continue here
			return true;
		}
	}
	// END VISIBILITY CHECK

	return true;// all ok
}// moveBulletUnit

// moveBullets ********
// This method moves every bullet in the game world (executed once
// per game frame).
bool moveBullets(DWORD frameNumber)
{
	vectorListNodeStruct<bulletUnitStruct> *unit;

	// Manage the AI of all the bullets on screen...
	for (unit=bulletList.getFirstNode(); unit!=NULL; unit=bulletList.getNextNode(unit)) {
		// don't move this unit if it's not active now
		if (!unit->data.nowActive) continue;

		// move this unit
		if (!moveBulletUnit(frameNumber, *unit)) return false;
	}// for unit

	return true;// all ok
}// moveBullets
