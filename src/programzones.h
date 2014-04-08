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
PROGRAMZONES.*:

Here goes the code which manages the program zones and the different tasks
related to them (switching).

Program Zones are the different areas of the program, which require
specific executeFrame and drawFrame functions to work.

*/

#ifndef FILE_PROGRAMZONES_INCLUDED
#define FILE_PROGRAMZONES_INCLUDED

#define FILE_PROGRAMZONES_VERSION "15-7-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "win-constants.h"// for defining those common types, like DWORD, etc


// Program Zone Numbers Declaration:
// All the program zone names are pgzon_Z, where Z is the name of the zone.
enum programZonesEnum {
	pgzon_emptyZone=0,// null zone, or rather, a zone without resources
	pgzon_actualGameRunning,// zone for the game running
	pgzon_gameIntro,// zone for the game intro
	pgzon_gameMenues,// zone for the game menues

	// invalidZone has to have the highest enum value, keep it the
	// latest...
	pgzon_invalidZone// invalid zone, to be used only as a delimiter in zone vectors
};// programZonesEnum

// a value telling how many program zones are there, used for the
// creation of arrays and so... its value is based in pgzon_invalidZone,
// so don't change the enum unless without being sure TOTALPROGRAMZONES
// will still be valid.
// Includes pgzon_invalidZone and pgzon_emptyZone.
#define TOTALPROGRAMZONES (pgzon_invalidZone+1)

// Functions related to the management of program zones
bool switchToProgramZone(programZonesEnum programZone, DWORD programSubZone, DWORD programSubSubZone, bool reInitializeAllResources=true);
bool switchToNullProgramZone();

#endif// FILE_PROGRAMZONES_INCLUDED
