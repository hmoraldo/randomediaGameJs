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

#include "optionsfile.h"// for the management of the files where the game options are saved
/*#include "winutil.h"// for accessing to some useful functions related to low level windows specific code*/
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
/*#include "gamedata.h"// data types of this game*/
#include "programinfo.h"// for general info about the program itself
#include <string.h>

#define FILE_OPTIONSFILE_OPTIONSFILENAME "data/gamedata.dat"

// HIGH SCORES MANAGER CLASS

// init ************
// Initializes the object...
bool highScoresManagerClass::init(
	highScoreStruct* hScoresList,// pointer to a list of high scores
	int listSize)// size of such list
{
	if (initialized) return false;

	// initialize all...
	highScoresManagerClass::hScoresList=hScoresList;
	highScoresManagerClass::listSize=listSize;

	// all ok
	initialized=true;
	return true;
}// init

// end ************
// Deinitializes the object...
bool highScoresManagerClass::end()
{
	if (!initialized) return false;

	// deinitialize all...
	// (nothing to do here, yet)

	// all ok
	initialized=false;
	return true;
}// end

// resetList ************
// Resets the lists of high scores (assigns default values to all the
// nodes)
bool highScoresManagerClass::resetList()
{
	int i;

	if (!initialized) return false;

	// go through the entire list
	for (i=0; i<listSize; i++) {
		// assign a default value to every node
		hScoresList[i].score=(i+1)*1500+(i-4>0?i-4:0)*3000;
		strcpy(hScoresList[i].name, "Anonymous");
	}

	return true;// all ok
}// resetList

// getLastNodeHSF ************
// Used a lot for adding nodes. When adding a node, some other node
// is supposed to be taken out of the list (as it's fixedly sized), and
// this node to be taken out, in a typical high scores list, is the one
// with the lowest score, so the latest in a higher score first (HSF)
// list. So we use, instead of addNode, this method to take the latest
// node and modify it.
// The next node of the last node is NULL.
highScoreStruct* highScoresManagerClass::getLastNodeHSF()
{
	highScoreStruct* lastNode;
	highScoreStruct* tempNode;

	// get lastNode to point to the latest node in HSF order
	// (when that's done, tempNode will point to NULL)
	lastNode=NULL;
	for (tempNode=getFirstNodeHSF(); tempNode!=NULL;
		lastNode=tempNode, tempNode=getNextNodeHSF(tempNode));
	;// just to correct indentation

	// ready (lastNode will be NULL in the case of error)
	return lastNode;
}// getLastNodeHSF

// getFirstNodeHSF ************
// Returns a pointer to the first node of the high scores list, in
// Higher Score First order (so the node returned is the one with the
// highest score).
// If there are two nodes with the same score, and it's the highest
// score available, the choosen node is the one with the lowest index.
highScoreStruct* highScoresManagerClass::getFirstNodeHSF()
{
	int i;
	int hSIdx;// highest score index

	if (!initialized) return NULL;

	// check for empty lists...
	if (listSize<=0) return NULL;

	// look for the index of the node with the highest score
	for (i=0, hSIdx=i; i<listSize; i++) {
		if (hScoresList[i].score>hScoresList[hSIdx].score) {
			// we found a higher score!
			hSIdx=i;
		}
	}

	// ready
	return &(hScoresList[hSIdx]);
}// getFirstNodeHSF

