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

#include "massDeallocator.h"// for accessing to the massDeallocator object, useful for massive deallocations
#include "gamedata.h"

// MASS DEALLOCATOR METHODS
// ************************

// PRIVATE ONES

void deleteData(void* memory, bool oneOrMany, MemoryType memType) {
	if (oneOrMany) {
		// one
		if (memType == MEMTYPE_FRAMEDATA) {
			delete (frameDataStruct*) memory;
		} else {
			delete (char*) memory;
		}
	}else{
		// many
		if (memType == MEMTYPE_FRAMEDATA) {
			delete[] (frameDataStruct*) memory;
		} else {
			delete[] (char*) memory;
		}
	}
}

// genericRemoveMemAllocation *******
// This method removes a memory allocation from the internal list,
// with and without deallocating it
bool massDeallocatorClass::genericRemoveMemAllocation(
	void* memory,// data to remove from the list
	bool deallocate)// deallocate it at the time it's removed?
{
	allocatedNodeStruct* node;

	if (!ready) return false;// it should have been initialized previously

	// traverse the list looking for such mem object
	for (node=memoryList;
	node!=NULL && node->memory!=memory;
	node++);

	// if we didn't find it, return an error value
	if (node==NULL) return false;

	// remove it from the list...
	// remove references from the previous node
	if (node->previousNode==NULL) {
		memoryList=node->nextNode;
	}else{
		node->previousNode->nextNode=node->nextNode;
	}
	// remove references from the next node
	if (node->nextNode!=NULL) {
		node->nextNode->previousNode=node->previousNode;
	}

	// deallocate it if needed
	if (deallocate) {
		deleteData(node->memory, node->oneOrMany, node->memType);
	}

	// and deallocate the node information, always
	delete node;

	return true;// all ok
}// genericRemoveMemAllocation



// INITIALIZATION / DEINITIALIZATION

// init *********
// This method initializes the mass deallocator object
bool massDeallocatorClass::init()
{
	if (ready) return false;// check it's not already initialized

	ready=true;
	memoryList=NULL;

	return true;// all ok
}// init

// close *********
// This method closesthe mass deallocator object
bool massDeallocatorClass::close()
{
	bool ok;

	if (!ready) return false;// it should have been initialized previously

	// try removing all from memory
	ok=massDeallocate();

	// ok...
	ready=false;
	return ok;
}// close

// LIST MANAGEMENT

// addMemAllocation ******
// This method adds a memory allocation to the list of mem to
// deallocate. It only adds it, doesn't ever allocate anything for
// the caller.
bool massDeallocatorClass::addMemAllocation(
	void* memory,// memory position of the memory object to add
	MemoryType memType,
	bool oneOrMany)// that is: true for an mem obj that should be later deallocated with delete; false, with delete[]
{
	allocatedNodeStruct* node;

	if (!ready) return false;// it should have been initialized previously

	// create the node
	node=new allocatedNodeStruct;
	node->memory=memory;
	node->oneOrMany=oneOrMany;
	node->previousNode=NULL;
	node->memType = memType;

	// add the node at the beginning of the linked list
	node->nextNode=memoryList;
	if (node->nextNode!=NULL) node->nextNode->previousNode=node;
	memoryList=node;

	// all ok
	return true;
}// addMemAllocation

// removeMemAllocation *******
// This method removes a memory allocation from the internal list,
// but without ever deallocating it (at least not by default, use
// deallocateOne for these purposes).
bool massDeallocatorClass::removeMemAllocation(
	void* memory)
{
	// remove it from the list, without deallocating it
	return genericRemoveMemAllocation(memory, false);
}// removeMemAllocation 


// MEMORY MANAGEMENT

// massDeallocate *******
// This method deallocates all what's present in the linked list of
// memory to deallocate
bool massDeallocatorClass::massDeallocate()
{
	allocatedNodeStruct* node;
	allocatedNodeStruct* nextNode;

	if (!ready) return false;// it should have been initialized previously

	// go through the entire list deallocating all the nodes
	for (node=memoryList;
	node!=NULL;
	node=nextNode)// node=tempNode means node++
	{
		// deallocate that object
		deleteData(node->memory, node->oneOrMany, node->memType);

		// remove the current node from memory, and remember which
		// one will be the next node
		nextNode=node->nextNode;
		delete node;
	}

	// initialize all back as a brand new massDeallocator, ready
	// for striking back
	memoryList=NULL;

	// all ok
	return true;
}// massDeallocate

// deallocateOne *******
// This method deallocates one specific memory allocation, and
// removes it from the internal list.
bool massDeallocatorClass::deallocateOne(void* memory)
{
	// remove it from the list, and deallocate it
	return genericRemoveMemAllocation(memory, true);
}// deallocateOne
