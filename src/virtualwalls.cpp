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

#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include "gameglobals.h"// for using the game objects
#include "scriptman.h"// used for accessing to the script manager object

// memory where to store the virtual walls information
static int* virtualWallLeft;
static int* virtualWallRight;
static int* virtualWallGround;

// height of the virtual walls
static int lWallSize;// left wall size
static int rWallSize;// right wall size
static int gWallSize;// ground wall size
// information for this tad
static bool initialized=false;

// initializeVirtualWalls *******
// Initialize our TAD of virtual walls
bool initializeVirtualWalls()
{
	int i;
	lua_State* lstate;
	int value;

	// check for initialization problems
	if (initialized) return false;
	initialized=true;

	// get the lua state object
	lstate=scriptMan.getLuaObject();

	// set wallSize's value
	lWallSize=scriptMan.getIntVariable("leftWallSize");
	rWallSize=scriptMan.getIntVariable("rightWallSize");
	gWallSize=scriptMan.getIntVariable("groundWallSize");

	// allocate memory
	virtualWallLeft=new int[lWallSize];
	virtualWallRight=new int[rWallSize];
	virtualWallGround=new int[gWallSize];

	// fill left wall with the right values
	for (i=0; i<lWallSize; i++) {
		// get leftVirtualWal[i]
		lua_getglobal(lstate, "leftVirtualWall");
		lua_pushnumber(lstate, i);
		lua_gettable(lstate, -2);
		value=int(lua_tonumber(lstate, -1));
		lua_pop(lstate, 2);

		// save the value
		virtualWallLeft[i]=value;
	}

	// fill right wall with the right values
	for (i=0; i<rWallSize; i++) {
		// get rightVirtualWal[i]
		lua_getglobal(lstate, "rightVirtualWall");
		lua_pushnumber(lstate, i);
		lua_gettable(lstate, -2);
		value=int(lua_tonumber(lstate, -1));
		lua_pop(lstate, 2);

		// save the value
		virtualWallRight[i]=value;
	}

	// fill ground wall with the right values
	for (i=0; i<gWallSize; i++) {
		// get groundVirtualWal[i]
		lua_getglobal(lstate, "groundVirtualWall");
		lua_pushnumber(lstate, i);
		lua_gettable(lstate, -2);
		value=int(lua_tonumber(lstate, -1));
		lua_pop(lstate, 2);

		// save the value
		virtualWallGround[i]=value;
	}

	return true;// all ok
}// initializeVirtualWalls

// deinitializeVirtualWalls *******
// Deinitialize our Tad of virtual walls.
bool deinitializeVirtualWalls()
{
	// check for initialization problems
	if (!initialized) return false;
	initialized=false;

	delete[] virtualWallLeft;
	delete[] virtualWallRight;
	delete[] virtualWallGround;

	return true;// all ok
}// deinitializeVirtualWalls

// getLeftWall *********************
// Gets the x position of the wall at a given y value, for the left
// wall
int getLeftWall(int y)
{
	// check for initialization problems
	if (!initialized) return -1;

	// check if we aren't outside the defined wall
	if (y<0 || y>=lWallSize) return virtualWallLeft[0];

	return virtualWallLeft[y];
}// getLeftWall

// getRightWall *********************
// Gets the x position of the wall at a given y value, for the right
// wall
int getRightWall(int y)
{
	// check for initialization problems
	if (!initialized) return -1;

	// check if we aren't outside the defined wall
	if (y<0 || y>=rWallSize) return virtualWallRight[0];

	return virtualWallRight[y];
}// getRightWall

// getGroundWall *********************
// Gets the x position of the wall at a given y value, for the ground
// wall (ground heights)
int getGroundWall(int x)
{
	// check for initialization problems
	if (!initialized) return -1;

	// check if we aren't outside the defined wall
	if (x<0 || x>=gWallSize) return virtualWallGround[0];

	return virtualWallGround[x];
}// getGroundWall
