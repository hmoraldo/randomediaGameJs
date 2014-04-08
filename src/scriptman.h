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
SCRIPTMAN.*:
(functions for managing the game scripts)


These files contain the scriptMan object, which is used for accessing
to the scripts used in the game.

*/

#ifndef FILE_SCRIPTMAN_INCLUDED
#define FILE_SCRIPTMAN_INCLUDED

#define FILE_SCRIPTMAN_VERSION "2-8-2003-H"// last modification: dd/mm/yy


// header files for using lua
extern "C"{
	#include <lua.h>
}// extern

// other useful includes...
#include "globals.h"// for using the program objects


// scriptManagerClass
class scriptManagerClass{
private:
	bool ready;// has it been properly initialized
	lua_State* lstate;// lua state object
public:
	// constructor / destructor
	scriptManagerClass():ready(false) {}
	~scriptManagerClass() {}

	// init / deinit
	bool init();
	bool close();

	// some general purpose interfaces
	bool isReady() {return ready;}
	lua_State* getLuaObject() {return lstate;}
	int getIntVariable(char* varName, bool* errCode=NULL);
	double getDoubleVariable(char* varName, bool* errCode=NULL);
	char* getStringValue(char* varName, bool* errCode=NULL);
	bool doString(char* string);

	// some interfaces specific for this game...
	bool setCurrentLanguageInLua(enum languageEnum lang);
	bool setSoundEnabledInLua(bool nowEnabled, bool soundActive);
};// scriptManagerClass

#endif// FILE_SCRIPTMAN_INCLUDED
