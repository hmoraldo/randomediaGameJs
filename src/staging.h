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
STAGING.*:

These files contain all the functions related to the management of
the stages, the levels, and the advance and switching between these two.

*/

#ifndef FILE_STAGING_INCLUDED
#define FILE_STAGING_INCLUDED

#define FILE_STAGING_VERSION "20-12-2002-H"// last modification: dd/mm/yy

// some header files to be used
#include "gamedata.h"

// This is the structure that tells the properties of a stageLevel.
struct stageLevelPropertiesStruct{
	int maximumTime;// maximum working time for a stage level, 0 for infinitum
};


// Some useful functions
int getNextLevel(int fromLevel, gameStagesEnum fromStage);
gameStagesEnum getNextStage(int fromLevel, gameStagesEnum fromStage);
bool activateCurrentStageLevel();
bool setStageLevel(int level, gameStagesEnum stage);
bool advanceStage();
bool logCurrentStageLevelInfo(bool toConsole);
int getStageLevelIndex(int level, gameStagesEnum stage);
stageLevelPropertiesStruct getStageLevelProperties(int level, gameStagesEnum stage);
bool manageAutomaticStaging(DWORD frameNumber, bool* didItSwitch=NULL);
double getTimeLeftInSeconds();


#endif// FILE_STAGING_INCLUDED