// getNextNodeHSF ************
// Returns a pointer to the next node of the high scores list, in
// Higher Score First order, since a given node.
// If there are two nodes with the same score, and both are supposed
// to be the next node, the first node to be visited will be the one with
// the lowest index.
// The next node of the last node is NULL.
highScoreStruct* highScoresManagerClass::getNextNodeHSF(
	highScoreStruct* since)// node where to start counting
{
	int i;
	int hSIdx, hSIdx2;// highest score index
	int sinceIndex=since-hScoresList;

	if (!initialized) return NULL;

	// check for empty lists and other easy stuff...
	if (listSize<=0) return NULL;
	if (since==NULL) return NULL;
	// check for list boundaries
	if (sinceIndex<0 || sinceIndex>=listSize) return NULL;

	// look for the index of the node with the highest score, in the
	// half next to the 'since' index
	for (i=sinceIndex+1, hSIdx=-1; i<listSize; i++) {
		if ((hSIdx<0 || hScoresList[i].score>hScoresList[hSIdx].score)
			&& hScoresList[i].score<=since->score) {

			// we found a higher score!
			hSIdx=i;
		}
	}

	// look for the index of the node with the highest score, in the
	// half previous to the 'since' index
	// (note here we compare "hScoresList[i].score<since->score",
	// with a lower-only sign, as if the next node has a lower
	// index than since has, then it cannot have the same score
	// _never_, according to the way equal scores are ordered).
	for (i=0, hSIdx2=-1; i<sinceIndex; i++) {
		if ((hSIdx2<0 ||
				hScoresList[i].score>hScoresList[hSIdx2].score)
			&& hScoresList[i].score<since->score) {

			// we found a higher score!
			hSIdx2=i;
		}
	}

	// now choose between hSIdx and hSIdx2, the right value will be put
	// in hSIdx
	// check for error values
	if (hSIdx<0 || hSIdx==sinceIndex ||
		hScoresList[hSIdx].score>since->score ||
		hSIdx>=listSize) hSIdx=-1;
	if (hSIdx2<0 || hSIdx2==sinceIndex ||
		hScoresList[hSIdx2].score>since->score ||
		hSIdx2>=listSize) hSIdx2=-1;
	if (hSIdx==-1) hSIdx=hSIdx2;
	// now check hSIdx2 should be put in hSIdx: it has to be put there
	// only if it's strictly bigger than the highest value in the
	// right side of the list
	// Cases ('left' means 'left score', that is, hSIdx.score):
	// 1- Left (hSIdx2) is lower than right (hSIdx), then hSIdx=hSIdx.
	// (trivial)
	// 2- Right (hSIdx) is lower than left (hSIdx2), then hSIdx=hSIdx2.
	// (trivial)
	if (hSIdx2!=-1 && hSIdx!=1  &&
		(hScoresList[hSIdx2].score>hScoresList[hSIdx].score))
		hSIdx=hSIdx2;
	// 3- Left (hSIdx2) is equal to right (hSIdx), then there are two
	// cases:
	// a- hSIdx==since, then hSIdx=hSIdx (because hSIdx2 has supposedly
	// been used before)
	// b- hSIdx!=since, then hSIdx=hSIdx2 (because it's before)
	if (hSIdx2!=-1 && hSIdx!=1  &&
		(since->score!=hSIdx2) &&
		(hScoresList[hSIdx2].score==hScoresList[hSIdx].score))
		hSIdx=hSIdx2;

	// check to see if we found such next node or not (that is, check
	// all erroneous values of hSIdx)
	if (hSIdx<0) {

		// node not found
		return NULL;
	}

	// ready
	return &(hScoresList[hSIdx]);
}// getNextNodeHSF


// OPTIONS FILE CLASS

// init ************
// Initializes the object...
bool optionsFileClass::init()
{
	if (initialized) return false;

	// initialize all...
	options=new optionsStruct;

	// initialize the high scores lists
	highScoresOneManager=new highScoresManagerClass;
	if (!highScoresOneManager->init(
		options->highScoresOne,// list of high scores
		FILE_HSCORES_SCORES_ONE_MAXNAMES))// list size
	{
		// error!
		delete[] options;
		delete[] highScoresOneManager;
		return false;
	}

	// all ok
	initialized=true;
	return true;
}// init

// end ************
// Deinitializes the object...
bool optionsFileClass::end()
{
	if (!initialized) return false;

	// deinitialize all...
	highScoresOneManager->end();
	delete[] highScoresOneManager;
	delete[] options;

	// all ok
	initialized=false;
	return true;
}// end

// load ************
// Loads the options from the file...
bool optionsFileClass::load()
{
/*	HANDLE hFile;
	DWORD fileSize;
	DWORD dataSize=sizeof(optionsStruct);
	DWORD bytesRead=0;

	if (!initialized) return false;

	// Open the file for reading (it has to exist)
	if ((hFile=CreateFile(
		FILE_OPTIONSFILE_OPTIONSFILENAME,//name of the file
		GENERIC_READ,//desired access
		FILE_SHARE_READ,//share mode
		NULL,//security attributes
		OPEN_EXISTING,//creation disposition
		FILE_ATTRIBUTE_NORMAL,//flags and attr
		NULL)//template file
		)==INVALID_HANDLE_VALUE)

		return false;

	// check the file size is enough
	fileSize=GetFileSize(hFile,NULL);
	if (fileSize==0xFFFFFFFF || fileSize<dataSize) {

		// failed
		CloseHandle(hFile);
		return false;
	}

	// read the data to the options structure
	do {
		if (0==ReadFile(
			hFile,// the file
			((BYTE*)options)+bytesRead,// where to read to
			dataSize,// how many bytes to read
			&bytesRead,// where to tell how many bytes have been read
			NULL))// overlapped?
		{

			// failed
			CloseHandle(hFile);
			return false;
		}
	}while(bytesRead<dataSize);

	// that's all
	CloseHandle(hFile);

	// time to decrypt and authenticate what we took from the file
	if (!decrypt() || !checkAuthenticity()) return false;
*/
	return true;// all ok
}// load

