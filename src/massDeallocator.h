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
MASSDEALLOCATOR.*:
(massDeallocator: an object that is used to deallocate big numbers
of different memory allocations with a simple call... useful for
easily avoiding memory leaks when allocating lots of objects at a
time is necessary)

These files contain the declaration of the massDeallocator object,
which was designed to easily deallocate big numbers of different
memory allocations.

*/

#ifndef FILE_MASSDEALLOCATOR_INCLUDED
#define FILE_MASSDEALLOCATOR_INCLUDED

#define FILE_MASSDEALLOCATOR_VERSION "28-7-2003-H"// last modification: dd/mm/yy


#include "win-constants.h"

// ALLOCATED NODE STRUCT
struct allocatedNodeStruct{
	void* memory;// pointer to the memory data to deallocate
	bool oneOrMany;// that is: true for deallocating with delete; false, with delete[]

	allocatedNodeStruct* nextNode;// pointer to the next node
	allocatedNodeStruct* previousNode;// pointer to the previous node
};// allocatedNodeStruct

// MASS DEALLOCATOR CLASS
// The massDeallocator object
class massDeallocatorClass{
private:
	allocatedNodeStruct* memoryList;
	bool ready;

	// for using with removeMemAllocation and deallocateOne
	bool genericRemoveMemAllocation(void* memory, bool deallocate);

public:
	// constructor / destructor
	massDeallocatorClass():ready(false) {}
	virtual ~massDeallocatorClass() {if (isReady()) close();}

	// initialization / deinitialization
	virtual bool init();
	virtual bool close();

	// list management
	virtual bool addMemAllocation(void* memory, bool oneOrMany=true);
	virtual bool removeMemAllocation(void* memory);

	// memory management
	virtual bool massDeallocate();
	virtual bool deallocateOne(void* memory);

	// useful stuff
	virtual bool isReady() {return ready;}
};// massDeallocatorClass

#endif// FILE_MASSDEALLOCATOR_INCLUDED
