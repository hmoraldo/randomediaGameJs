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
ENEMYAI.*:
(functions for managing the enemy AI)


These files contain the AI for the game enemy units, and the main
functions which manage the enemy army as a whole.

*/

#ifndef FILE_ENEMYAI_INCLUDED
#define FILE_ENEMYAI_INCLUDED

#define FILE_ENEMYAI_VERSION "16-4-2002-H"// last modification: dd/mm/yy

// some useful header files
#include "gamedata.h"


// Functions of this file
bool executeEnemyAI(DWORD frameNumber);
bool executeUnitAI(DWORD frameNumber,vectorListNodeStruct<enemyUnitStruct>& unit);
bool enemyUnitFreed(enemyUnitStruct* enemyData);

// Some more internal functions
bool unitStartFalling(enemyUnitStruct& unitE);

#endif// FILE_ENEMYAI_INCLUDED
