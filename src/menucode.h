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
MENUCODE.*:

Here goes the main loop of the program zone corresponding to the menues
of the program.

Program zone: pgzon_gameMenues.

*/

#ifndef FILE_MENUCODE_INCLUDED
#define FILE_MENUCODE_INCLUDED

#define FILE_MENUCODE_VERSION "17-1-2003-H"// last modification: dd/mm/yy

/*#include "winutil.h"// for accessing to some useful functions related to low level windows specific code*/
#include "scriptman.h"// used for accessing to the script manager object

// MAIN FUNCTIONS
// These functions are accessed as MenuLoopCode::*
namespace MenuLoopCode{
	bool afterLoop();
	bool beforeLoop();
	bool executeFrame(DWORD frameNumber);
	bool drawFrame();
}// namespace MenuLoopCode


// FUNCTIONS FOR INTERFACING WITH LUA
int luaMenuBltImage(lua_State* lstate);
int luaMenuDrawText(lua_State* lstate);
int luaSwitchToMenu(lua_State* lstate);
int luaMenuSetMouseXY(lua_State* lstate);
int luaSetCurrentLanguage(lua_State* lstate);
int luaUpdateHighScoresList(lua_State* lstate);
int luaSetSoundEnabled(lua_State* lstate);
int luaPlaySoundResource(lua_State* lstate);
int luaStopSoundResource(lua_State* lstate);
int luaStopAndPlaySoundResource(lua_State* lstate);
int luaResetHighScores(lua_State* lstate);

// EXTRA FUNCTIONS
bool executeHighScoreTyping(char key);


#endif// FILE_MENUCODE_INCLUDED
