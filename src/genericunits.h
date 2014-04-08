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

/*
GENERICUNITS.*:
(functions for managing the generic units)


These files contain the functions that are common to all the unit types,
including linear movement and other useful methods.

*/

#ifndef FILE_GENERICUNITS_INCLUDED
#define FILE_GENERICUNITS_INCLUDED

#define FILE_GENERICUNITS_VERSION "30-11-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "gamedata.h"// data types of this game


// Some functions related to linear movement
bool setSpeedForLinealMove(genericUnitStruct* unit, long double xTo, long double yTo, double linealSpeed);
bool unintelligentLinealMove(genericUnitStruct* unit);

// Some functions related to framing
frameDataStruct* getUnitFramesList(genericUnitStruct* unit);
frameDataStruct* getUnitFrame(genericUnitStruct* unit);

bool constructContainerFrame(genericUnitStruct* unit, frameDataStruct* frameToWrite);
frameDataStruct constructContainerFrame(genericUnitStruct* unit);
bool constructSlaverFullFrame(genericUnitStruct* unit, frameDataStruct* frameToWrite);
frameDataStruct constructSlaverFullFrame(genericUnitStruct* unit);

genericUnitStruct* getUnitPointer(gameObjectPointerStruct unitInfo);
bool removeUnit(genericUnitStruct* unit);

bool freeSlavesOf(genericUnitStruct* unit);

bool autoFrameUnit(genericUnitStruct* unit);
bool moveWithOwner(genericUnitStruct* unit);
bool isUnitInsideTheScreen(genericUnitStruct* unit);

#endif// FILE_GENERICUNITS_INCLUDED
