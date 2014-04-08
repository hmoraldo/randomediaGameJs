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

#include "genericunits.h"// for managing the generic units...
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "generalmaths.h"// for mathematical functions
#include <math.h>// for using sqrt

// setSpeedForLinealMove *****
// This method calculates speedX and speedY for moving a unit lineally.
bool setSpeedForLinealMove(
	genericUnitStruct* unit,// enemy unit to move
	long double xTo,// coordinates of destination point
	long double yTo,
	double linealSpeed)// speed to use in the lineal movement
{
	long double dx, dy, dDiagonal;// delta x, y and diagonal
	double ratio;// ratio between distances

	// calculate our delta and other initial data
	dx=xTo-(unit->x);
	dy=yTo-(unit->y);
	dDiagonal=sqrt((dx*dx)+(dy*dy));// by Pythagoras theorem
	linealSpeed=ABS(linealSpeed);// forget about negative quantities

	// let's avoid divisions by zero
	if (dDiagonal==0) {
		unit->speedX=0;
		unit->speedY=0;
		return true;// all ok
	}

	// calculate the ratio between the distance to travel by in total,
	// and the distance to travel every piece of time (velocity)
	ratio=linealSpeed/dDiagonal;

	// now calculate the definitive horizontal and vertical speed...
	unit->speedX=dx*ratio;
	unit->speedY=dy*ratio;

	return true;// all ok
}// setSpeedForLinealMove


// unintelligentLinealMove ******
// This method executes the movement of a generic unit, if it's required
// that the unit should be moved to a given point
bool unintelligentLinealMove(
	genericUnitStruct* unit)// enemy unit to move
{
	long double gx=unit->goingToPointX;// point to go coordinates
	long double gy=unit->goingToPointY;

	// check it's necessary to move the unit such way
	if (!unit->goingToPoint) return true;

	// we assume the speed has already been calculated
	double sx=unit->speedX;// for desired speed
	double sy=unit->speedY;

	// check for relative coordinates
	if (unit->isPosRelative) {
		gx+=gameWorld.xCamera;
		gy+=gameWorld.yCamera;
	}

	// move it, checking that it doesn't surpass the destine
	// point
	// x movement
	if (ABS(gx-unit->x)<=ABS(sx)) {
		// reached the desired point
		unit->x=gx;
	}else{
		// move it
		unit->x+=sx;
	}
	// y movement
	if (ABS(gy-unit->y)<=ABS(sy)) {
		// reached the desired point
		unit->y=gy;
	}else{
		// move it
		unit->y+=sy;
	}

	// point reached?
	if (unit->x==gx && unit->y==gy) {
		unit->goingToPoint=false;
		unit->speedX=0;
		unit->speedY=0;
	}

	return true;// all ok
}// unintelligentLinealMove


// getUnitFramesList ************
// Obtain the list of frames of the generic unit, or NULL if that cannot
// be done.
frameDataStruct* getUnitFramesList(
	genericUnitStruct* unit)// enemy unit to obtain the current frame from
{
	genericUnitTypeDataStruct* typeDataList;
	genericUnitTypeDataStruct* myUnitType;

	typeDataList=getTypeDataStruct(unit->objectType);

	if (typeDataList==NULL) return NULL;

	myUnitType=&(typeDataList[unit->unitType]);

	return myUnitType->frames;
}// getUnitFramesList

// getUnitFrame *****************
// Obtain the current frame of the generic unit, or NULL if that cannot
// be done.
frameDataStruct* getUnitFrame(
	genericUnitStruct* unit)// enemy unit to obtain the current frame from
{
	frameDataStruct* frames;

	frames=getUnitFramesList(unit);

	if (frames==NULL)
		return frames;
	else
		return &(frames[unit->currentFrame]);
}// getUnitFrame

// constructContainerFrame ******
// Create a frame structure whose information is such that the created
// frame is bigger than any other frame used by the unit (that is, that
// a frame with such dimensions would cover any other frame, so ensuring
// that pixels won't ever be written outside these coordinates in a
// given unit)
// Only the handle coordinates and the frame size are set up in this
// fake frame
// Big collision coordinates aren't calculated.
bool constructContainerFrame(
	genericUnitStruct* unit,// enemy unit to obtain the current frame from
	frameDataStruct* frameToWrite)// frame struct to write to
{
	frameDataStruct* frames;
	int framesCount;
	frameDataStruct* thisFrame;

	// these are: distance from the x handle to the left and right border,
	// and distance to the y handle to the top and bottom border. These
	// four values will be maximized.
	int wToLeft=0, wToRight=0, hToTop=0, hToBottom=0;
	int tempWToLeft, tempWToRight, tempHToTop, tempHToBottom;

	// calculate some things
	frames=getUnitFramesList(unit);
	if (frames==NULL || frameToWrite==NULL)
		return false;// error!
	framesCount=getFramesCount(frames);
	if (framesCount<=0) return false;// we don't like zero frames!

	// now go set up the fields according to the data of all the forms
	for (thisFrame=frames; thisFrame-frames<framesCount; thisFrame++) {
		// calculate the new four values
		tempWToLeft=thisFrame->handlerX;
		tempWToRight=thisFrame->w-thisFrame->handlerX;
		tempHToTop=thisFrame->handlerY;
		tempHToBottom=thisFrame->h-thisFrame->handlerY;

		// obtain the most important values of those
		wToLeft=(wToLeft>tempWToLeft?wToLeft:tempWToLeft);
		wToRight=(wToRight>tempWToRight?wToRight:tempWToRight);
		hToTop=(hToTop>tempHToTop?hToTop:tempHToTop);
		hToBottom=(hToBottom>tempHToBottom?hToBottom:tempHToBottom);
	}

	// write the final frame data
	frameToWrite->handlerX=wToLeft;
	frameToWrite->handlerY=hToTop;
	frameToWrite->w=wToLeft+wToRight;
	frameToWrite->h=hToTop+hToBottom;

	return true;// all ok...
}// constructContainerFrame

