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
VECTORLIST.*:


These files set up a template for managing dynamics lists of objects
with no special order, by using the once-allocated memory of a vector.
It's somewhat a mix between a vector and a linked list, though the order
in the objects should not be important here.

This vector list is optimized both for adding and traversing the nodes.

*/

#ifndef FILE_VECTORLIST_INCLUDED
#define FILE_VECTORLIST_INCLUDED

#define FILE_VECTORLIST_VERSION "19-12-2002-H"// last modification: dd/mm/yy

#include "win-constants.h"// for defining those common types, like DWORD, etc

// This is the template which defines a node.
template <class T>
struct vectorListNodeStruct {
	// here is stored the real data...
	T data;

	// here is stored the condition of this node
	bool empty;
};

// This is the main template, which manages the lists.
template <class T>
class vectorListClass{
private:
	// Some useful variables:
	// this variable tells whether the list was already initialized or not
	bool initialized;
	// this variable tells how many nodes there are in the list.
	DWORD nodesInList;
	// this variable points to the last active mode... thanks to this
	// info the iterator can know where to stop when looking for active
	// nodes.
	// lastActiveNode=nodesInList if no one is active.
	DWORD lastActiveNode;
	// this variable points to the first inactive node... it's very
	// helpful when adding an active node, as it already tells what's the
	// first empty node to use.
	// firstEmptyNode=nodesInList if no one is empty.
	DWORD firstEmptyNode;
	// this variable is used for statistical purposes only (and for
	// checking the overall use). It tells what's been the maximum amount
	// of enemies in the list ever.
	DWORD maxNodesUntilNow;
	// And here is the list itself, which actually is a vector...
	vectorListNodeStruct<T> *nodesList;

	// Some private methods...
	bool activateNode(DWORD nodeNumber);
public:
	// Some useful methods...
	bool initialize(DWORD size);
	bool end();
	bool resetList();
	bool removeNode(DWORD nodeNumber);
	DWORD addNode(T& data);

	// Methods used for traversing the list
	DWORD getIndexFirstNode();
	DWORD getNextNode(DWORD since);
	vectorListNodeStruct<T> *getFirstNode();
	vectorListNodeStruct<T> *getNextNode(vectorListNodeStruct<T> *since);

	// Some inlined methods:
	// Used for getting a node from its number... returns null if the
	// index isn't allowed
	vectorListNodeStruct<T> *getNode(DWORD nodeNumber) {
		return (isValidIndex(nodeNumber)?nodesList+nodeNumber:NULL);}
	DWORD getIndex(vectorListNodeStruct<T> *node) {return (node-nodesList);}

	// Some simple methods...
	DWORD getMaxNodesUntilNow() {return maxNodesUntilNow;}
	DWORD vectorSize() {return nodesInList;}
	bool isValidIndex(DWORD index) {return index>=0 && index<vectorSize();}
	bool isReady() {return initialized;}

	// constructor / destructor
	vectorListClass():initialized(false),nodesList(NULL) {}
	~vectorListClass() {end();}
};// vectorListClass

// VECTORLISTCLASS methods
// This method initializes the list, by allocating memory and resetting
// it.
template <class T>
bool vectorListClass<T>::initialize(DWORD size)
{
	if (initialized) return false;// error!

	// allocate the memory needed
	nodesList=new vectorListNodeStruct<T>[size];
	if (nodesList==NULL) return false;// memory couldn't be allocated
	nodesInList=size;

	// set the statistic to zero
	maxNodesUntilNow=0;

	// change the flag
	initialized=true;

	// go reset the list
	if (!resetList()) {
		// it's been an error
		initialized=false;
		return false;
	}

	return true;// all ok
}// initialize

// Free the memory in use and terminate the list system.
template <class T>
bool vectorListClass<T>::end()
{
	if (!initialized) return false;// error!

	// free the memory in use
	delete[] nodesList;
	nodesList=NULL;

	// change the flag
	initialized=false;

	return true;// all ok
}// end

// Reset the contents of the list...
template <class T>
bool vectorListClass<T>::resetList()
{
	DWORD i;
	vectorListNodeStruct<T>* node;

	if (!initialized) return false;// error!

	// go for the first node
	node=nodesList;

	// iterate the entire list to empty it
	for (i=0; i<nodesInList; i++) {
		// place a void node
		node->empty=true;

		// go for the next one
		node++;
	}// for i

	// set some extra variables
	lastActiveNode=nodesInList;// no one is active
	firstEmptyNode=0;// the first node is empty (and all of them are)

	return true;// all ok
}// resetList

