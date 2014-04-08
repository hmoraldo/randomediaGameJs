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
GAMESOUND.*:


This is the file where the functions that execute the respective sounds
for every game event are placed.

*/

#ifndef FILE_GAMESOUND_INCLUDED
#define FILE_GAMESOUND_INCLUDED

#define FILE_GAMESOUND_VERSION "9-4-2003-H"// last modification: dd/mm/yy




// The functions that can be accessed from here...
bool makeSoundFlameBulletHitEnemy();
bool makeSoundPlayerDropBullet();
bool makeSoundGameOver();
bool makeSoundProtagTouched();
bool makeSoundProtagKilled();
bool makeSoundAdvanceStage();
bool makeSoundGeneratedUnit();
bool makeSoundFlameBulletHitWall();
bool makeSoundFriendlyUnitEscaped();
bool makeSoundHappySheep();
bool executeIngameMusic(int ordMusic);



#endif// FILE_GAMESOUND_INCLUDED
