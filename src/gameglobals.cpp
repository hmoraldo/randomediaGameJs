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

#include "gameglobals.h"// for using the game objects
#include "globals.h"// for using the program objects
#include "virtualwalls.h"// for obtaining information about the virtual walls that limit units movement
#include "resinfoman.h"// for managing the information related with the use of resources, ie: animation scripts
#include "scriptman.h"// used for accessing to the script manager object
#include "programinfo.h"// for general info about the program itself
#include <string.h>

// THIS GAME ONLY DATA
// Our game world data...
worldDataClass gameWorld;

// Our script manager object
scriptManagerClass scriptMan;

// Some variables for debugging...
bool dbgClipSprites=true;// Tells whether sprites are clipped out of the game world or not

// This variables tell what's going to be the next ID for an game
// object unit (enemy, bullet, player objects, etc)...
DWORD nextGameObjectID;

// Variables related to program modes, ie those switched from the
// console.
// Bounding rectangles utilities
bool drawBoundingRectangles=false;// tells whether the collision rectangles of the different game units should be drawn
bool letSpritesTremble=true;// tells whether the trembling field of the units will be taken into account when drawing
// Wave generators utilities
bool drawWaveGenerators=true;// tells whether we should draw the wave generators or not... in normal mode, they have to be drawn
// Virtual walls utilities
bool drawVirtualWalls=false;// tells whether we should draw the virtual walls or not, for debugging purposes
// Falling limits utilities
bool drawFallingLimits=false;// tells whether we should draw the limits for falling objets or not, for debugging purposes
// Units drawing utilities
bool drawGoingToLines=false;// tells whether we should draw the lines showing the going to trajectories of the units or not
bool drawFollowingLines=false;// tells whether we should draw the lines showing the following to trajectories of the units or not
bool drawHandlerPoints=false;// tells whether we should draw the handler points for each unit on the screen
// Screen shooting utilities
bool takeASingleScreenshot=false;// tells if it's time to take a screenshot now
bool keepTakingScreenshots=false;// tells whether we are in video capture mode (continuous screenshooting) or not