// Activate a node and check for other changes that are needed when some
// nodes are activated.
// This method VERY important for the correct working of this class.
// Hence, this method has to be called every time a node is activated.
template <class T>
bool vectorListClass<T>::activateNode(DWORD nodeNumber)
{
	DWORD i;
	vectorListNodeStruct<T>* node;

	if (!initialized) return false;// error!

	// activate the node
	nodesList[nodeNumber].empty=false;

	// check for our special case: if we activate a node which is
	// after the lastActiveNode, we need to update that variable...
	// If no nodes are active, it has to be updated too.
	if (nodeNumber>lastActiveNode || lastActiveNode==nodesInList) {
		// now it's the last active node!
		lastActiveNode=nodeNumber;
	}

	// second special case: we are activating the first empty node...
	if (nodeNumber==firstEmptyNode) {
		// in such a case, we have to look for the next empty node...
		// which is also supposed to be the first of course

		// go from the next node
		node=nodesList+firstEmptyNode+1;

		// iterate the entire list until we find the first empty node
		for (i=firstEmptyNode+1; i<nodesInList; i++) {
			// if this one is empty, go break
			if (node->empty) break;

			// go for the next one
			node++;
		}// for i

		// it's very nice, as it easily sets the variable with the
		// right number, and if the list is entirely occupied, it's
		// set equal to nodesInList, which is the value used for
		// telling that there are no empty nodes.
		firstEmptyNode=i;
		// mission complete!
	}

	// at last, let's update the statistic...
	// This statistic works with the supposition that nodes are always
	// added in the first empty node which can be found in the list.
	// This way, if lastActiveNode is incremented somewhere, it's so
	// because there are currently no gaps in the list, and so
	// lastActiveNode+1 is, at that moment only, the current amount of
	// nodes in the list. Later as we delete nodes in the middle of the
	// list, that value will only be meaningful as the maximum of
	// nodes until now, and that's exactly the way we use it.
	if (lastActiveNode!=nodesInList && lastActiveNode+1>maxNodesUntilNow)
		maxNodesUntilNow=lastActiveNode+1;

	return true;// all ok
}// activateNode

// This method empties a node, and also checks for other changes that
// are needed when some special nodes are emptied.
// Because of this, it's VERY important for the correct working of this
// class.  Hence, this method has to be called every time a node is
// removed.
template <class T>
bool vectorListClass<T>::removeNode(DWORD nodeNumber)
{
	vectorListNodeStruct<T>* node;

	if (!initialized) return false;// error!

	// empty the node
	nodesList[nodeNumber].empty=true;

	// now check for our special case: if we remove a node which is
	// before the firstEmptyNode, we need to update that variable...
	if (nodeNumber<firstEmptyNode) {
		// now it's the first empty node
		firstEmptyNode=nodeNumber;
	}

	// special case two: we are removing the lastActiveNode!
	if (nodeNumber==lastActiveNode) {
		// in such a case, we have to look for the first active node
		// before it... (that is, the first backwards!)
		// which is also supposed to be the last in the list of course

		// iterate the entire list until we find the first active node
		// backwards...
		for (node=nodesList+lastActiveNode-1;node>=nodesList; node--) {

			// if this one is active, go break
			if (!(node->empty)) break;
		}// for node

		// now it's time to check what has happened
		if (node<nodesList) {
			// no more active nodes...
			lastActiveNode=nodesInList;// means no one is active now
		}else{
			// set the right active node
			lastActiveNode=(node-nodesList);
		}
		// mission complete!
	}

	return true;// all ok
}// removeNode

// Used for starting a list traversal. Use together with getNode and
// getNextNode.
// It returns "nodesInList" if no more nodes can be found (to check with
// isValidIndex).
template <class T>
DWORD vectorListClass<T>::getIndexFirstNode()
{
	// if the first node is empty, check for the next ones...
	if (!(nodesList[0].empty)) return 0;
	return getNextNode(0);
}// getIndexFirstNode

// Used for starting a list traversal. Use together with getNextNode.
// It returns NULL if no more nodes can be found.
template <class T>
vectorListNodeStruct<T>* vectorListClass<T>::getFirstNode()
{
	// if the first node is empty, check for the next ones...
	if (!(nodesList[0].empty)) return nodesList;
	return getNextNode(nodesList);
}// getFirstNode

// Used for traversing the list, it just tells what's the next active
// node... use together with getNode and getIndexFirstNode.
// It returns "nodesInList" if no more nodes can be found (to check with
// isValidIndex).
template <class T>
DWORD vectorListClass<T>::getNextNode(DWORD since)
{
	vectorListNodeStruct<T> *node;

	for (node=nodesList+since+1;node<nodesList+nodesInList; node++) {

		// if this one is active, go break
		if (!(node->empty)) break;
	}// for node

	return node-nodesInList;
}// getNextNode

// Used for traversing the list, it just tells what's the next active
// node... use together getFirstNode.
// It returns NULL if no more nodes can be found.
template <class T>
vectorListNodeStruct<T>* vectorListClass<T>::getNextNode(vectorListNodeStruct<T> *since)
{
	for (since++;since<nodesList+nodesInList; since++) {

		// if this one is active, go break
		if (!(since->empty)) break;
	}// for node

	if (since>=nodesList+nodesInList) return NULL;
	return since;
}// getNextNode

// This method is used for adding a node to the list. Returns
// "nodesInList" if the node can't be added, otherwise it returns
// the index of the added node (which later can be used together with
// getNode)... The returned value can be checked with isValidIndex
// for error checking.
template <class T>
DWORD vectorListClass<T>::addNode(T& data)
{
	DWORD index;

	// remember it before the value changes
	index=firstEmptyNode;

	// return error if there is no more space in the list
	if (firstEmptyNode>=nodesInList) return nodesInList;

	// add the node...
	nodesList[firstEmptyNode].data=data;
	if (!activateNode(firstEmptyNode)) return nodesInList;// check for errors

	// return the index where the node was stored
	return index;
}// addNode

#endif// FILE_VECTORLIST_INCLUDED
