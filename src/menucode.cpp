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

#include "menucode.h"// for accesing to the methods related to the program zone corresponding to the game menues
#include "gameglobals.h"// for using the game objects
/*#include "generalmaths.h"// for mathematical functions*/
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include "ddwrap.h"
/*#include "diwrap.h"
#include "audiowrap.h"*/
#include "console.h"
#include "con-logger.h"
#include "globals.h"// for using the program objects
#include "mainloop.h"// for accessing to the main loop object
#include "programinfo.h"// for general info about the program itself
/*#include "soundman.h"// for using the functions related to sound management*/
#include <string.h>

// VARIABLES AND FUNCTIONS
// Useful variables for the obscuring stuff
static double screenLightness;// 0 to 255, lightness factor
static double obscuringSpeed;// how much to obscure per frame
bool nowObscuring;// whether we should be obscuring the screen now, or lighting it up

// Useful for knowing where the mouse is
static int mouseX, mouseY;

// this variable should only be used during the execution of
// MenuLoopCode::drawFrame
static ddSurfaceClass* screenBuffer;



// *****************************
// LUA INTERFACING FUNCTIONS   *
// *****************************

// luaResetHighScores *******
// A function that resets the high scores list every time it's called
int luaResetHighScores(lua_State* lstate)
{
	int paramNum=lua_gettop(lstate);

	// log it
	logger.logLine("High scores list has been reset");

	return 0;// there are no return values
}// luaResetHighScores

// luaSetSoundEnabled *******
// A function to set the soundEnabled field to true or false,
// as needed
int luaSetSoundEnabled(lua_State* lstate)
{
	return 0;// there are no return values
}// luaSetSoundEnabled

// luaStopAndPlaySoundResource *******
// a function that stop and plays a given sound resource
int luaStopAndPlaySoundResource(lua_State* lstate)
{
/*	int paramNum=lua_gettop(lstate);
	char* soundName;

	// we get the parameters out of the stack
	soundName=(char*)(lua_tostring(lstate, 1));

	// now just execute it
	stopAndPlaySoundResource(soundName);
*/
	return 0;// there are no return values
}// luaStopAndPlaySoundResource

// luaPlaySoundResource *******
// a function that plays a given sound resource
int luaPlaySoundResource(lua_State* lstate)
{
/*	int paramNum=lua_gettop(lstate);
	char* soundName;
	bool looping;// should the sound loop?

	// we get the parameters out of the stack
	soundName=(char*)(lua_tostring(lstate, 1));
	looping=(lua_toboolean(lstate, 2)==1?true:false);

	// now just execute it
	playSoundResource(soundName, looping);
*/
	return 0;// there are no return values
}// luaPlaySoundResource

// luaStopSoundResource *******
// a function that stops playing a given sound resource
int luaStopSoundResource(lua_State* lstate)
{
/*	int paramNum=lua_gettop(lstate);
	char* soundName;

	// we get the parameter out of the stack
	soundName=(char*)(lua_tostring(lstate, 1));

	// now just stop it
	stopSoundResource(soundName);
*/
	return 0;// there are no return values
}// luaStopSoundResource

// luaUpdateHighScoresList *******
// a function that updates or constructs a high scores list in the
// lua side
int luaUpdateHighScoresList(lua_State* lstate)
{
	int paramNum=lua_gettop(lstate);
	int i;

	// let's start pushing an emtpy table to the stack
	lua_newtable(lstate);// we call it gt

	// we traverse the high scores list
	for (i=1; i<10; i++)// 2010 TODO
	{
		// i
		lua_pushnumber(lstate, i);
		// gt[i].name=name, gt[i].score=score
		lua_newtable(lstate);// (this will be gt[i])
		lua_pushstring(lstate, "name");
		lua_pushstring(lstate, "Anonymous");
		lua_rawset(lstate, -3);
		lua_pushstring(lstate, "score");
		lua_pushnumber(lstate, (10-i)*30);
		lua_rawset(lstate, -3);
		// save it in the gt object too
		lua_rawset(lstate, -3);
	}// for// 2010 TODO

	// finally, we do highScoresList=gt
	lua_setglobal(lstate, "highScoresList");

	return 0;// there are no return values
}// luaUpdateHighScoresList

