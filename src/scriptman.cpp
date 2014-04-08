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

#include "scriptman.h"// used for accessing to the script manager object
#include "globals.h"// for using the program objects
#include "fileString.h"// for accessing to strings, memory, files and windows resources in an only simple way
#include "menucode.h"// for accesing to the methods related to the program zone corresponding to the game menues
/*#include "gameglobals.h"// for using the game objects
#include "programinfo.h"// for general info about the program itself
#include "timewrap.h"// for using the time wrapper class
*/
// header files for using lua
extern "C"{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}// extern

#define lua_dostring luaL_dostring // change in Lua 5.1
#define luaL_dobuffer(l, buff, size, name) (luaL_loadbuffer(l, buff, size, name) || lua_pcall(l, 0, LUA_MULTRET, 0))
#define lua_dobuffer luaL_dobuffer


// LUA INTERNAL FUNCTIONS

// luaLogToFile *******
// a function used for logging from a Lua script to the logs file
static int luaLogToFile(lua_State* lstate)
{
	int i;
	int n=lua_gettop(lstate);

	for (i=1; i<=n; i++) {
		logger.logLine("[Lua msg] %s", true, true, false,
			lua_tostring(lstate, i));
	}

	return 0;// there are no return values
}// luaLogToFile

// SCRIPT MANAGER METHODS
// ======================

// INIT / DEINIT METHODS

// init *********
// method used for the initialization of this object
bool scriptManagerClass::init()
{
	fileStringClass luadata;
	char* scriptName;
	timeWrapperClass timer;

	// check for previous initialization
	if (ready) return false;// error

	// log it
	logger.logLine("Initializing scripting manager...", true, false);

	// open lua
	lstate=luaL_newstate();
	if (lstate==NULL) {
		logger.logLine("Failed opening Lua");
		return false;
	}
	// open the extra libraries
	luaopen_base(lstate);
	luaopen_string(lstate);
	luaopen_table(lstate);
	luaopen_math(lstate);

	// register our logging function
	lua_register(lstate, "logToFile", luaLogToFile);
	// register our drawing images function for the menues
	lua_register(lstate, "menuBltImage", luaMenuBltImage );
	// register our drawing texts function for the menues
	lua_register(lstate, "menuDrawText", luaMenuDrawText);
	// register our function that switches to other menues
	lua_register(lstate, "switchToMenu", luaSwitchToMenu);
	// register our function that moves the mouse
	lua_register(lstate, "menuSetMouseXY", luaMenuSetMouseXY);
	// register our function that switches the current language
	lua_register(lstate, "setCurrentLanguage", luaSetCurrentLanguage);
	// register our function that updates the high scores list
	lua_register(lstate, "updateHighScoresList",
		luaUpdateHighScoresList);
	// register our function that changes the sound enabled field
	lua_register(lstate, "setSoundEnabled", luaSetSoundEnabled);
	// register our function that plays a given sound
	lua_register(lstate, "playSoundResource", luaPlaySoundResource);
	// register our function that stops playing a given sound
	lua_register(lstate, "stopSoundResource", luaStopSoundResource);
	// register our function that stops and plays a given sound
	lua_register(lstate, "stopAndPlaySoundResource",
		luaStopAndPlaySoundResource);
	// register our function that resets the high scores
	lua_register(lstate, "resetHighScores",
		luaResetHighScores);

	// save the desiredFramesPerSecond variable in Lua
	lua_pushnumber(lstate, desiredFramesPerSecond);
	lua_setglobal(lstate, "desiredFramesPerSecond");

	// set the current language in lua
	this->setCurrentLanguageInLua(currentLanguage);

	// Execute SCRIPTFILES
	// open the script data from a text file...
	if (!luadata.openFromFile("data/scripts/scriptfiles.lua")) {

		luadata.close();
		if (!luadata.openFromFile(
			"data/scriptsdata/scriptfiles.lua")) {

			logger.logLine("Failed opening scriptfiles.lua");
			return false;
		}
	}
	// load the lua data to lua
	if (lua_dobuffer(
		lstate,// lua state
		(char*)luadata.getDataPointer(),
		luadata.getDataSize(),
		"redia load list script")!=0)
	{
		logger.logLine("Failed executing pre load buffer");
		return false;
	}
	// ready
	luadata.close();

	// Now, for every value in the scriptFilesList, execute a lua
	// script
	lua_getglobal(lstate, "scriptFilesList");
	lua_pushnil(lstate);
	while (lua_next(lstate, -2)!=0) {
		// execute the script on the list
		scriptName=(char*)(lua_tostring(lstate, -1));
		if (!luadata.openFromFile(scriptName)) {
			logger.logLine("Failed opening %s for lua",
				true, true, true, scriptName);
			return false;
		}
		// load the lua data to lua
		if (lua_dobuffer(
			lstate,// lua state
			(char*)luadata.getDataPointer(),
			luadata.getDataSize(),
			scriptName)!=0)
		{
			logger.logLine("Failed executing the %s lua script",
				true, true, true, scriptName);
			return false;
		}
		// ready
		luadata.close();

		// set up for the next step of the loop
		lua_pop(lstate, 1);
	}
	// remove the table and the key value as well
	lua_pop(lstate, 2);


	// time the use of lua
	if (!timer.init()) return false;

	// execute the initialize function before starting
	if (lua_dostring(lstate, "startScript()")!=0) {
		logger.logLine("Failed executing the initialize script");
		return false;
	}

	// ready, log the time it took to execute lua
#ifdef PROGMODE_DEVELOPER_YES
	logger.logLine("Time in setting up lua: %f",
		true, true, true, timer.getTime()/1000.0);
#endif // PROGMODE_DEVELOPER_YES
	timer.end();

	// set flags
	ready=true;

	// all ok
	return true;
}// init