// Another version of this function, that passes the frame information
// directly through the stack...
frameDataStruct constructContainerFrame(
	genericUnitStruct* unit)// enemy unit to obtain the current frame from
{
	frameDataStruct frame;

	constructContainerFrame(unit, &frame);// don't care about errors

	// ready, we did the same that constructContainerFrame, but returning
	// the frame through stack
	return frame;
}// constructContainerFrame


// constructSlaverFullFrame ******
// Compose the frame information of a unit and all its recursive slaves,
// so that the composed frame information has the combined boundaries
// of all the slaves and the unit itself. This way, a frameDataStruct
// is created containing the smallest boundaries that let all these
// units fit there.
// Only the handle coordinates and the frame size are set up in this
// fake frame
// Combined collision coordinates aren't calculated.
bool constructSlaverFullFrame(
	genericUnitStruct* unit,// slaver enemy unit to obtain the current frame from
	frameDataStruct* frameToWrite)// frame struct to write to
{
	frameDataStruct* unitFrame;
	frameDataStruct slaveFrame;
	genericUnitStruct* slave;

	// these are: distance from the x handle to the left and right border,
	// and distance to the y handle to the top and bottom border. These
	// four values will be calculated, once for visibility boundaries,
	// and then for collision boundaries.
	int wToLeft=0, wToRight=0, hToTop=0, hToBottom=0;
	int tempWToLeft, tempWToRight, tempHToTop, tempHToBottom;
	int xOff, yOff;

	// get the frame of this unit
	unitFrame=getUnitFrame(unit);
	if (unitFrame==NULL) return false;// check for errors

	// does it have any slaves?
	if (unit->slaveUnit.objectType==FILE_GAMEDATA_GAMEOBJECT_EMPTY) {
		// base case, no more slaves!
		// return the information of the current frame 
		(*frameToWrite)=(*unitFrame);
	}else{
		// get the slave first
		slave=getUnitPointer(unit->slaveUnit);
		if (slave==NULL) return false;// check for errors

		// write in slaveFrame the information of the slave unit first
		if (!constructSlaverFullFrame(slave, &slaveFrame)) return false;

		// now combine the information of this unit with the one of
		// the slave
		// VISIBILITY CALCULATION
		// calculate the displacement between the slave and the unit
		xOff=(unitFrame->adherenceX)-(slaveFrame.adherenceX);
		yOff=(unitFrame->adherenceY)-(slaveFrame.adherenceY);
		// calculate the new four values, for the unit
		wToLeft=unitFrame->handlerX;
		wToRight=unitFrame->w-unitFrame->handlerX;
		hToTop=unitFrame->handlerY;
		hToBottom=unitFrame->h-unitFrame->handlerY;
		// and for the slave (displaced as needed)
		tempWToLeft=slaveFrame.handlerX-xOff;
		tempWToRight=slaveFrame.w-slaveFrame.handlerX+xOff;
		tempHToTop=slaveFrame.handlerY-yOff;
		tempHToBottom=slaveFrame.h-slaveFrame.handlerY+yOff;

		// obtain the combination of both values
		wToLeft=(wToLeft>tempWToLeft?wToLeft:tempWToLeft);
		wToRight=(wToRight>tempWToRight?wToRight:tempWToRight);
		hToTop=(hToTop>tempHToTop?hToTop:tempHToTop);
		hToBottom=(hToBottom>tempHToBottom?hToBottom:tempHToBottom);

		// write the final frame data
		frameToWrite->handlerX=wToLeft;
		frameToWrite->handlerY=hToTop;
		frameToWrite->w=wToLeft+wToRight;
		frameToWrite->h=hToTop+hToBottom;
	}

	return true;// all ok
}// constructSlaverFullFrame


// Another version of this function, that passes the frame information
// directly through the stack...
frameDataStruct constructSlaverFullFrame(
	genericUnitStruct* unit)// slaver enemy unit to obtain the current frame from
{
	frameDataStruct frame;

	constructSlaverFullFrame(unit, &frame);// don't care about errors

	// ready, we did the same that constructContainerFrame, but returning
	// the frame through stack
	return frame;
}// constructContainerFrame

