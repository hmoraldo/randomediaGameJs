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
COLLISIONS.*:


Here go all the functions related to the management of collisions of
units of any kind.

*/

#ifndef FILE_COLLISIONS_INCLUDED
#define FILE_COLLISIONS_INCLUDED

#define FILE_COLLISIONS_VERSION "8-4-2002-H"// last modification: dd/mm/yy


// some useful headers
#include "gamedata.h"// data types of this game

// Some functions to be used everywhere.
bool checkCollisions();

// Some other more specific functions, that should be used only for
// specific stuff.
bool bulletUnitHit(vectorListNodeStruct<bulletUnitStruct>& bUnit, gameObjectPointerStruct& toucher);

#endif// FILE_COLLISIONS_INCLUDED
