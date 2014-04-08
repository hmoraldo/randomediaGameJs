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
GAMEDATA.*:


This is where we declare the general data and behaviour of the main game
objects. Specific behaviour (like more complex AI) will appear in their
respective files.

Here we are using some structures in a very non-OOP way, half because of
optimization issues (memory, mainly) and half because this way it's a lot
faster to program it.

*/

#ifndef FILE_GAMEDATA_INCLUDED
#define FILE_GAMEDATA_INCLUDED

#define FILE_GAMEDATA_VERSION "11-4-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "win-constants.h"
#include "ddwrap.h"
#include "diwrap.h"
#include "vectorlist.h"// for using the vector list object type

// ************************************
// WORLD DATA CLASS and related stuff
// Zone Numbers Declaration (game zones are program sub zones):
// All the game zone names are gmzon_Z, where Z is the name of the zone.
enum gameZonesEnum {
	gmzon_emptyZone,// null zone
	gmzon_classicZone// classic game zone
};// gameZonesEnum

// Game Stage Numbers Declaration:
// All the game stage names are gmstg_Z, where Z is the name of the stage.
enum gameStagesEnum {
	gmstg_emptyStage,// null initial stage
	gmstg_freeSheeps,// stage for freeing sheeps
	gmstg_freeHeads// stage for freeing human heads
};// gameZonesEnum

// This is the class which manages and stores the current state of the
// game world.
class worldDataClass{
private:
public:
	// General game stuff
	bool isGamePaused;// tells whether the game is currently paused or not

	// Stage & level counting
	gameStagesEnum currentStage;// current stage, cyclic value
	int currentLevel;// current level, there are two stages per level
	int timeLeftToNextStage;// time left (in virtual frames), when it's zero, it's time to advance to the next stageLevel

	// Game options to be set from outside (before gamecode's
	// beforeFrame is called)
	int playersToStart;// tells the amount of players which start the game... right now, it can only be 1 or 2 though

	// Drawing stuff
	ddSurfaceClass* srfGameScreen;// surface where to draw the game
	int screenX, screenY;// where the game has to be drawn, on the game screen

	// Zooming stuff
	bool zoomed;// tells whether we zoom the screen or not
	double zoomBy;// tells the multiplier used for zooming
	int zoomCX, zoomCY;// tells the coordinates where zooming is centered

	// Background related stuff
	long double bgOffX, bgOffY;// offset of the background image in the game
	ddSurfaceClass* srfBg;// surface of the current background

	// Camera related stuff
	long double xCamera, yCamera;// position of the camera in the game world
	DWORD wCamera, hCamera;// size of the camera

	// Camera and background speed related stuff
	double xSpeedCamera, ySpeedCamera;// speed of the camera...
	double bgSpeedX, bgSpeedY;// speed of the background offset

	// Data for the game over
	int maxGameFinishedTimeLeft;// value to set gameFinishedTimeLeft every time the game is finished
	int gameFinishedTimeLeft;// time left (in virtual frames) before the game is supposed to finish, -1 for 'not finishing'
	bool showGameOverMessage;// is time to show the game over message?

	// Data for statistics
	int scoreInHits;// how much of the score was got from hits to stuff carriers and other stuff?
	int scoreInFreeWhiteSheeps;// how much of the score was got from free white sheeps?
	int scoreInFreeBlackSheeps;// how much of the score was got from free black sheeps?
	int scoreInFreeHeads;// how much of the score was got from free heads?

	// Other in-game data
	int yGround;// distance from the top to the ground
	int yGroundForPlayer;// max distance from the top to the feet of the player

	// Data for falling objects
	int minXFallenWSheeps;// minimum x for falling white sheeps
	int maxXFallenWSheeps;// maximum x for falling white sheeps
	int minXFallenBSheeps;// minimum x for falling black sheeps
	int maxXFallenBSheeps;// maximum x for falling white sheeps
	int minXFallenHeads;// minimum x for falling heads
	int maxXFallenHeads;// maximum x for falling heads

