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
GAMEGLOBALS.*:
(repository of game objects)


This is our repository of global game objects. Here we store the
variables related to the different sprites, frames and enemies used in
the game.

*/

#ifndef FILE_GAMEGLOBALS_INCLUDED
#define FILE_GAMEGLOBALS_INCLUDED

#define FILE_GAMEGLOBALS_VERSION "1-5-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "programinfo.h"// for general info about the program itself
#include "scriptman.h"
#include "fixedfontwrap.h"// for creating and displaying fixed bitmapped fonts
#include "resobjects.h"// for accessing to the resource objects (graphics, sounds, etc)
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "optionsfile.h"// for the management of the files where the game options are saved
#include "gamedata.h"
#include "vectorlist.h"
/*#include "audiowrap.h"
*/

// THIS GAME ONLY DATA
// Our game world data...
extern worldDataClass gameWorld;

// Our script manager object
extern scriptManagerClass scriptMan;

// Some variables for debugging...
extern bool dbgClipSprites;// Tells whether sprites are clipped out of the game world or not

// This variables tell what's going to be the next ID for an game
// object unit (enemy, bullet, player objects, etc)...
extern DWORD nextGameObjectID;

// Variables used for options and high scores management
extern optionsFileClass optionsFileObject;// object used for managing the game and program options
extern bool isCurrentlyTypingNameHS;// tells whether the player is currently typing his name for the high score list or not
extern highScoreStruct* highScoreToTypeIn;// pointer to the node in the high scores list where to type in

// Variables related to program modes, ie those switched from the
// console.
// Bounding rectangles utilities
extern bool drawBoundingRectangles;// tells whether the collision rectangles of the different game units should be drawn
extern bool letSpritesTremble;// tells whether the trembling field of the units will be taken into account when drawing
// Wave generators utilities
extern bool drawWaveGenerators;// tells whether we should draw the wave generators or not... in normal mode, they have to be drawn
// Virtual walls utilities
extern bool drawVirtualWalls;// tells whether we should draw the virtual walls or not, for debugging purposes
// Falling limits utilities
extern bool drawFallingLimits;// tells whether we should draw the limits for falling objets or not, for debugging purposes
// Units drawing utilities
extern bool drawGoingToLines;// tells whether we should draw the lines showing the going to trajectories of the units or not
extern bool drawFollowingLines;// tells whether we should draw the lines showing the following to trajectories of the units or not
extern bool drawHandlerPoints;// tells whether we should draw the handler points for each unit on the screen
// Screen shooting utilities
extern bool takeASingleScreenshot;// tells if it's time to take a screenshot now
extern bool keepTakingScreenshots;// tells whether we are in video capture mode (continuous screenshooting) or not

// The lists of objects used in the game
extern vectorListClass<playerUnitStruct> playerList;// list of players
extern vectorListClass<enemyUnitStruct> enemyList;// list of enemies
extern vectorListClass<bulletUnitStruct> bulletList;// list of bullets
extern vectorListClass<waveGenUnitStruct> waveGenList;// list of wave generators


// Now, the resource objects list.
// The structure to be used:
struct resObjectDataStruct{
	// the resource itself...
	resourceClass* resObj;

	// a vector finishing with pgzon_invalidZone, telling what program
	// zones this resource is active in
	programZonesEnum usedInZones[TOTALPROGRAMZONES];

	// a boolean telling whether a resource is real necessary for
	// working, or if the system can work without it
	bool isNecessary;

	// a boolean telling whether a resource should be managed normally,
	// or if it should never be deallocated (unless explicity asked to)
	// until the pgzon_emptyZone zone
	bool neverDeallocate;

	// a boolean telling whether a resource should be reallocated always
	// between program zones or not...
	// resources that use system memory are better reallocated always
	// (to avoid system memory fragmentation)
	bool reallocateAlways;
};
// The resource objects list itself
extern resObjectDataStruct resObjectsList[];
extern int resObjectsListSize;


// The font objects...
extern bitmappedFixedFontClass fntDahFont;
extern bitmappedFixedFontClass fntEdiaFont;
extern bitmappedFixedFontClass fntEdiaBigFont;


// Here is stored the information about the way the different player
// types have to behave and work.
extern genericUnitTypeDataStruct playerType[];

