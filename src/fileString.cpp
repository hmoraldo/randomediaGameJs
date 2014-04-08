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

#include "fileString.h"// for accessing to strings, memory, files and windows resources in an only simple way
#include <stdio.h>
#include <string.h>


// FILE STRING CLASS METHODS
// *************************

// OPENERS
// openFromFile ******
// initializes the fileString object from a file... all next method
// calls asking for string data will actually taking that data from
// the file.
bool fileStringClass::openFromFile(char* fileName)
{
	FILE* file;
	DWORD fsize;
	DWORD readed;

	if (isReady()) return false;// should be closed first, if it has been initialized before

	// open the file
	file=fopen(fileName, "rb");
	if (file==NULL) return false;

	// get the file size and check for errors
	if (fseek(file,0,SEEK_END)!=0) {
		fclose(file);
		return false;
	}
	fsize=ftell(file);
	fseek(file,0,SEEK_SET);

	// create the memory and set up the data...
	memory=new BYTE[fsize];

	// read the data...
	readed=fread(memory, 1, fsize, file);
	// TODO 2010: big while for this
	if (readed<fsize) return false;

	// close the file...
	fclose(file);

	// set up the filestring data...
	shouldReleaseMemory=true;
	size=fsize;
	ready=true;

	// all ok...
	return true;
}// openFromFile
/*
// openFromResource ******
// initializes the fileString object from a windows resource...
// all next method calls asking for string data will actually taking
// that data from the resource.
bool fileStringClass::openFromResource(
	char* resName,// resource name
	HINSTANCE hInstance)// app instance handler
{
	if (isReady()) return false;// should be closed first, if it has been initialized before

	// yet to be implemented, check open from file to see how
	return false;
}// openFromResource 
*/
// openFromStringZ ******
// initializes the fileString object from a string ending in zero
// all next method calls asking for string data will actually taking
// that data from the stringZ
bool fileStringClass::openFromStringZ(
	char* stringz,// pointer to the string itself
	bool localCopy)// will the string be copied to a memory position that is local to this object?
{
	if (isReady()) return false;// should be closed first, if it has been initialized before

	// get the size data...
	size=strlen(stringz);

	// do the local copy of the data if needed
	if (localCopy) {
		// do a copy of the data
		memory=new char[size];
		memcpy(memory, stringz, size);
	}else{
		// no copy, it's actually easier
		memory=stringz;
	}

	// set all up
	shouldReleaseMemory=localCopy;
	ready=true;

	// all ok
	return true;
}// openFromStringZ

// openFromMemory ******
// initializes the fileString object from a memory pointer
// all next method calls asking for string data will actually taking
// that data from that memory position.
bool fileStringClass::openFromMemory(
	void* mem,// pointer to the memory data itself
	int memSize,// size of the memory to be used
	bool localCopy)// will the string be copied to a memory position that is local to this object?
{
	if (isReady()) return false;// should be closed first, if it has been initialized before

	// do the local copy of the data if needed
	if (localCopy) {
		// do a copy of the data
		memory=new BYTE[memSize];
		memcpy(memory, mem, memSize);
	}else{
		// no copy, it's actually easier
		memory=mem;
	}

	// set all up
	shouldReleaseMemory=localCopy;
	size=memSize;
	ready=true;

	// all ok
	return true;
}// openFromMemory 

// CLOSERS
// close *********
// This method closes the fileString object
bool fileStringClass::close()
{
	if (!isReady()) return false;// should be initialized first

	// when needed, release the memory used for the data (only
	// when it's 'local' to the object)
	if (shouldReleaseMemory) delete[] ((BYTE*)memory);

	// close all
	ready=false;

	// closed!
	return true;
}// close

// SIMPLE INTERFACES

// getDataPointer ********
// This method returns a pointer to the data in memory
void* fileStringClass::getDataPointer()
{
	if (!isReady()) return false;// should be initialized first

	return memory;
}// getDataPointer

// isReady **********
// This method simply tells whether we are ready or not for using
// this object (that is, if it has been initialized)
bool fileStringClass::isReady()
{
	return ready;
}// isReady

// getDataSize *********
// This method returns the amount of bytes that are to be accessed
// with getDataPointer... that is, what the file / resource / string
// / memory size is
int fileStringClass::getDataSize()
{
	if (!isReady()) return false;// should be initialized first

	return size;
}// getDataSize