// luaSetCurrentLanguage *******
// a function used for changing the current mouse position
int luaSetCurrentLanguage(lua_State* lstate)
{
	char* langName;

	int paramNum=lua_gettop(lstate);

	// get the new language name
	langName=(char*)(lua_tostring(lstate, 1));

	if (strcmp(langName, "spanish")==0) {
		currentLanguage=language_spanish;
	}else if (strcmp(langName, "english")==0) {
		currentLanguage=language_english;
	}else{
		// nothing to do if lang name is not specified correctly
	}

	// set the language in lua
	scriptMan.setCurrentLanguageInLua(currentLanguage);

	// log it
	logger.logLine("Language been switched to %s", true, false, true,
		langName);

	return 0;// there are no return values
}// luaSetCurrentLanguage

// luaMenuSetMouseXY *******
// a function used for changing the current mouse position
int luaMenuSetMouseXY(lua_State* lstate)
{
	int paramNum=lua_gettop(lstate);

	// get the new mouse coordinates
	mouseX=(int)(lua_tonumber(lstate, 1));
	mouseY=(int)(lua_tonumber(lstate, 2));
	
	return 0;// there are no return values
}// luaMenuSetMouseXY

// luaSwitchToMenu *******
// a function used for switching the current menu
int luaSwitchToMenu(lua_State* lstate)
{
	char* menuName;
	int errCode;

	int paramNum=lua_gettop(lstate);

	// get the menu name...
	menuName=(char*)(lua_tostring(lstate, 1));
	
	// execute the lua code that manages menu switching
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "cpp_switchToMenu");
	lua_pushstring(lstate, menuName);
	errCode=lua_pcall(lstate, 1, 0, -3);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over cpp_switchToMenu");
		return false;
	}

	// now check for the actions related to the different menus...
	if (strcmp(menuName, "startGame_pseudoMenu")==0) {
		// make it go start the game
		gameWorld.playersToStart=1;// let's start with only one player
		// go to the game itself
		if (!switchToProgramZone(pgzon_actualGameRunning,
			gmzon_classicZone, 0, true)) return false;
	} else if (strcmp(menuName, "exitingMenu")==0) {
		// start obscuring all down
		nowObscuring=true;
	}

	return 0;// there are no return values
}// luaSwitchToMenu

// luaMenuBltImage *******
// a function used for blting from a Lua script to the screen
int luaMenuBltImage(lua_State* lstate)
{
	ddSurfaceClass* surface;

	int paramNum=lua_gettop(lstate);

	int xD, yD, xS, yS, wS, hS;
	bool transparent;
	char* srfName;

	// check which version of the function is being called
	switch(paramNum) {
	case 4:
		// short version
		// params, in the same order they should be
		// passed to the program
		srfName=(char*)(lua_tostring(lstate, 1));
		xD=int(lua_tonumber(lstate, 2));
		yD=int(lua_tonumber(lstate, 3));
		transparent=lua_toboolean(lstate, 4)==1?true:false;

		// look for the surface...
		surface=getSurfaceByResourceName(srfName);

		// go draw it...
		screenBuffer->bltFrom(
			*surface, xD, yD, true, transparent);

		break;
	case 8:
		// long version
		// params, in the same order they should be
		// passed to the program
		srfName=(char*)(lua_tostring(lstate, 1));
		xD=int(lua_tonumber(lstate, 2));
		yD=int(lua_tonumber(lstate, 3));
		xS=int(lua_tonumber(lstate, 4));
		yS=int(lua_tonumber(lstate, 5));
		wS=int(lua_tonumber(lstate, 6));
		hS=int(lua_tonumber(lstate, 7));
		transparent=lua_toboolean(lstate, 8)==1?true:false;

		// look for the surface...
		surface=getSurfaceByResourceName(srfName);

		// go draw it...
		screenBuffer->bltFrom(
			*surface, xD, yD, xS, yS, wS, hS, true, transparent);

		break;
	}

	return 0;// there are no return values
}// luaMenuBltImage

