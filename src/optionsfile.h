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
OPTIONFILE.*:

These files contain all the functions related to the management of
the game options that are saved in files, including those related to
the list of high scores

*/

#ifndef FILE_OPTIONSFILE_INCLUDED
#define FILE_OPTIONSFILE_INCLUDED

#define FILE_OPTIONSFILE_VERSION "2-1-2003-H"// last modification: dd/mm/yy

// Some useful files to include
#include "win-constants.h"// for using DWORD and that stuff
#include "globals.h"// for using the program objects

// Some useful defines
#define FILE_HSCORES_SCORES_ONE_MAXNAMES 10// maximum amount of persons in the high scores list One
#define FILE_HSCORES_SCORES_ONE_NAMELEN 20// maximum length of the string containing the name of somebody in the high scores list One, including the trailing zero
// maximum allowed length of the string containing the name of
// somebody in the high scores list
// (this has to be always at least two bytes lower than NAMELEN)
#define FILE_HSCORES_SCORES_ONE_VIRTUAL_NAMELEN 18


// HIGH SCORE STRUCT
// This structure stores the data of a person in the list of high scores.
struct highScoreStruct{
	char name[FILE_HSCORES_SCORES_ONE_NAMELEN];
	int score;
};// highScoreStruct

// OPTIONS STRUCT
// Structure where all the data is stored
struct optionsStruct{
	WORD header;// the header HO, from HHM's Options
	DWORD authenticityNumber;// something like a CRC, for checking authenticity

	// the language to use
	languageEnum selectedLanguage;
	// is the sound active?
	bool programSoundEnabled;

	// some statistics
	DWORD usedTimeSeconds;// time, in seconds, the program has been used

	// the high scores lists data
	// First list: the most commonly used
	highScoreStruct highScoresOne[FILE_HSCORES_SCORES_ONE_MAXNAMES];
};// optionsStruct

// HIGH SCORES MANAGER CLASS
// A class that manages all the data and stuff related to the high
// scores list, through the optionsFileClass object
class highScoresManagerClass{
private:
	// some useful data
	bool initialized;// was it already initialized?
	highScoreStruct* hScoresList;// list of high scores
	int listSize;// size of the list pointed by hScoresList
public:
	// constructor / destructor
	highScoresManagerClass():initialized(false) {}
	virtual ~highScoresManagerClass() {}

	// initialization / deinitialization
	bool init(highScoreStruct* hScoresList, int listSize);
	bool end();

	// methods for working with the high scores list
	bool resetList();

	// Methods fpr traversing the high scores list:
	// First, the methods for Higher Score First (HSF), the most useful
	// order for typical high scores lists.
	highScoreStruct* getFirstNodeHSF();
	highScoreStruct* getNextNodeHSF(highScoreStruct* since);
	highScoreStruct* getLastNodeHSF();
};// highScoresManagerClass

// OPTIONS FILE CLASS
// This is the object that manages the game options stored in files.
class optionsFileClass{
private:
	bool initialized;// was it already initialized?
	optionsStruct* options;// the options of this object
public:
	// constructors / destructors
	optionsFileClass():initialized(false) {}
	virtual ~optionsFileClass() {if (isReady()) end();}

	// init / deinit
	bool init();
	bool end();

	// working on the file
	bool load();
	bool save();
	bool createFileIfNeeded();

	// working with the struct
	bool resetOptions();

	// encryption / authenticity
	bool crypt();
	bool decrypt();
	bool checkAuthenticity() {
		if (options->authenticityNumber!=calculateAuthenticityNumber())
			return false;
		else
			return true;
	}// checkAuthenticity
	bool setAuthenticityNumber() {
		options->authenticityNumber=calculateAuthenticityNumber();
		return true;
	}// setAuthenticityNumber
	DWORD calculateAuthenticityNumber(bool* errorCode=NULL) {
		if (errorCode!=NULL) *errorCode=true;
		return 'MH';
	}// calculateAuthenticityNumber

	// simple getters / setters
	bool isReady() {return initialized;}
	optionsStruct* getOptionsPointer() {return options;}

	// objects that manage the multiple high score lists
	highScoresManagerClass* highScoresOneManager;
};// optionsFileClass


#endif// FILE_OPTIONSFILE_INCLUDED
