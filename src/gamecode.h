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
GAMECODE.*:

Here goes the main loop of the real game (that is, the game without
considering initial screens, options and so on).

Program zone: pgzon_actualGameRunning.

*/

#ifndef FILE_GAMECODE_INCLUDED
#define FILE_GAMECODE_INCLUDED

#define FILE_GAMECODE_VERSION "8-4-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "win-constants.h"
#include "ddwrap.h"
#include "diwrap.h"
/*#include "audiowrap.h"*/
#include "console.h"
#include "con-logger.h"
#include "globals.h"// for using the program objects
#include "mainloop.h"// for accessing to the main loop object


// MAIN FUNCTIONS
// These functions are accessed as GameLoopCode::*
namespace GameLoopCode{
	bool afterLoop();
	bool beforeLoop();
	bool executeFrame(DWORD frameNumber);
	bool drawFrame();
}// namespace GameLoopCode


#endif// FILE_GAMECODE_INCLUDED