// The lists of objects used in the game
vectorListClass<playerUnitStruct> playerList;// list of players
vectorListClass<enemyUnitStruct> enemyList;// list of enemies
vectorListClass<bulletUnitStruct> bulletList;// list of bullets
vectorListClass<waveGenUnitStruct> waveGenList;// list of wave generators
// Now, the resource objects list.
// The resource objects list itself
resObjectDataStruct resObjectsList[]={
	// BUFFERS
	// Resource: srfScreenSizedBuffer
	{
		// resource
		new graphicResourceClass(
			"srfScreenSizedBuffer",// resname
			NULL,// filename, null for empty resource
			false,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			800, 600),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameIntro, pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// FONTS
	// Resource: srfDahFont
	{
		// resource
		new graphicResourceClass(
			"srfDahFont",// resname
			"data/graphics/dahfont.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			1, 1,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameIntro, pgzon_actualGameRunning, pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfEdiaFont
	{
		// resource
		new graphicResourceClass(
			"srfEdiaFont",// resname
			"data/graphics/ediafont.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			1, 1,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameIntro, pgzon_actualGameRunning, pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// INTRO IMAGES
	// Resource: srfIntroTmgLogo
	{
		// resource
		new graphicResourceClass(
			"srfIntroTmgLogo",// resname
			"data/graphics/introtmg.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameIntro, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfIntroRediaLogo
	{
		// resource
		new graphicResourceClass(
			"srfIntroRediaLogo",// resname
			"data/graphics/splash_def.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameIntro, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// MENU IMAGES
	// Resource: srfMenuBackground
	{
		// resource
		new graphicResourceClass(
			"srfMenuBackground",// resname
			"data/graphics/backintro.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfBackIntroButton
	{
		// resource
		new graphicResourceClass(
			"srfBackIntroButton",// resname
			"data/graphics/backintrobut.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfMenuBackground2
	{
		// resource
		new graphicResourceClass(
			"srfMenuBackground2",// resname
			"data/graphics/backintwo.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfMenuBackground2
	{
		// resource
		new graphicResourceClass(
			"srfMenuBackground2",// resname
			"data/graphics/backintwo.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfMenuBackground3
	{
		// resource
		new graphicResourceClass(
			"srfMenuBackground3",// resname
			"data/graphics/backinthree.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfBackIntroButton3
	{
		// resource
		new graphicResourceClass(
			"srfBackIntroButton3",// resname
			"data/graphics/backintrobuthree.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			true,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// MISCELLANEOUS IMAGES
	// Resource: srfCursorImage
	{
		// resource
		new graphicResourceClass(
			"cursorImage",// resname
			"data/graphics/cursor.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			8, 16,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_gameMenues, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// INGAME GRAPHICS
	// Resource: srfClassicLevelBg
	{
		// resource
		new graphicResourceClass(
			"srfClassicLevelBg",// resname
			"data/graphics/background.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			false,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfClassicLevelFg
	{
		// resource
		new graphicResourceClass(
			"srfClassicLevelFg",// resname
			"data/graphics/foreground.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// IMAGES OF UNITS OF ANY KIND FOR THE GAME
	// Resource: srfPlayerJAvatar
	{
		// resource
		new graphicResourceClass(
			"srfPlayerJAvatar",// resname
			"data/graphics/prot_def2.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			1, 1,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfeneHeadCarrier
	{
		// resource
		new graphicResourceClass(
			"srfeneHeadCarrier",// resname
			"data/graphics/et2_def.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			59, 58,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfeneSheepCarrier
	{
		// resource
		new graphicResourceClass(
			"srfeneSheepCarrier",// resname

			"data/graphics/et1_def.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srffndHead -- friendly head
	{
		// resource
		new graphicResourceClass(
			"srffndHead",// resname
			"data/graphics/cabeza_def.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srffndBlackSheep
	{
		// resource
		new graphicResourceClass(
			"srffndBlackSheep",// resname
			"data/graphics/cuerpo3.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			1, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srffndWhiteSheep
	{
		// resource
		new graphicResourceClass(
			"srffndWhiteSheep",// resname
			"data/graphics/oveja1.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			1, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	},
	// Resource: srfbulPlayerFlame
	{
		// resource
		new graphicResourceClass(
			"srfbulPlayerFlame",// resname
			"data/graphics/rope_def.png",// filename, null for empty resource
			true,// automatic video or system memory deciding?
			false,// videomem
			true, NULL,// file or resource, resource type
			false,// bmp or pcx
			true,// is transparent
			false,// uses transparent color components?
			0, 0, 0,// transparent color RGB components
			0, 0,// transparent color coordinates
			0, 0),// size (w,h) for the image if empty (filename=NULL)
		// used in zones, finishes with pgzon_invalidZone
		{pgzon_actualGameRunning, pgzon_invalidZone},
		// is the resource necessary?
		true,
		// never deallocate?
		false,
		// reallocate always?
		true
	}
};// resObjectsList

// Size of the resource objects list
int resObjectsListSize=sizeof(resObjectsList)/sizeof(resObjectDataStruct);


// The font objects...
bitmappedFixedFontClass fntDahFont;
bitmappedFixedFontClass fntEdiaFont;
bitmappedFixedFontClass fntEdiaBigFont;



// FRAMELISTSLIST info
// This is a list where each node is actually a pointer (with some
// extra information) to a list of frames.
frameListDataStruct frameListsList[]={
	{// frplJAvatar
	NULL,// a pointer to the list of frames itself
	"frplJAvatar",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frenWhiteSheep
	NULL,// a pointer to the list of frames itself
	"frenWhiteSheep",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frenBlackSheep
	NULL,// a pointer to the list of frames itself
	"frenBlackSheep",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frenHumanHead
	NULL,// a pointer to the list of frames itself
	"frenHumanHead",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frenSheepCarrier
	NULL,// a pointer to the list of frames itself
	"frenSheepCarrier",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frenHeadCarrier
	NULL,// a pointer to the list of frames itself
	"frenHeadCarrier",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frblPlayerFlame
	NULL,// a pointer to the list of frames itself
	"frblPlayerFlame",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frwgWhiteSheeps
	NULL,// a pointer to the list of frames itself
	"frwgWhiteSheeps",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frwgBlackSheeps
	NULL,// a pointer to the list of frames itself
	"frwgBlackSheeps",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frwgHumanHeads
	NULL,// a pointer to the list of frames itself
	"frwgHumanHeads",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// frwgSheepsOut
	NULL,// a pointer to the list of frames itself
	"frwgSheepsOut",// a name for such frame list
	-1// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	},
	{// empty node, delimiting the end of the list
	NULL,// a pointer to the list of frames itself
	NULL,// a name for such frame list
	0// an integer value telling the size of the list of frames, excluding the last 'null' node when it's present
	}
};// frameListsList



// PLAYERTYPE info
// Here is stored the information about the way the different player
// types have to behave and work.
genericUnitTypeDataStruct playerType[]={
	// FILE_GAMEDATA_PLAYER_EMPTY 0
	{FILE_GAMEDATA_PLAYER_EMPTY,// int playerType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// Here used to be FILE_GAMEDATA_PLAYER_CLASSICBROWNSHIP 1
	{1,// int playerType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_PLAYER_SMALLBROWNSHIP 2
	{2,// int playerType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_PLAYER_BLUESUIT 3
	{3,// int playerType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_PLAYER_JAVATAR 4
	{FILE_GAMEDATA_PLAYER_JAVATAR,// int playerType
	"javatarNormal",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frplJAvatar",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0}// pixels to tremble when untouchable (an even value)
};// playerType

// ENEMYTYPE info
// Here is stored the information about the way the different enemy
// types have to behave and work.
genericUnitTypeDataStruct enemyType[]={
	// FILE_GAMEDATA_ENEMY_EMPTY 0
	{FILE_GAMEDATA_ENEMY_EMPTY,// int enemyType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// Here used to be FILE_GAMEDATA_ENEMY_SMALLGREENSHIP 1
	{1,// int enemyType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// Here used to be FILE_GAMEDATA_ENEMY_GREENOCTOPUS 2
	{2,// int enemyType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_ENEMY_WHITESHEEP 3
	{FILE_GAMEDATA_ENEMY_WHITESHEEP,// int enemyType
	"whiteSheepCarried",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frenWhiteSheep",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_ENEMY_BLACKSHEEP 4
	{FILE_GAMEDATA_ENEMY_BLACKSHEEP,// int enemyType
	"blackSheepCarried",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frenBlackSheep",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_ENEMY_HUMANHEAD 5
	{FILE_GAMEDATA_ENEMY_HUMANHEAD,// int enemyType
	"humanHeadCarriedByHeadCarrier",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frenHumanHead",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_ENEMY_SHEEPCARRIER 6
	{FILE_GAMEDATA_ENEMY_SHEEPCARRIER,// int enemyType
	"sheepCarrierNormal",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frenSheepCarrier",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_ENEMY_HEADCARRIER 7
	{FILE_GAMEDATA_ENEMY_HEADCARRIER,// int enemyType
	"headCarrierNormal",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frenHeadCarrier",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0}// pixels to tremble when untouchable (an even value)
};// enemyType

// BULLETTYPE info
// Here is stored the information about the way the different bullet
// types have to behave and work.
genericUnitTypeDataStruct bulletType[]={
	// FILE_GAMEDATA_BULLET_EMPTY 0
	{FILE_GAMEDATA_BULLET_EMPTY,// int bulletType

	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	2},// pixels to tremble when untouchable (an even value)
	// Here used to be the FILE_GAMEDATA_BULLET_CLASSICPLAYER 1
	{1,// int bulletType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// Here used to be the FILE_GAMEDATA_BULLET_CLASSICOCTOPUS 2
	{2,// int bulletType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_BULLET_PLAYERFLAME 3
	{FILE_GAMEDATA_BULLET_PLAYERFLAME,// int bulletType
	"flameBulletAppearing",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frblPlayerFlame",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	2}// pixels to tremble when untouchable (an even value)
};// bulletType

// WAVEGENTYPE info
// Here is stored the information about the way the different wave
// generators types have to behave and work.
genericUnitTypeDataStruct waveGenType[]={
	// FILE_GAMEDATA_WAVEGEN_EMPTY 0
	{FILE_GAMEDATA_WAVEGEN_EMPTY,// int waveGenType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_WAVEGEN_WHITESHEEPS 1
	{FILE_GAMEDATA_WAVEGEN_WHITESHEEPS,// int waveGenType
	"waveGenWSheepsStart",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frwgWhiteSheeps",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS 2
	{FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS,// int waveGenType
	"waveGenBSheepsStart",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frwgBlackSheeps",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_WAVEGEN_HUMANHEADS 3
	{FILE_GAMEDATA_WAVEGEN_HUMANHEADS,// int waveGenType
	"waveGenHeadsStart",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frwgHumanHeads",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_WAVEGEN_SHEEPSOUT 4
	{FILE_GAMEDATA_WAVEGEN_SHEEPSOUT,// int waveGenType
	"waveGenSheepsOutStart",// name of the first frame, for calculating it
	-1,// first frame, load later
	"frwgSheepsOut",// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0},// pixels to tremble when untouchable (an even value)
	// FILE_GAMEDATA_WAVEGEN_UNDEFINED 10
	{FILE_GAMEDATA_WAVEGEN_UNDEFINED,// int waveGenType
	NULL,// name of the first frame, for calculating it
	0,// first frame
	NULL,// the name of the vector which contains the frames info
	NULL,// pointer to the frames info
	0}// pixels to tremble when untouchable (an even value)
};// waveGenType


// SET-UP'ers
// ==========

// setupGameDataStructures ********
// This function manages the initialization of all the data
// structures that are going to be used in the execution of the
// game.
bool setupGameDataStructures()
{
	// set the unit type data structures up
	genericUnitTypeDataStruct* typeList;
	int i, j, siz;

	// first, set up the resources information...
	if (!initializeResInfo()) {
		logger.logLine("Failed when executing initializeResInfo");
		return false;
	}

	// let's load the firstFrame information of every unit
	// type
	for (i=0; i<4; i++) {
		// we select a types list each time...
		switch(i) {
		case 0:
			siz=sizeof(playerType)/sizeof(genericUnitTypeDataStruct);
			typeList=playerType;
			break;
		case 1:
			siz=sizeof(enemyType)/sizeof(genericUnitTypeDataStruct);
			typeList=enemyType;
			break;
		case 2:
			siz=sizeof(bulletType)/sizeof(genericUnitTypeDataStruct);
			typeList=bulletType;
			break;
		case 3:
			siz=sizeof(waveGenType)/sizeof(genericUnitTypeDataStruct);
			typeList=waveGenType;
			break;
		}

		for (j=0; j<siz; j++) {
			// set the first frame...
			typeList[j].firstFrame=
				getFrameIndexAndListByName(
				typeList[j].firstFrameName);
			// set the frames list for this unit type
			if (typeList[j].frameListName!=NULL) {
				char* flname=typeList[j].frameListName;

				typeList[j].frames=
					getFrameListNodeByName(flname)->frameList;
			}
		}
	}

	// Time to initialize the game stuff...
	// The list of players...
	if (!playerList.initialize(5)) {
		// error
		closeAll("playerList.init failed.");
		return false;
	}
	// The list of enemies...
	if (!enemyList.initialize(400)) {
		// error
		closeAll("enemyList.init failed.");
		return false;
	}
	// The list of bullets...
	if (!bulletList.initialize(400)) {
		// error
		closeAll("bulletList.init failed.");
		return false;
	}
	// The list of wave generators...
	if (!waveGenList.initialize(10)) {
		// error
		closeAll("waveGenList.init failed.");
		return false;
	}

	// Create the virtual walls
	if (!initializeVirtualWalls()) {
		// error
		closeAll("virtualWalls.init failed.");
		return false;
	}

	// Set the current language from the options file
	currentLanguage=language_english;
	// set the current language in lua
	scriptMan.setCurrentLanguageInLua(currentLanguage);
	// set the current sound state in the program
	programSoundEnabled=false;
	// set the current sound state in lua
	scriptMan.setSoundEnabledInLua(
		programSoundEnabled, programSoundActive);

	return true;
}// setupGameDataStructures

// Methods for setting up units... (these functions have to be used
// when creating a new unit, before setting all its data).

// setupGenericUnit sets the usual data of units up, only to be used
// internally by other specific set-up'ers
bool setupGenericUnit(genericUnitStruct& unit, int unitType)
{
	// set it all up

	// type related stuff
	unit.unitType=unitType;

	// general stuff
	// assignate an identifier to this enemy
	unit.id=nextGameObjectID;
	nextGameObjectID++;

	// set other values to default... most of the times it will be
	// meaningless though
	unit.speedX=0; unit.speedY=0;
	unit.minSpeedX=0; unit.maxSpeedX=0;
	unit.minSpeedY=0; unit.maxSpeedY=0;
	unit.linearSpeed=0;// linear speed to be used always, by the unit (per virtual frame)
	unit.vFramesPassed=0;
	unit.x=0; unit.y=0;

	// Set the relative position stuff (relative to camera)
	unit.isPosRelative=false;// are xFromCamera and yFromCamera used for relative position?
	unit.xFromCamera=0; unit.yFromCamera=0;// relative position to camera

	// Set the data telling the way this unit behaves
	unit.energyToDrop=0;// energy this unit has to drop when touching any other unit
	unit.decrementableEnergy=true;// tells whether the unit energy can become smaller or not

	// Set the data telling the current unit state
	unit.nowActive=true;// tells if the unit is active and playing right now

	unit.nowDrawable=true;// tells if the unit should be drew or not (only affects the drawing of a unit, not collision neither anything else), that is, a visibility state
	unit.trembling=false;// tells if the unit is trembling right now

	// Set the slave related stuff
	unit.slaveOf.objectType=FILE_GAMEDATA_GAMEOBJECT_EMPTY;
	unit.slaveUnit.objectType=FILE_GAMEDATA_GAMEOBJECT_EMPTY;
	unit.slaveOf.id=FILE_GAMEDATA_WHATEVER_ID;
	unit.slaveUnit.id=FILE_GAMEDATA_WHATEVER_ID;

	// Data for unintelligent unit movement
	unit.goingToPoint=false;// should the ai keep following that point now?
	unit.goingToPointX=0;// point the unit is going to, if any
	unit.goingToPointY=0;

	return true;// all ok
}// setupGenericUnit

// setupEnemyUnit sets an enemy unit up, by making all the values in the
// struct be the default values for that enemy type...
bool setupEnemyUnit(enemyUnitStruct& unit, int unitType)
{
	// set it all up

	// some score values, all together
	const int headCarrierScore=
		scriptMan.getIntVariable("headCarrierScore");// head carrier hit
	const int sheepCarrierScore=
		scriptMan.getIntVariable("sheepCarrierScore");// sheep carrier hit
	const int headScore=
		scriptMan.getIntVariable("headScore");// head by itself
	const int wSheepScore=
		scriptMan.getIntVariable("wSheepScore");// sheep by itself
	const int wSheepScoreWithHead=
		scriptMan.getIntVariable("wSheepScoreWithHead");// sheep + head
	const int bSheepScore=
		scriptMan.getIntVariable("bSheepScore");// sheep by itself
	const int bSheepScoreWithHead=
		scriptMan.getIntVariable("bSheepScoreWithHead");// sheep + head

	// first set the generic stuff up
	if (!setupGenericUnit(unit, unitType)) return false;

	// type related stuff
	unit.currentFrame=enemyType[unitType].firstFrame;

	// general stuff
	unit.objectType=FILE_GAMEDATA_GAMEOBJECT_ENEMY;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*

	// set x to be, by default, just a pixel inside the screen
	unit.x=gameWorld.xCamera+gameWorld.wCamera+
		enemyType[unit.unitType].frames[unit.currentFrame].handlerX-1;

	// Set the data telling the way this unit behaves
	unit.untouchableTimeMax=int(0.4*double(desiredFramesPerSecond));// maximum value for untouchableTimeLeft
	unit.unarmedTimeMax=int(0.4*double(desiredFramesPerSecond));// maximum value for unarmedTimeLeft
	unit.energyToDrop=8;// energy this unit has to drop when touching any other unit
	// Generally, units aren't supposed to be out of the screen ever
	unit.timeOutsideTheScreenMax=0;// maximum value for timeOutsideTheScreenLeft

	// Set the data telling the current unit state
	unit.untouchableTimeLeft=0;// for how much more time (in vframes) the unit will be untouchable
	unit.unarmedTimeLeft=0;// for how much more time (in vframes) the unit won't be able to shoot
	unit.timeOutsideTheScreenLeft=0;// for how much more time (in vframes) the unit will be able to be outside the screen without being removed
	unit.justGoingOut=false;// tells whether the unit is just going out to the screen, or it's been there from some time (usually affects the state of the collision variables)

	// Data for collision checking
	unit.canBeHitByPlayers=true;// tells whether players can collide with it or not
	unit.canBeHitByBullets=true;// tells whether bullets can collide with it or not

	// set the values which difer for every enemy type
	switch (unitType) {
	case FILE_GAMEDATA_ENEMY_HEADCARRIER:
		// let's give those a larger untouchable time...
		unit.untouchableTimeMax=int(0.7*double(desiredFramesPerSecond));// maximum value for untouchableTimeLeft

		unit.energyLeft=30;// energy left for this unit... 0 for dead
		unit.decrementableEnergy=false;// tells whether the unit energy can become smaller or not
		unit.scoreToGivePerHit=headCarrierScore;// how much score it gives per hit
		break;
	case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
		// let's give those a larger untouchable time...
		unit.untouchableTimeMax=int(0.7*double(desiredFramesPerSecond));// maximum value for untouchableTimeLeft

		unit.energyLeft=30;// energy left for this unit... 0 for dead
		unit.decrementableEnergy=false;// tells whether the unit energy can become smaller or not
		unit.scoreToGivePerHit=sheepCarrierScore;// how much score it gives per hit
		break;
	case FILE_GAMEDATA_ENEMY_BLACKSHEEP:
	case FILE_GAMEDATA_ENEMY_WHITESHEEP:
	case FILE_GAMEDATA_ENEMY_HUMANHEAD:
		// Data for collision checking
		unit.canBeHitByPlayers=false;// tells whether players can collide with it or not
		unit.canBeHitByBullets=false;// tells whether bullets can collide with it or not

		// some general data
		unit.energyLeft=50;// energy left for this unit... 0 for dead
		unit.decrementableEnergy=false;// tells whether the unit energy can become smaller or not
		unit.scoreToGivePerHit=0;// how much score it gives per hit
		break;
	}

	// AI related stuff...
	// Flocking
	switch (unitType) {
	case FILE_GAMEDATA_ENEMY_WHITESHEEP:
		// About current falling state
		unit.aiSpecific.sheepData.nowFalling=false;// tells whether the unit is currently falling or not
		unit.aiSpecific.sheepData.nowJumpingForHigh=false;// tells whether the unit is currently jumping up or not

		// About the way a unit should jump
		unit.aiSpecific.sheepData.jumpHDistance=40;// horizontal distance in pixels, for the jumps
		unit.aiSpecific.sheepData.maxVarianceHD=5;// maximum variance in pixels for the horizontal jumping distance.. the maximum jump will be jumpHDistance+maxVarianceHD
		unit.aiSpecific.sheepData.jumpVDistance=20;// vertical distance in pixels, for the jumps

		// About score when freed
		unit.aiSpecific.sheepData.sheepScoreToGiveWhenFreed=
			wSheepScore;
		unit.aiSpecific.sheepData.sheepScoreToGiveWhenFreedWithAHead=
			wSheepScoreWithHead;

		// Some information about the way it falls
		unit.aiSpecific.sheepData.sheepSpeedWhenFalling=0;// linear speed the unit has when it falls
		break;
	case FILE_GAMEDATA_ENEMY_BLACKSHEEP:
		// About current falling state
		unit.aiSpecific.sheepData.nowFalling=false;// tells whether the unit is currently falling or not
		unit.aiSpecific.sheepData.nowJumpingForHigh=false;// tells whether the unit is currently jumping up or not

		// About the way a unit should jump
		unit.aiSpecific.sheepData.jumpHDistance=40;// horizontal distance in pixels, for the jumps
		unit.aiSpecific.sheepData.maxVarianceHD=5;// maximum variance in pixels for the horizontal jumping distance.. the maximum jump will be jumpHDistance+maxVarianceHD
		unit.aiSpecific.sheepData.jumpVDistance=20;// vertical distance in pixels, for the jumps

		// About score when freed
		unit.aiSpecific.sheepData.sheepScoreToGiveWhenFreed=
			bSheepScore;
		unit.aiSpecific.sheepData.sheepScoreToGiveWhenFreedWithAHead=
			bSheepScoreWithHead;

		// Some information about the way it falls
		unit.aiSpecific.sheepData.sheepSpeedWhenFalling=0;// linear speed the unit has when it falls
		break;
	case FILE_GAMEDATA_ENEMY_HUMANHEAD:
		// About current falling state
		unit.aiSpecific.headData.fallingHead=false;// tells whether the unit is currently falling or not

		// About score when freed
		unit.aiSpecific.headData.headScoreToGiveWhenFreed=headScore;

		// Some information about the way it falls
		unit.aiSpecific.headData.headSpeedWhenFalling=0;// linear speed the unit has when it falls

		break;
	case FILE_GAMEDATA_ENEMY_SHEEPCARRIER:
	case FILE_GAMEDATA_ENEMY_HEADCARRIER:
		// For having followers and pursued units
		// Enemy unit this unit will have to follow (none in this case)
		unit.aiSpecific.flockerData.unitToFollow.objectType=
			FILE_GAMEDATA_GAMEOBJECT_EMPTY;
		unit.aiSpecific.flockerData.unitToFollow.id=
			FILE_GAMEDATA_WHATEVER_ID;
		break;
	}

	return true;// all ok
}// setupEnemyUnit

// setupPlayerUnit sets an player unit up, by making all the values in the
// struct be the default values for that player type...
bool setupPlayerUnit(playerUnitStruct& unit, int unitType)
{
	// set it all up

	// first set the generic stuff up
	if (!setupGenericUnit(unit, unitType)) return false;


	// type related stuff
	unit.currentFrame=playerType[unitType].firstFrame;

	// general stuff
	unit.objectType=FILE_GAMEDATA_GAMEOBJECT_PLAYER;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*

	// set the keys to default...
	unit.userControlled=true;
/*	unit.keyUp=DIK_UP;
	unit.keyDown=DIK_DOWN;
	unit.keyLeft=DIK_LEFT;
	unit.keyRight=DIK_RIGHT;
	unit.keyShoot=DIK_LCONTROL;
*/
	// Set the relative position stuff (relative to camera)
	unit.isPosRelative=true;// are xFromCamera and yFromCamera used for relative position?
	unit.xFromCamera=60; unit.yFromCamera=210;// relative position to camera

	// set the coordinates to be equal to *FromCamera
	unit.x=unit.xFromCamera;
	unit.y=unit.yFromCamera;

	// Set the data telling the way this unit behaves
	unit.untouchableTimeMax=int(0.6*double(desiredFramesPerSecond));// maximum value for untouchableTimeLeft
	unit.energyToDrop=10;// energy this unit has to drop when touching any other unit
	unit.scoreToGivePerHit=20;// how much score it gives per hit
	unit.canBeHitByPlayers=true;// tells whether players can hurt it or not
	unit.playerUseAllKeyboard=false;// if it's set, we can use special key codes for making many keys do for the same function

	// We don't care about it now... we use events for ending the
	// unarmed thing
	unit.unarmedTimeMax=int(0.8*double(desiredFramesPerSecond));// maximum value for unarmedTimeLeft

	// Set the data telling the current unit state
	unit.nowEnteringGame=true;// tells whether the player is now entering the game area or not
	unit.energyLeft=100;// energy left for this unit... 0 for dead
	unit.untouchableTimeLeft=0;// for how much more time (in vframes) the unit will be untouchable
	unit.unarmedTimeLeft=0;// for how much more time (in vframes) the unit won't be able to shoot
	unit.score=0;// current score this unit has...

	return true;// all ok
}// setupPlayerUnit

// setupBulletUnit sets a bullet unit up, by making all the values in the
// struct be the default values for that bullet type...
bool setupBulletUnit(bulletUnitStruct& unit, int unitType)
{
	// set it all up

	// first set the generic stuff up
	if (!setupGenericUnit(unit, unitType)) return false;

	// type related stuff
	unit.currentFrame=bulletType[unitType].firstFrame;

	// general stuff
	unit.objectType=FILE_GAMEDATA_GAMEOBJECT_BULLET;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*

	// Set the variables which tell what kind of units a given bullet
	// may hit
	unit.canCollideWithPlayers=false;// can this bullet unit collide with players?
	unit.canCollideWithEnemies=false;// can this bullet unit collide with enemies?
	unit.canCollideWithBullets=false;// can this bullet unit collide with other bullets?
	unit.canCollideWithTheOwner=false;// can this bullet unit collide with its owner?

	// Set the data telling the way this unit behaves
	unit.energyToDrop=10;// energy this unit has to drop when touching any other unit
	// Generally, units aren't supposed to be out of the screen ever
	unit.timeOutsideTheScreenMax=0;// maximum value for timeOutsideTheScreenLeft

	// Set the data telling the current unit state
	unit.timeOutsideTheScreenLeft=0;// for how much more time (in vframes) the unit will be able to be outside the screen without being removed
	unit.nowExploding=false;// tells whether this bullet is exploding or not
	// Zero all the owner stuff
	// this tells which unit dropped the bullet
	unit.owner.listIndex=0;
	unit.owner.objectType=FILE_GAMEDATA_GAMEOBJECT_EMPTY;
	unit.owner.id=FILE_GAMEDATA_WHATEVER_ID;

	// set the values which difer for every bullet type
	switch (unitType) {
	case FILE_GAMEDATA_BULLET_PLAYERFLAME:
		break;
	}

	return true;// all ok
}// setupBulletUnit


// setupWaveGenUnit sets a wave gen unit up, by making all the
// values in the struct be the default values for that wave gen type...
bool setupWaveGenUnit(waveGenUnitStruct& unit, int unitType)
{
	// set it all up

	// first set the generic stuff up
	if (!setupGenericUnit(unit, unitType)) return false;

	// type related stuff
	unit.currentFrame=waveGenType[unitType].firstFrame;

	// general stuff
	unit.objectType=FILE_GAMEDATA_GAMEOBJECT_WAVEGENERATOR;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*

	// More specific stuff
	// it's not generating units by default
	unit.generatingNow=false;

	// Enemy wave generation related stuff
	// This nextEnemyWaveX simulates the wave coming from the right,
	// so that every time it gets inside the screen, it's executed.
	unit.distanceBetweenWaves=1;// what's the distance which separates an enemy wave form another
	unit.virtualEnemyWavesSpeedX=1/double(desiredFramesPerSecond);
	unit.nextEnemyWaveX=gameWorld.xCamera+gameWorld.wCamera;// this tells where that virtual enemy wave object is.
	// If virtualEnemyWavesSpeedX is set that way, distance between
	// waves has to be the number of seconds (or fraction of seconds)
	// has to pass between one wave and the next one.

	return true;// all ok
}// setupWaveGenUnit


// FRAME LIST MANAGEMENT methods
// =============================


// getFrameListIndexByName ******
// Given a frame list name, this function returns the index of such
// frame list in the frameListsList list, or -1 for error
int getFrameListIndexByName(char* frameListName)
{
	frameListDataStruct* flist;

	flist=frameListsList;

	// let's traverse the entire lists' list for such a name...
	for(;!(flist->frameList==NULL && flist->listName==NULL); flist++) {
		if (strcmp(flist->listName, frameListName)==0)
			return flist-frameListsList;// found
	}

	// not found, return an error code
	return -1;
}// getFrameListIndexByName

// getFrameListNodeByName ******
// Given a frame list name, this function returns the corresponding
// node of such frame list in the frameListsList list.
frameListDataStruct* getFrameListNodeByName(char* frameListName)
{
	int listIdx;

	listIdx=getFrameListIndexByName(frameListName);

	return (listIdx==-1?NULL:&(frameListsList[listIdx]));
}// getFrameListNodeByName

// getFramesCount ********
// Given a frame list name, this function returns the amount of
// different frame nodes that are present in such list, excluding
// the last null node when it's present, or -1 for error
int getFramesCount(char* frameListName)
{
	frameListDataStruct* list;

	list=getFrameListNodeByName(frameListName);
	if (list==NULL) return -1;// error, the list doesn't exist
	
	return getFramesCount(list->frameList);
}// getFramesCount

// getFramesCount *****************
// Obtain a number telling the amount of frames a frames list has.
int getFramesCount(
	frameDataStruct* framesList)// enemy unit to obtain the current frame from
{
	frameDataStruct* thisFrame;

	// traverse the list until finding the latest node, marked by
	// a NULL in the surface field
	for (thisFrame=framesList; thisFrame->surface!=NULL; thisFrame++);

	return thisFrame-framesList;
}// getFramesCount

// getFrameIndexByName *******
// Given a frame list name, and the name of a specific frame, this
// function returns the index of such frame in that specific frame
// list, or -1 for error
int getFrameIndexByName(char* frameListName, char* frameName)
{
	frameListDataStruct* list;
	frameDataStruct* frame;

	list=getFrameListNodeByName(frameListName);
	if (list==NULL) return -1;// error, the list doesn't exist
	
	// traverse that frame list for the frame named that way
	// (don't forget, it's valid to have a frame with no name)
	for (frame=list->frameList;
	frame->surface!=NULL && (frame->frameName==NULL ||
		strcmp(frame->frameName, frameName)!=0);
	frame++);

	// no such frame was found, it's an error
	if (frame->surface==NULL) return -1;

	// otherwise, we found the frame...
	return frame-list->frameList;
}// getFrameIndexByName

// getFrameIndexAndListByName ********
// Given a frame list name, and not the name of a specific frame in
// this version, this function returns the index of such frame in
// the specific frame list it's placed at, and a pointer to
// such list, or -1 for error
int getFrameIndexAndListByName(
	char* frameName,// name of the frame we look for
	frameListDataStruct** foundList)// if not null, a pointer to a pointer that will be set to point to the frame list where it was found
{
	frameListDataStruct* list;
	int idx;

	list=frameListsList;

	// error, no frame is named with a NULL string
	if (frameName==NULL) return -1;

	// let's traverse the entire lists' list for such a frame...
	for(;list->frameList!=NULL; list++) {
		idx=getFrameIndexByName(list->listName, frameName);
		if (idx!=-1) {
			// found!
			if (foundList!=NULL) *foundList=list;
			return idx;
		}
	}

	// not found, return an error code
	logger.logLine("Error getting frame named: %s",
		true, false, true, frameName);
	return -1;
}// getFrameIndexAndListByName 


// FRAME LIST MANAGEMENT methods 2: for special cases
// ==================================================

// getFrameIndexByNameInSheeps ********
// Given a frame list name, and the name of a specific frame, this
// function returns the index of such frame in that specific frame
// list, or -1 for error, adding "white" or "black" to the
// frame name, respecting the color marked by isSheepWhite...
int getFrameIndexByNameInSheeps(
	bool isSheepWhite,
	char* frameListName,
	char* frameNamePart)
{
	const int fnStringLen=50;
	char completeFrameName[fnStringLen];
	const char* stringToAdd=
		isSheepWhite?"white":"black";

	// let's see if the string we get with the modifications would
	// be larger than our string buffer...
	if (strlen(frameNamePart)+strlen(stringToAdd)+1>fnStringLen)
		return -1;// error

	// let's do the string modification
	sprintf(completeFrameName, "%s%s",
		stringToAdd, frameNamePart);

	// do the real work
	return getFrameIndexByName(frameListName, completeFrameName);
}// getFrameIndexByNameInSheeps

// getFrameIndexAndListByNameInSheeps *********
// Given a frame list name, and not the name of a specific frame in
// this version, this function returns the index of such frame in
// the specific frame list it's placed at, and a pointer to
// such list, or -1 for error, adding "white" or "black" to the
// frame name, respecting the color marked by isSheepWhite...
int getFrameIndexAndListByNameInSheeps(
	bool isSheepWhite,
	char* frameNamePart,
	frameListDataStruct** foundList)
{
	frameListDataStruct* list;
	int idx;

	list=frameListsList;

	// let's traverse the entire lists' list for such a frame...
	for(;list->frameList!=NULL; list++) {
		idx=getFrameIndexByNameInSheeps(
			isSheepWhite, list->listName, frameNamePart);
		if (idx!=-1) {
			// found!
			if (foundList!=NULL) *foundList=list;
			return idx;
		}
	}

	// not found, return an error code
	logger.logLine("Error getting frame named: %s",
		true, false, true, frameNamePart);
	return -1;
}// getFrameIndexAndListByNameInSheeps



// RESOURCE LIST GENERAL MANAGEMENT methods
// ========================================

// Given a resource name, this method returns the index of the
// respective resource object in resObjectsList, or -1 if the resource
// object isn't present.
int getResObjectIndexByName(char* resname)
{
	int nodeIdx;

	// check the resource name isn't null...
	// if it's null, it was a call looking for me to return the
	// 'no one' value, so we return this error value... but this
	// specific case isn't a real error but a not found return
	if (resname==NULL) return -1;

	// for every node in the objects list...
	for (nodeIdx=0; nodeIdx<resObjectsListSize; nodeIdx++) {
		// compare the two resource names
		if (strcmp(resObjectsList[nodeIdx].resObj->getResourceName(),
			resname)==0) {

			// both are equal! this index is the right one
			return nodeIdx;
		}
	}// for nodeIdx

	// log the fault...
	logger.logLine("Error getting resource %s from the resources list",
		true, false, true, resname);

	// no object has such name
	return -1;
}// getResObjectIndexByName


// Given a resource name, this method returns a pointer to the
// respective resource object in resObjectsList, or NULL if the resource
// object isn't present.
resourceClass* getResObjectByName(char* resname) {
	int index;

	// obtain the index of that object
	index=getResObjectIndexByName(resname);

	if (index==-1) {
		// the object doesn't exist
		return NULL;
	}else{
		// return a pointer to the object itself
		return resObjectsList[index].resObj;
	}
}// getResObjectByName



// Given a resource name, this method returns a pointer to the graphic
// resource with that name, or NULL if the resource object isn't
// present, or if it isn't of the expected type.
graphicResourceClass* getGraphicResourceByName(char* resname)
{
	resourceClass* object;

	// obtain a pointer to the container object
	object=getResObjectByName(resname);

	if (object==NULL) {
		// the object doesn't exist
		return NULL;
	}else{
		// firstly, check the object type
		if (object->getType()!=FILE_RESOBJECTS_GRAPHIC) return NULL;

		// now return the object contained
		return ((graphicResourceClass*)object);
	}
}// getGraphicResourceByName

// Given a resource name, this method returns a pointer to the
// surface contained by the graphic resource that has that name,
// or NULL if the resource object isn't present, or if it isn't of
// the expected type.
ddSpriteClass* getSurfaceByResourceName(char* resname)
{
	graphicResourceClass* resource;

	// first we obtain the graphic resource
	resource=getGraphicResourceByName(resname);

	if (resource==NULL) {
		// the graphic resource doesn't exist
		return NULL;
	}else{
		// the resource exists, return the contained surface
		return resource->getObjectPointer();
	}
}// getSurfaceByResourceName


// OTHER USEFUL FUNCTIONS FOR THE RESOURCE OBJECTS

// This function tells whether a given resource is in a given
// program zone.
bool isResObjectInProgramZone(
	resObjectDataStruct* resObject,// resource object structure to tell
	programZonesEnum programZone)// program zone
{
	programZonesEnum* ptrPz;

	// Check the entire zones list of this object to know whether
	// the given zone uses it or not.
	// Remember the list finishes with pgzon_invalidZone always.
	for (ptrPz=resObject->usedInZones;
	*ptrPz!=pgzon_invalidZone; ptrPz++)
	{
		// check if this zone is the zone we look for
		if (*ptrPz==programZone) {
			return true;
		}
	}// for ptrPz

	// The resource isn't used by this zone
	return false;
}// isResObjectInProgramZone

// This function is to be used only once, when closing the program, for
// freeing the memory used in the resource classes
bool destroyAllResourcesData()
{
	int nodeIdx;

	// for every node in the objects list...
	for (nodeIdx=0; nodeIdx<resObjectsListSize; nodeIdx++) {
		// delete it
		delete resObjectsList[nodeIdx].resObj;
	}// for nodeIdx

	// all ok
	return true;
}// destroyAllResourcesData

// logAllResourcesState
// This function logs the name of every resource that is currently
// loaded in memory.
bool logAllResourcesState()
{
	int nodeIdx;// node in the resources list
	resourceClass* theObject;// a resource object...

	// for every node in the objects list...
	for (nodeIdx=0; nodeIdx<resObjectsListSize; nodeIdx++) {
		theObject=resObjectsList[nodeIdx].resObj;

		// log the information
		logger.logLine(
			"Resname: %s - Loaded: %s - Needs to be reloaded: %s",
			true, true, true,
			theObject->getResourceName(),
			(theObject->isLoaded()?"yes":"no"),
			(theObject->needsToBeReloaded()?"yes":"no"));
	}// for nodeIdx

	return true;// all ok
}// logAllResourcesState

// OTHER GENERAL USEFUL FUNCTIONS...

// getTypeDataStruct *******
// This function returns the type data struct of a given unit type
genericUnitTypeDataStruct* getTypeDataStruct(int objectType)
{
	switch(objectType) {
	case FILE_GAMEDATA_GAMEOBJECT_PLAYER:
		return playerType;
		break;
	case FILE_GAMEDATA_GAMEOBJECT_ENEMY:
		return enemyType;
		break;
	case FILE_GAMEDATA_GAMEOBJECT_BULLET:
		return bulletType;
		break;
	case FILE_GAMEDATA_GAMEOBJECT_WAVEGENERATOR:
		return waveGenType;
		break;
	}

	return NULL;// failed
}// getTypeDataStruct