// luaMenuDrawText *******
// a function used for drawing a text from a Lua script to the screen
int luaMenuDrawText(lua_State* lstate)
{
	// useful variables...
	char* fontName;
	char* text;
	fixedFontClass* font;
	int x, y, boxWidth, boxHeight;
	alignEnum h_align;
	valignEnum v_align;
	char* alignvTmp;
	char* alignhTmp;


	// get the parameters, in the order they should have been sent
	fontName=(char*)(lua_tostring(lstate, 1));
	text=(char*)(lua_tostring(lstate, 2));
	x=int(lua_tonumber(lstate, 3));
	y=int(lua_tonumber(lstate, 4));
	boxWidth=int(lua_tonumber(lstate, 5));
	boxHeight=int(lua_tonumber(lstate, 6));
	alignhTmp=(char*)(lua_tostring(lstate, 7));
	alignvTmp=(char*)(lua_tostring(lstate, 8));

	// select the asked font
	if (strcmp(fontName, "ediaFont")==0) {
		font=&fntEdiaFont;
	}else if (strcmp(fontName, "ediaBigFont")==0) {
		font=&fntEdiaBigFont;
	}else if (strcmp(fontName, "dahFont")==0) {
		font=&fntDahFont;
	}else {
		// default
		font=&fntDahFont;
	}

	// select the asked h align
	if (strcmp(alignhTmp, "left")==0) {
		h_align=FILE_DDWRAP_ALIGN_LEFT;
	}else if (strcmp(alignhTmp, "right")==0) {
		h_align=FILE_DDWRAP_ALIGN_RIGHT;
	}else if (strcmp(alignhTmp, "center")==0) {
		h_align=FILE_DDWRAP_ALIGN_CENTER;
	}else {
		// default
		h_align=FILE_DDWRAP_ALIGN_LEFT;
	}

	// select the asked v align
	if (strcmp(alignvTmp, "top")==0) {
		v_align=FILE_DDWRAP_VALIGN_TOP;
	}else if (strcmp(alignvTmp, "bottom")==0) {
		v_align=FILE_DDWRAP_VALIGN_BOTTOM;
	}else if (strcmp(alignvTmp, "center")==0) {
		v_align=FILE_DDWRAP_VALIGN_CENTER;
	}else {
		// default
		v_align=FILE_DDWRAP_VALIGN_TOP;
	}

	// draw the text
	font->drawMultilineText(
		*screenBuffer, text, strlen(text), x, y,
		boxWidth, boxHeight, true, h_align, v_align);

	return 0;// there are no return values
}// luaMenuDrawText



// *****************************
// MAIN FUNCTIONS
// Addressed as MenuLoopCode::*
// *****************************


// THE MAIN CODE
// drawFrame *************
// Draw the current frame on screen.
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool MenuLoopCode::drawFrame()
{
	lua_State* lstate;
	int errCode;

	// let's see whether we need a buffer or not
	if (screenLightness==255) {
		// we don't need that buffer
		screenBuffer=&DD;
	}else{
		// we need the buffer
		screenBuffer=getSurfaceByResourceName("srfScreenSizedBuffer");
	}

	// REVISE LATER BEGIN
	// first go and black all down
	screenBuffer->fillAllDword(0);
	// REVISE LATER END

	// DRAW THE SCREEN FOR THE MAIN MENU
	// execute the lua code that manages menu drawing
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "drawFrame");
	errCode=lua_pcall(lstate, 0, 0, -2);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over drawFrame");
		return false;
	}

	// let's see whether we used the buffer or not, and manage the
	// light of the screen
	if (screenLightness!=255) {
		// we used it, so let's blt it to the screen
		DD.obscuredBltFrom(*screenBuffer,// surface
			0, 0,// coordinates
			(unsigned char)(screenLightness),// lightness
			false);// transparent?
	}

	return true;// all ok
}// drawFrame