	// Methods
	bool initializeGame(int gameZoneNumber);
	bool deinitializeGame();
};// worldDataClass

// ************************************
// GAME OBJECT TYPES DEFINITION and related stuff
// Definition of the existent game objects:
// This object type means the structure does not contain valid object
// data.
#define FILE_GAMEDATA_GAMEOBJECT_EMPTY 0
// This object type means it's an invisible object (marker)
#define FILE_GAMEDATA_GAMEOBJECT_MARKER 1
// This object type means the object is a player
#define FILE_GAMEDATA_GAMEOBJECT_PLAYER 2
// This object type means the object is an enemy
#define FILE_GAMEDATA_GAMEOBJECT_ENEMY 3
// This object type means the object is a bullet
#define FILE_GAMEDATA_GAMEOBJECT_BULLET 4
// This object type means the object is an item
#define FILE_GAMEDATA_GAMEOBJECT_ITEM 5
// This object type means the object is a wave generator
#define FILE_GAMEDATA_GAMEOBJECT_WAVEGENERATOR 6
// This object type means the object is a virtual wall (check
// virtualwalls.h for description)
#define FILE_GAMEDATA_GAMEOBJECT_VIRTUALWALL 7

// ************************************
// Special values for object id numbers
// Id for objects that don't have any real id
#define FILE_GAMEDATA_WHATEVER_ID 0
// Id of the left virtual wall
#define FILE_GAMEDATA_LEFTWALL_ID 1
// Id of the right virtual wall
#define FILE_GAMEDATA_RIGHTWALL_ID 2
// Id of the left, or right virtual wall (we don't care which one)
#define FILE_GAMEDATA_SOMEWALL_ID 3
// First id we'll use to count
#define FILE_GAMEDATA_FIRST_ID 4

// ************************************
// GAME OBJECT POINTER STRUCT and related stuff
// gameObjectPointerStruct declaration:
// gameObjectPointerStruct is a structure used to point to any game object.
// Thanks to this structure, you can have in an only variable all the data
// you need to access to a specific object, that is: the ID, the kind
// of object, and the index in its respective objects list (if any).
// The way to use it is: look at the kind of object, then access to the
// list of objects of that kind, look for the index this
// structure points to, and compare the unique ID. If both are equal,
// and the node is active, that's the searched object, otherwise, it can
// only mean that the searched object doesn't exist anymore (either if
// the IDs are differents, or if both are equal but the object is
// inactive).
// (enemy types are FILE_GAMEDATA_GAMEOBJECT_*)
struct gameObjectPointerStruct {
	DWORD id;// unique identifier for the object pointed
	BYTE objectType;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*
	DWORD listIndex;// index for this object in its respective objects list
};// gameObjectPointerStruct


// ************************************
// FRAME DATA STRUCT and related stuff
// frameDataStruct declaration:
// frameDataStruct is an structure which contains all the data needed
// for every sprite, or more exactly, for every frame... General game
// objects which have a graphical representation will have a vector
// of this structure, where every node is a frame.
// cX and cY are relative to the frame coordinates... that is, in the
// surface they start from x and y.
// Any other position markers are relative to the handler coordinates.
struct frameDataStruct {
	// Frame data
	// name of the frame, null for none
	char* frameName;
	// surface where the frame is stored
	ddSurfaceClass* surface;
	// coordinates and size of the image in the surface
	int x, y, w, h;
	// collision coordinates and size of this frame, usually equal
	// to 0, 0, w, h (relative to the frame, not to the surface)
	int cX, cY, cW, cH;
	// coordinates of the handler point in this frame (that is, the
	// point at which the object coordinates correspond). Again, it's
	// relative to the frame coordinates, not to the surface's.
	int handlerX, handlerY;

	// other position markers, relative to the handler coordinates
	int spawnBulletsX, spawnBulletsY;// position from where to spawn bullets
	int adherenceX, adherenceY;// position where to adhere this frame to other objects (as with slave objects, for example)