// This functions frees the slave units (if there are any) of a given
// unit
bool freeSlavesOf(genericUnitStruct* unit)
{
	genericUnitStruct* slave;

	if (unit->slaveUnit.objectType==FILE_GAMEDATA_GAMEOBJECT_EMPTY) {
		// nothing to do
		return true;// all ok
	}else{
		// get the slave unit
		slave=getUnitPointer(unit->slaveUnit);
		if (slave==NULL) return false;

		// unlink both objects
		unit->slaveUnit.objectType=FILE_GAMEDATA_GAMEOBJECT_EMPTY;
		unit->slaveUnit.id=FILE_GAMEDATA_WHATEVER_ID;
		slave->slaveOf.objectType=FILE_GAMEDATA_GAMEOBJECT_EMPTY;
		slave->slaveOf.id=FILE_GAMEDATA_WHATEVER_ID;

		// make the unlinked slave visible
		slave->nowDrawable=true;
	}

	return true;// all ok
}// freeSlaveOf

// This function returns the pointer to the unit unitInfo points to,
// or null if such unit does not exist anymore.
genericUnitStruct* getUnitPointer(
	gameObjectPointerStruct unitInfo)
{
	genericUnitStruct* unit;

	// look for it, following the object type information
	switch(unitInfo.objectType) {
	case FILE_GAMEDATA_GAMEOBJECT_EMPTY:
		// the unit does not exist
		unit=NULL;
		break;
	case FILE_GAMEDATA_GAMEOBJECT_PLAYER:
		unit=&(playerList.getNode(unitInfo.listIndex)->data);
		break;
	case FILE_GAMEDATA_GAMEOBJECT_BULLET:
		unit=&(bulletList.getNode(unitInfo.listIndex)->data);
		break;
	case FILE_GAMEDATA_GAMEOBJECT_ENEMY:
		unit=&(enemyList.getNode(unitInfo.listIndex)->data);
		break;
	case FILE_GAMEDATA_GAMEOBJECT_WAVEGENERATOR:
		unit=&(waveGenList.getNode(unitInfo.listIndex)->data);
		break;
	default:
		// bad news, that type isn't allowed!
		unit=NULL;
		break;
	}

	// invalidate the unit if the id doesn't match
	if (unit!=NULL && unit->id!=unitInfo.id) unit=NULL;

	// all ok, return the unit (it could be NULL)
	return unit;
}// getUnitPointer

// autoFrameUnit *******
// This function does the automatic frame switch tasks that are supposed
// to be done once per virtual frame. Basically, all it does is to
// make the unit animation to continue until its frame information is
// changed from outside.
bool autoFrameUnit(genericUnitStruct* unit)
{
	frameDataStruct* myUnitFrame=getUnitFrame(unit);

	// auto frame it if we should...
	if (myUnitFrame->autoShowVirtualFrames!=0) {
		unit->vFramesPassed++;
		if (unit->vFramesPassed>=(myUnitFrame->autoShowVirtualFrames)) {
			// go to the next frame
			unit->currentFrame=myUnitFrame->autoNextFrame;

			// continue auto framing
			unit->vFramesPassed=0;
		}
	}

	// all ok
	return true;
}// autoFrameUnit

// moveWithOwner *********
// This function moves the given unit with the camera and parent unit
// if it's needed.
bool moveWithOwner(genericUnitStruct* unit)
{
	frameDataStruct* myUnitFrame;
	genericUnitStruct* slave;
	frameDataStruct* slaveFrame;

	// to avoid mistakes, update the frame info
	myUnitFrame=getUnitFrame(unit);

	// if this unit has a slave, it should move it to its own
	// position
	if (unit->slaveUnit.objectType!=FILE_GAMEDATA_GAMEOBJECT_EMPTY) {

		// get the slave unit and its frame information...
		if ((slave=getUnitPointer(unit->slaveUnit))==NULL)
			return false;
		slaveFrame=getUnitFrame(slave);

		// set the slave coordinates
		slave->x=(unit->x)+
			(myUnitFrame->adherenceX)-(slaveFrame->adherenceX);
		slave->y=(unit->y)+
			(myUnitFrame->adherenceY)-(slaveFrame->adherenceY);
	}
	
	return true;// all ok
}// moveWithCameraAndOwner

// isUnitInsideTheScreen
// This function tells whether a given unit is inside the screen (visible)
// or not.
bool isUnitInsideTheScreen(genericUnitStruct* unit)
{
	frameDataStruct combFrame;

	if (!constructSlaverFullFrame(unit, &combFrame)) return false;

	// calculate the intersection of the biggest frame with the
	// screen rectangle
	return INTERSECTINGRECTANGLES(
		unit->x-(combFrame.handlerX),
		unit->y-(combFrame.handlerY),
		unit->x+(combFrame.w)-(combFrame.handlerX)-1,
		unit->y+(combFrame.h)-(combFrame.handlerY)-1,
		gameWorld.xCamera, gameWorld.yCamera,
		gameWorld.xCamera+gameWorld.wCamera-1,
		gameWorld.yCamera+gameWorld.hCamera-1);
}// isUnitInsideTheScreen