// Here is stored the information about the way the different enemy
// types have to behave and work.
extern genericUnitTypeDataStruct enemyType[];

// Here is stored the information about the way the different bullet
// types have to behave and work.
extern genericUnitTypeDataStruct bulletType[];

// Here is stored the information about the way the different wave
// generators types have to behave and work.
extern genericUnitTypeDataStruct waveGenType[];

// Set-up'ers
// Methods for setting up units...
bool setupGameDataStructures();
bool setupEnemyUnit(enemyUnitStruct& unit, int unitType);
bool setupPlayerUnit(playerUnitStruct& unit, int unitType);
bool setupBulletUnit(bulletUnitStruct& unit, int unitType);
bool setupWaveGenUnit(waveGenUnitStruct& unit, int unitType);

// Frame list management methods
int getFrameListIndexByName(char* frameListName);
frameListDataStruct* getFrameListNodeByName(char* frameListName);
int getFramesCount(char* frameListName);
int getFramesCount(frameDataStruct* framesList);
int getFrameIndexByName(char* frameListName, char* frameName);
int getFrameIndexAndListByName(char* frameName,	frameListDataStruct** foundList=NULL);

// Frame list management methods 2: special cases
int getFrameIndexByNameInSheeps(bool isSheepWhite, char* frameListName, char* frameNamePart);
int getFrameIndexAndListByNameInSheeps(bool isSheepWhite, char* frameNamePart,	frameListDataStruct** foundList=NULL);

// Resource list general management methods
int getResObjectIndexByName(char* resname);
resourceClass* getResObjectByName(char* resname);
graphicResourceClass* getGraphicResourceByName(char* resname);
/*waveResourceClass* getWaveResourceByName(char* resname);
midiResourceClass* getMidiResourceByName(char* resname);*/
ddSpriteClass* getSurfaceByResourceName(char* resname);
/*waveFileClass* getWaveSoundByResourceName(char* resname);
midiMusicClass* getMidiMusicByResourceName(char* resname);
*/
// Other useful functions for the resource objects
bool isResObjectInProgramZone(resObjectDataStruct* resObject, programZonesEnum programZone);
bool destroyAllResourcesData();
bool logAllResourcesState();

// Other general useful functions...
genericUnitTypeDataStruct* getTypeDataStruct(int objectType);


// A small macro for making more readable frame definitions...
#define createFrame( \
	framename, /* name of the frame itself */ \
	resname, /* name of the resource to use */ \
	x, y, w, h, /* x, y, w, h, as usual */ \
	cx, cy, cw, ch, /* cX, cY, cW, cH, as usual, but in absolute coordinates! */ \
	hx, hy, /* handlerX, handlerY, as usual, but in absolute coordinates */ \
	sx, sy, /* spawnBulletsX, spawnBulletsY, as usual, but in absolute coordinates */ \
	ax, ay, /* adherenceX, adherenceY, as usual, but in absolute coordinates */ \
	aNF, /* autoNextFrame, just as usual */ \
	aSVT, /* autoShowVirtualTime, just as autoShowVirtualFrames but with time measured in seconds */ \
	eventName,/* name of the event to execute before going to the next frame, null for none */ \
	groupName /* name of the group of frames this frame belongs to, null for none */ \
	) /* */ \
	{ /* start the structure */ \
	(framename), /* just as usual */ \
	(getSurfaceByResourceName((resname))),/* simply we look for the surface that has this name */ \
	(x), (y), (w), (h), /* just as usual */ \
	(cx)-(x), (cy)-(y), (cw), (ch), /* relativize these */ \
	(hx)-(x), (hy)-(y), /* relativize these, too */ \
	(sx)-(hx), (sy)-(hy), /* relativize these, since hcoordinates */ \
	(ax)-(hx), (ay)-(hy), /* just as with sx and sy */ \
	(aNF), /* just as usual */ \
	(int((aSVT)*double(desiredFramesPerSecond))), /* we calculate the amount of frames that are required for such time in seconds */ \
	(eventName),/* just as usual */ \
	(groupName)/* just as usual */ \
	} /* end the structure */



#endif// FILE_GAMEGLOBALS_INCLUDED