	// Frames sequence data (when going automatically)
	// That's the next frame to be executed...
	int autoNextFrame;
	// Amount of frame executions this frame has to keep on screen
	// before automatically going to the next frame... If it's zero,
	// it's never supposed to change anymore.
	DWORD autoShowVirtualFrames;
	// name of the event this frame should execute at its end,
	// null for none
	char* nextEventName;

	// name of the frames group this frame belongs to
	char* groupName;
};// frameDataStruct

// ************************************
// FRAMELIST DATA STRUCT and related stuff
// frameListDataStruct declaration:
// frameListDataStruct is an structure which points to a list of
// frames, and gives such a list a name and some other attributes
struct frameListDataStruct {
	// a pointer to the list of frames itself
	frameDataStruct* frameList;
	// a name for such frame list
	char* listName;
	// an integer value telling the size of the list of frames,
	// excluding the last 'null' node when it's present
	int size;
};// frameListDataStruct

// ************************************
// GENERIC UNIT STRUCT and related stuff
// This is the class which manages and stores the current state of a
// generic unit (to be inherited by specific unit structs).
struct genericUnitStruct{
	DWORD id;// unique identifier for this unit
	long double x, y;// position of this unit
	int unitType;// value: FILE_GAMEDATA_PLAYER_*, FILE_GAMEDATA_ENEMY_*, FILE_GAMEDATA_BULLET_*, etc
	BYTE objectType;// what object type is, FILE_GAMEDATA_GAMEOBJECT_*

	// speed related
	double speedX, speedY;// amount of pixels per tick (virtual frame)
	double minSpeedX, maxSpeedX;// as speedX, but working as limits... (absolute value)
	double minSpeedY, maxSpeedY;// as speedY, but working as limits... (absolute value)
	double linearSpeed;// linear speed to be used always, by the unit (per virtual frame)

	// frame stuff
	int currentFrame;// frame this object is currently in
	DWORD vFramesPassed;// amount of virtual frames passed for this frame

	// Relative position stuff (relative to camera)
	bool isPosRelative;// are xFromCamera and yFromCamera used for relative position?
	long double xFromCamera, yFromCamera;// relative position to camera

	// Data telling the way this unit behaves
	double energyToDrop;// energy this unit has to drop when touching any other unit
	bool decrementableEnergy;// tells whether the unit energy can become smaller or not

	// Data telling the current unit state
	bool nowDrawable;// tells if the unit should be drew or not (only affects the drawing of a unit, not collision neither anything else), that is, a visibility state
	bool nowActive;// tells if the unit is active and playing right now
	bool trembling;// tells if the unit is trembling right now

	// Slave information...
	gameObjectPointerStruct slaveOf;// this tells what unit this unit is a slave of, it's empty if this unit isn't a slave of any other
	gameObjectPointerStruct slaveUnit;// this tells what unit this unit has as a slave, it's empty if no unit is a slave of this one

	// Data for unintelligent unit movement
	bool goingToPoint;// should the ai keep following that point now?
	long double goingToPointX, goingToPointY;// point the unit is going to, if any
};// genericUnitStruct

// ************************************
// GENERIC UNIT TYPE DATA STRUCT and related stuff
// This is the class which stores the data used in all the units
// of the same type.
struct genericUnitTypeDataStruct{
	int unitType;// value: FILE_GAMEDATA_type_*

	// Frames stuff...
	char* firstFrameName;// name of the first frame, for calculating it
	int firstFrame;// frame this type starts in

	// Frame list stuff...
	char* frameListName;// the name of the vector which contains the frames info
	frameDataStruct* frames;// here goes the vector which contains the frames

	// About states and behaviour
	int pixelsToTrembleWhenUntouchable;// how much it has to tremble when in untouchable mode, it has to be an even value
};// genericUnitTypeDataStruct

// ************************************
// PLAYER TYPE DATA STRUCT and related stuff
// Player types declaration:
// This enemy type means the structure does not contain valid enemy data.
#define FILE_GAMEDATA_PLAYER_EMPTY 0
// This is a newer player, called Juan Avatar, definitive graphic for
// techDemo3
#define FILE_GAMEDATA_PLAYER_JAVATAR 4


