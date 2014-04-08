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
WAVEGENS.*:
(functions for managing the wave generators)


These files contain all the functions which are related with the
wave generators management.

*/

#ifndef FILE_WAVEGENS_INCLUDED
#define FILE_WAVEGENS_INCLUDED

#define FILE_WAVEGENS_VERSION "20-11-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "gamedata.h"// data types of this game


// Functions to be called before and after executing the game
bool createWaveGenObjects();
bool destroyWaveGenObjects();
bool executeEnemyWaves(DWORD frameNumber);

#endif// FILE_WAVEGENS_INCLUDED