// save ************
// Saves the options data to the options file...
bool optionsFileClass::save()
{
/*	HANDLE hFile;
	DWORD dataSize=sizeof(optionsStruct);
	DWORD bytesWritten=0;

	if (!initialized) return false;

	// first, time to crypt and set the authenticity of
	// the data
	if (!setAuthenticityNumber() || !crypt()) return false;

	// Open the file for writing (it must not exist)
	if ((hFile=CreateFile(
		FILE_OPTIONSFILE_OPTIONSFILENAME,//name of the file
		GENERIC_WRITE,//desired access
		0,//share mode
		NULL,//security attributes
		CREATE_ALWAYS,//creation disposition
		FILE_ATTRIBUTE_NORMAL,//flags and attr
		NULL)//template file
		)==INVALID_HANDLE_VALUE)

		return false;

	// write the options structure to the file
	do {
		if (0==WriteFile(
			hFile,// the file
			((BYTE*)options)+bytesWritten,// where to read to
			dataSize,// how many bytes to read
			&bytesWritten,// where to tell how many bytes have been read
			NULL))// overlapped?
		{

			// failed
			CloseHandle(hFile);
			return false;
		}
	}while(bytesWritten<dataSize);

	// that's all
	CloseHandle(hFile);

	// now we wrote the structure, we can decrypt it again
	if (!decrypt()) return false;
*/
	return true;// all ok
}// save

// createFileIfNeeded ************
// Checks whether the options file exists or not, and if it does not
// exist, it creates such file
bool optionsFileClass::createFileIfNeeded()
{
/*	bool errCode;
	bool exists;

	if (!initialized) return false;

	// check the file exists
	exists=WinUtilCode::fileExists(
		FILE_OPTIONSFILE_OPTIONSFILENAME, &errCode);
	if (!errCode) return false;

	// if the file exists, it's time to go out
	if (exists) return true;

	// otherwise, create it new
	if (!resetOptions() || !save()) return false;
*/
	// that's all, and all has been ok
	return true;
}// createFileIfNeeded

// resetOptions ************
// This method resets all the data of the options structure, so they
// have the default values.
bool optionsFileClass::resetOptions()
{
/*	if (!initialized) return false;

	// first zero all the data
	memset(options, 0, sizeof(*options));

	// set all up (but don't touch authentication data)
	options->header='OH';// the header HO, from HHM's Options
	options->usedTimeSeconds=0;// time, in seconds, the program has been used

	// Set the default language according to the program info
	#ifdef PROGMODE_DEFAULTLANGUAGE_SPANISH
		options->selectedLanguage=language_spanish;
	#else// PROGMODE_DEFAULTLANGUAGE_ENGLISH
		options->selectedLanguage=language_english;
	#endif// PROGMODE_DEFAULTLANGUAGE_SPANISH

	// Set sound to be active initially
	options->programSoundEnabled=true;

	// reset the high scores...
	if (!highScoresOneManager->resetList()) return false;
*/
	return true;// all ok
}// resetOptions

// crypt ************
// This method crypts all the data in the options structure.
bool optionsFileClass::crypt()
{
/*	BYTE* thisByte;

#ifdef PROGMODE_DEVELOPER_YES
	// don't crypt anything
	return true;
#endif// PROGMODE_DEVELOPER_YES

	if (!initialized) return false;

	// crypt all... (typical xor)
	for (thisByte=(BYTE*)options;
	thisByte-(BYTE*)options<sizeof(optionsStruct);
	thisByte++)
		(*thisByte)=(*thisByte)^1;
*/
	return true;// all ok
}// crypt

// decrypt ************
// This method decrypts all the data in the options structure.
bool optionsFileClass::decrypt()
{
/*	BYTE* thisByte;

#ifdef PROGMODE_DEVELOPER_YES
	// don't decrypt anything, unless strictly necessary
	// That is: developer mode let us decrypt option files and use
	// them decrypted later.
	if (checkAuthenticity()) return true;
#endif// PROGMODE_DEVELOPER_YES

	if (!initialized) return false;

	// decrypt all... (typical xor)
	for (thisByte=(BYTE*)options;
	thisByte-(BYTE*)options<sizeof(optionsStruct);
	thisByte++)
		(*thisByte)=(*thisByte)^1;
*/
	return true;// all ok
}// decrypt