// close ********
// method used for the deinitialization of this object
bool scriptManagerClass::close()
{
	// check for previous initialization
	if (!ready) return false;// error

	// close lua
	lua_close(lstate);

	// set flags
	ready=false;

	return true;// all ok
}// close

// GENERAL PURPOSE INTERFACES
// getIntVariable ********
// method used for obtaining the value attached to a given integer
// variable, given the variable name.
// errCode is the pointer to a bool variable to set with true if
// all went ok, and to set with false if there was any errors. it
// might be NULL (default value) for avoiding such work
int scriptManagerClass::getIntVariable(char* varName, bool* errCode)
{
	bool success=true;// all ok by default
	int value;

	// check for previous initialization
	if (!ready || varName==NULL) {
		// error
		success=false;
		if (errCode!=NULL) *errCode=success;
		return 0;
	}

	// get the asked value
	lua_getglobal(lstate, varName);
	value=int(lua_tonumber(lstate, -1));
	lua_pop(lstate, 1);// discard

	// all ok
	if (errCode!=NULL) *errCode=success;
	return value;
}// getIntVariable

// getDoubleVariable ********
// method used for obtaining the value attached to a given double
// variable, given the variable name
// errCode is the pointer to a bool variable to set with true if
// all went ok, and to set with false if there was any errors. it
// might be NULL (default value) for avoiding such work
double scriptManagerClass::getDoubleVariable(char* varName, bool* errCode)
{
	bool success=true;// all ok by default
	double value;

	// check for previous initialization
	if (!ready || varName==NULL) {
		// error
		success=false;
		if (errCode!=NULL) *errCode=success;
		return 0;
	}

	// get the asked value
	lua_getglobal(lstate, varName);
	value=lua_tonumber(lstate, -1);
	lua_pop(lstate, 1);// discard

	// all ok
	if (errCode!=NULL) *errCode=success;
	return value;
}// getDoubleVariable

// getStringValue ***********
// method used for obtaining the value attached to a given string
// variable, given the variable name
// errCode is the pointer to a bool variable to set with true if
// all went ok, and to set with false if there was any errors. it
// might be NULL (default value) for avoiding such work
char* scriptManagerClass::getStringValue(char* varName, bool* errCode)
{
	bool success=true;// all ok by default
	char* value;

	// check for previous initialization
	if (!ready || varName==NULL) {
		// error
		success=false;
		if (errCode!=NULL) *errCode=success;
		return 0;
	}

	// get the asked value
	lua_getglobal(lstate, varName);
	value=(char*)(lua_tostring(lstate, -1));
	lua_pop(lstate, 1);// discard

	// all ok
	if (errCode!=NULL) *errCode=success;
	return value;
}// getStringValue

// doString ********
// this method executes a given string as a lua command, and returns
// the typical error code
bool scriptManagerClass::doString(char* string)
{
	// check for previous initialization
	if (!ready) return false;// error

	if (lua_dostring(lstate, string)!=0) {
		return false;// error
	}

	return true;// all ok
}// doString

// setCurrentLanguageInLua **********
// This method sets an environment variable telling which language
// is currently active, only in Lua
bool scriptManagerClass::setCurrentLanguageInLua(languageEnum lang)
{
	// check for previous initialization
	if (!ready) return false;// error

	// change the currentLanguage variable in Lua
	if (lang==language_spanish) {
		lua_pushstring(lstate, "spanish");
	}else{
		lua_pushstring(lstate, "english");
	}
	lua_setglobal(lstate, "currentLanguage");

	return true;// all ok
}// setCurrentLanguageInLua

// setSoundEnabledInLua **********
// This method sets an environment variable telling whether the
// sound is currently enabled or not
bool scriptManagerClass::setSoundEnabledInLua(
	bool nowEnabled,// whether the user selected enabled or disabled
	bool soundActive)// whether it's currently active in the program (would need restarting the program to change)
{
	// check for previous initialization
	if (!ready) return false;// error

	// change the currentLanguage variable in Lua
	lua_pushboolean(lstate, nowEnabled);
	lua_setglobal(lstate, "programSoundEnabled");
	lua_pushboolean(lstate, soundActive);
	lua_setglobal(lstate, "programSoundActive");

	return true;// all ok
}// setSoundEnabledInLua

