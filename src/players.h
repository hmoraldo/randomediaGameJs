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
PLAYERS.*:
(functions for managing the players)


These files contain all the functions which are related with the players
management.

*/

#ifndef FILE_PLAYERS_INCLUDED
#define FILE_PLAYERS_INCLUDED

#define FILE_PLAYERS_VERSION "17-4-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "gamedata.h"// data types of this game
#include "diwrap.h"// data types of this game

// Functions to be called before and after executing the game
bool createPlayerObjects();
bool destroyPlayerObjects();

// Other useful functions
bool playerIncrementScoreForHit(gameObjectPointerStruct& unitP, double extraScore);
bool allPlayersIncrementScore(double extraScore);
bool movePlayerUnit(playerUnitStruct& unit, FILE_DIWRAP_KEYS_TYPE* keys);
bool movePlayers(FILE_DIWRAP_KEYS_TYPE* keys);

#endif// FILE_PLAYERS_INCLUDED
