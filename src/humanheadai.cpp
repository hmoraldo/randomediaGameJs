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

#include "humanheadai.h"// for managing the AI of a specific type of enemies, the human heads
#include "enemyai.h"// for managing the enemy AI...
#include "genericunits.h"// for managing the generic units...
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects

// headLinearMovementDoneEvent *******
// This is the code to be executed when a head finishes with the
// execution of a linear movement.
bool headLinearMovementDoneEvent(
	vectorListNodeStruct<enemyUnitStruct>& unit)
{
	// check what kind of switch it was
	if (unit.data.aiSpecific.headData.fallingHead) {
		// it was falling, and has just stopped the free fall
		unit.data.aiSpecific.headData.fallingHead=false;

		// set another frame
		unit.data.currentFrame=
			getFrameIndexAndListByName("humanHeadNormal");// common frame
	}

	// all ok
	return true;
}// headLinearMovementDoneEvent