// ************************************
// PLAYER UNIT STRUCT and related stuff
// This is the class which manages and stores the current state of an
// player unit.
struct playerUnitStruct:genericUnitStruct{
	// player input...
	bool userControlled;// can the user control this?
	FILE_DIWRAP_SINGLEKEY_TYPE keyUp;
	FILE_DIWRAP_SINGLEKEY_TYPE keyDown;
	FILE_DIWRAP_SINGLEKEY_TYPE keyLeft;
	FILE_DIWRAP_SINGLEKEY_TYPE keyRight;
	FILE_DIWRAP_SINGLEKEY_TYPE keyShoot;
	bool playerUseAllKeyboard;// if it's set, we can use special key codes for making many keys do for the same function

	// Data telling the way this unit behaves
	int untouchableTimeMax;// maximum value for untouchableTimeLeft
	int unarmedTimeMax;// maximum value for unarmedTimeLeft
	double scoreToGivePerHit;// how much score it gives per hit
	bool canBeHitByPlayers;// tells whether players can hurt it or not

	// Data telling the current unit state
	bool nowEnteringGame;// tells whether the player is now entering the game area or not
	int untouchableTimeLeft;// for how much more time (in vframes) the unit will be untouchable
	int unarmedTimeLeft;// for how much more time (in vframes) the unit won't be able to shoot
	double energyLeft;// energy left for this unit... 0 for dead
	double score;// current score this unit has...
};// playerUnitStruct

// ************************************
// ENEMY TYPE DATA STRUCT and related stuff
// Enemy types declaration:
// This enemy type means the structure does not contain valid enemy data.
#define FILE_GAMEDATA_ENEMY_EMPTY 0
// This is the white friendly sheep
#define FILE_GAMEDATA_ENEMY_WHITESHEEP 3
// This is the black friendly sheep
#define FILE_GAMEDATA_ENEMY_BLACKSHEEP 4
// This is the friendly human head
#define FILE_GAMEDATA_ENEMY_HUMANHEAD 5
// This is the enemy that carries sheeps
#define FILE_GAMEDATA_ENEMY_SHEEPCARRIER 6
// This is the enemy that carries human heads
#define FILE_GAMEDATA_ENEMY_HEADCARRIER 7


// ************************************
// ENEMY UNIT STRUCT and related stuff
// This is the class which manages and stores the current state of an
// enemy unit.
struct enemyUnitStruct:genericUnitStruct{
	// Data telling the way this unit behaves
	int untouchableTimeMax;// maximum value for untouchableTimeLeft
	int unarmedTimeMax;// maximum value for unarmedTimeLeft
	double scoreToGivePerHit;// how much score it gives per hit
	int timeOutsideTheScreenMax;// maximum value for timeOutsideTheScreenLeft

	// Data for collision checking
	bool canBeHitByPlayers;// tells whether players can collide with it or not
	bool canBeHitByBullets;// tells whether bullets can collide with it or not

	// Data telling the current unit state
	double energyLeft;// energy left for this unit... 0 for dead
	int untouchableTimeLeft;// for how much more time (in vframes) the unit will be untouchable
	int unarmedTimeLeft;// for how much more time (in vframes) the unit won't be able to shoot
	int timeOutsideTheScreenLeft;// for how much more time (in vframes) the unit will be able to be outside the screen without being removed
	bool justGoingOut;// tells whether the unit is just going out to the screen, or it's been there from some time (usually affects the state of the collision variables)

	// Specific data for the AI, that might vary from type to type
	union{
		// flockerData
		// Specific AI for flocker objects (the carriers)
		struct {
			// For having followers and pursued units
			gameObjectPointerStruct unitToFollow;// enemy unit this unit will have to follow
		}flockerData;
		// sheepData
		// Specific for AI in falling objects (sheeps and other animals)
		struct {
			// About current falling state
			bool nowFalling;// tells whether the unit is currently falling or not
			bool nowJumpingForHigh;// tells whether the unit is currently jumping up or not