// executeFrame ***********
// Execute a frame, without drawing it on screen. It's what I call
// "virtual frames".
// Only return false for non-recover errors.
bool MenuLoopCode::executeFrame(DWORD frameNumber)
{
	lua_State* lstate;
	int errCode;

	bool keyPreviousPressed=false;
	bool keyNextPressed=false;
	bool keyClickPressed=false;
	bool mouseClickPressed=false;

	// CHECK FOR KEYS
	// check if it's time to change the currently selected menu option
	// Go up...
	if ((KEYDOWN(keyboard.keys, SDLK_UP) || KEYDOWN(keyboard.keys, SDLK_LEFT)) && !showConsole) {

		keyPreviousPressed=true;
	}
	// Go down...
	if ((KEYDOWN(keyboard.keys, SDLK_DOWN) || KEYDOWN(keyboard.keys, SDLK_RIGHT)) && !showConsole) {

		keyNextPressed=true;
	}
/*	// Check for selected buttons or mouse buttons
	if ((KEYDOWN(keyboard.keys, DIK_SPACE) ||// space key
		KEYDOWN(keyboard.keys, DIK_LCONTROL) ||// left control key
		KEYDOWN(keyboard.keys, DIK_ENTER))// return key
		&& !showConsole) {

		keyClickPressed=true;
	}*/
	if (MOUSEDOWN(mouse.mouseData, MOUSE_BUTTON_LEFT)) {

		// mouse click
		mouseClickPressed=true;
	}

	// Check it has moved at all
	if (MOUSEX(mouse.mouseData)!=0 || MOUSEY(mouse.mouseData)) {
		// Get the current mouse position
		mouseX+=MOUSEX(mouse.mouseData);
		if (mouseX<0) mouseX=0;
		if (mouseX>=DD.getWidth()-1) mouseX=DD.getWidth()-1;
		mouseY+=MOUSEY(mouse.mouseData);
		if (mouseY<0) mouseY=0;
		if (mouseY>=DD.getHeight()-1) mouseY=DD.getHeight()-1;
	}

	// LUA EXECUTE FRAME
	// execute the lua code that manages frame execution
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "executeFrame");
	lua_pushnumber(lstate, frameNumber);
	lua_pushnumber(lstate, mouseX);
	lua_pushnumber(lstate, mouseY);
	lua_pushboolean(lstate, mouseClickPressed);
	lua_pushboolean(lstate, keyPreviousPressed);
	lua_pushboolean(lstate, keyNextPressed);
	lua_pushboolean(lstate, keyClickPressed);
	errCode=lua_pcall(lstate, 7, 0, -9);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over executeFrame");
		return false;
	}

	// THE ALPHA BLENDING THING
	if (nowObscuring) {
		// go obscure the screen
		screenLightness-=obscuringSpeed;
		if (screenLightness<=0) {
			// finished lighting down? should execute some
			// obscuring event... for now we just go away
			screenLightness=0;
			// first obscure all the buffers of the screen
			if (DD.isReady() && DD.isVideoModeReady()) {
				int i;

				for (i=0; i<DD.getBackBuffersCount(); i++) {
					DD.fillAllDword(0); DD.flip();
				}
			}
			// bye bye
			if (!globalExit())
				return false;
		}
	}else{
		// anywhere else from the exiting menu, light screen up
		screenLightness+=obscuringSpeed;
		if (screenLightness>=255) screenLightness=255;
	}

	return true;// all ok
}// executeFrame

// beforeLoop **********
// This is the code to be executed every time a program zone linked to
// these functions is switched to.
bool MenuLoopCode::beforeLoop()
{
	lua_State* lstate;
	int errCode;
	char* menuToSwitch;

	// before starting, stop all the playing sounds (belonging to other
/*	// program zones)
	if (!stopAllSoundResources()) return false;
*/
	// calculate the obscuring stuff
	// light all up in 1 second
	obscuringSpeed=(255/double(desiredFramesPerSecond))/1;
	// light the screen slowly only when coming from the intro
	if (previousProgramZone==pgzon_gameIntro) {
		// go slowly
		screenLightness=0;
	}else{
		// make it be already lightened
		screenLightness=255;
	}

	// give the mouse cursor an initial position
	mouseX=0;
	mouseY=0;

	// EXECUTE THE LUA BEFORE LOOP
	// execute the lua code that manages the before loop
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "beforeLoop");
	errCode=lua_pcall(lstate, 0, 0, -2);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over beforeLoop");
		return false;
	}

	// Time to go to the main menu
	menuToSwitch="mainMenu";

	// EXECUTE THE LUA SWITCH TO MENU
	// execute the lua code that manages the menu switching
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "switchToMenu");
	lua_pushstring(lstate, menuToSwitch);
	errCode=lua_pcall(lstate, 1, 0, -3);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over switchToMenu");
		return false;
	}


	return true;// all ok
}// beforeLoop


// afterLoop ***********
// This is the code to be executed every time it's time to switch to
// another program zone, being a program zone currently linked to
// these functions.
bool MenuLoopCode::afterLoop()
{
	lua_State* lstate;
	int errCode;

	// EXECUTE THE LUA AFTER LOOP
	// execute the lua code that manages the after loop
	lstate=scriptMan.getLuaObject();
	lua_getglobal(lstate, "errorManager");
	lua_getglobal(lstate, "afterLoop");
	errCode=lua_pcall(lstate, 0, 0, -2);
	lua_pop(lstate, 1);// take errorManager off the stack
	if (errCode!=0) {
		lua_pop(lstate,1);// there is an extra value there...
		logger.logLine("Error calling pcall over afterLoop");
		return false;
	}

	return true;// all ok
}// afterLoop


// *****************
// EXTRA FUNCTIONS *
// *****************

// executeHighScoreTyping ************
// This function manages the process of receiving a key hit and making
// it go to the name of the high scores list currently under edition.
bool executeHighScoreTyping(char key)
{
	return true;
}// executeHighScoreTyping