			// About the way a unit should jump
			int jumpHDistance;// minimum horizontal distance in pixels, for the jumps
			int maxVarianceHD;// maximum variance in pixels for the horizontal jumping distance.. the maximum jump will be jumpHDistance+maxVarianceHD
			int jumpVDistance;// vertical distance in pixels, for the jumps

			// About score when freed, with and without a head
			int sheepScoreToGiveWhenFreed;
			int sheepScoreToGiveWhenFreedWithAHead;

			// Some information about the way it falls
			double sheepSpeedWhenFalling;// linear speed the unit has when it falls
		}sheepData;
		// headData
		// Specific for AI in human head objects
		struct {
			// About current falling state
			bool fallingHead;// tells whether the unit is currently falling or not

			// About score when freed
			int headScoreToGiveWhenFreed;

			// Some information about the way it falls
			double headSpeedWhenFalling;// linear speed the unit has when it falls
		}headData;
	}aiSpecific;// union
};// enemyUnitStruct

// ************************************
// BULLET TYPE DATA STRUCT and related stuff
// Bullet types declaration:
// This bullet type means the structure does not contain valid bullet data.
#define FILE_GAMEDATA_BULLET_EMPTY 0
// This is a flame bullet from the player
#define FILE_GAMEDATA_BULLET_PLAYERFLAME 3


// ************************************
// BULLET UNIT STRUCT and related stuff
// This is the class which manages and stores the current state of a
// bullet unit.
struct bulletUnitStruct:genericUnitStruct{
	// Variables which tell what kind of units a given bullet may hit
	bool canCollideWithPlayers;// can this bullet unit collide with players?
	bool canCollideWithEnemies;// can this bullet unit collide with enemies?
	bool canCollideWithBullets;// can this bullet unit collide with other bullets?
	bool canCollideWithTheOwner;// can this bullet unit collide with its owner?

	// Data telling the way this unit behaves
	int timeOutsideTheScreenMax;// maximum value for timeOutsideTheScreenLeft

	// Data telling the current unit state
	gameObjectPointerStruct owner;// this tells which unit dropped the bullet
	int timeOutsideTheScreenLeft;// for how much more time (in vframes) the unit will be able to be outside the screen without being removed
	bool nowExploding;// tells whether this bullet is exploding or not
};// bulletUnitStruct



// ******************************************
// WAVEGEN TYPE DATA STRUCT and related stuff
// Wave generators types declaration:
// This wave gen type means the structure does not contain valid wg data.
#define FILE_GAMEDATA_WAVEGEN_EMPTY 0
// This wave gen type is one that generates aliens that carry white sheeps.
#define FILE_GAMEDATA_WAVEGEN_WHITESHEEPS 1
// This wave gen type is one that generates aliens that carry black sheeps.
#define FILE_GAMEDATA_WAVEGEN_BLACKSHEEPS 2
// This wave gen type is one that generates aliens that carry human heads.
#define FILE_GAMEDATA_WAVEGEN_HUMANHEADS 3
// This wave gen type is one that does not generate anything, but is the entrance for walking sheeps.
#define FILE_GAMEDATA_WAVEGEN_SHEEPSOUT 4
// This wave gen type is one that does not generate anything, and has no defined utility (actually to be used for foreground sprites that do nothing).
#define FILE_GAMEDATA_WAVEGEN_UNDEFINED 10

// *************************************
// WAVEGEN UNIT STRUCT and related stuff
// This is the class which manages and stores the current state of a
// wave generator unit.
struct waveGenUnitStruct:genericUnitStruct{
	// state of a given wave generator
	bool generatingNow;// tells whether the wave generator is currently generating units

	// Enemy wave generation related stuff
	double distanceBetweenWaves;// what's the distance which separates an enemy wave form another
	double virtualEnemyWavesSpeedX;// what's the speed of the virtual enemy wave object when it's outside the screen
	double nextEnemyWaveX;// this tells where that virtual enemy wave object is.
};// waveGenUnitStruct



#endif// FILE_GAMEDATA_INCLUDED
