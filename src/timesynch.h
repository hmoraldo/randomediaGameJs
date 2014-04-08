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
TIMESYNCH.*:


These files contain the code which helps to manage the
synchronization with time. It's specially useful for making graphics and
sound go together.

Use the following documented line:
#include "timesynch.h"// for managing the synchronization with time
for including.

*/

#ifndef FILE_TIMESYNCH_INCLUDED
#define FILE_TIMESYNCH_INCLUDED

#include <windows.h>// for defining those common types, like DWORD, etc

#define FILE_TIMESYNCH_VERSION "18-7-2002-H"// last modification: dd/mm/yy

// These are the possible types for a timeSynchEvent.
enum timeSynchTypesEnum {
	FILE_TIMESYNCH_INSTANT,// the event is placed at an exact instant
	FILE_TIMESYNCH_RANGEAROUND,// the event size is distributed both at left and right
	FILE_TIMESYNCH_RANGEFORWARD,// the event size is distributed only after the event position
	FILE_TIMESYNCH_RANGEBACKWARD// the event size is distributed only before the event position
};// timeSynchTypesEnum

// This struct stores all the data related to a given event.
template <class extraInfoStruct>
struct timeSynchEventStruct {
	// original data
	DWORD timePosition;// time at which the event is placed, in mseconds.
	DWORD rangeSize;// maximum distance in time at which the event is executed
	timeSynchTypesEnum type;// type of event (more exactly, way the range size is distributed around the event position)
	extraInfoStruct extraInfo;// extra information about the event, to be used only by the user
	bool active;// tells whether this event can executed or not

	// data updated by calculateTimeSynchData
	bool isTimeToExecuteIt;// tells whether it's time to execute this event or not
};// timeSynchDataStruct

template <class eIS> // extraInfoStruct=eIS
bool analyzeTimeSynchData(DWORD currentTime, timeSynchEventStruct<eIS>* eventsList, int listSize, timeSynchEventStruct<eIS>** firstExecutedEvent=NULL);


// FUNCTIONS DEFINITIONS:
// analyzeTimeSynchData ****
// This function analyzes a list of time synch events and mark for
// execution those which are ready for that, according to their
// properties.
template <class eIS> // extraInfoStruct=eIS
bool analyzeTimeSynchData(
	DWORD currentTime,// current time
	timeSynchEventStruct<eIS>* eventsList,// array of listSize elements, each containing an event information
	int listSize,// size for eventsList, in elements
	DWORD* firstExecutedEvent=NULL)// if it's not NULL, analyzeTimeSynchData stores the index for the first element in eventsList which has been marked for execution. If no event has been executed, analyzeTimeSynchData sets it to listSize
{
	int i;// for loops
	timeSynchEventStruct<eIS>& tEv=eventsList[0];// by This Event
	DWORD firstExeNoParam;// temp value for firstExecutedEvent

	// initialize firstExeNoParam to the default "no execution" value
	firstExeNoParam=listSize+1;

	// for every event in the list
	for (i=0; i<listSize; i++) {
		tEv=eventsList[i];// point "This Event" to it

		// don't even consider inactive events
		if (!tEv.active) continue;

		// according to its type, see whether it's time to execute
		// it or not
		switch (tEv.type) {
		case FILE_TIMESYNCH_INSTANT:// the event is placed at an exact instant
			tEv.isTimeToExecuteIt=
				(currentTime==tEv.timePosition);
			break;
		case FILE_TIMESYNCH_RANGEAROUND:// the event size is distributed both at left and right
			tEv.isTimeToExecuteIt=
				(currentTime>=tEv.timePosition-(tEv.rangeSize/2)) &&
				(currentTime<=tEv.timePosition+(tEv.rangeSize/2));
			break;
		case FILE_TIMESYNCH_RANGEFORWARD:// the event size is distributed only after the event position
			tEv.isTimeToExecuteIt=
				(currentTime>=tEv.timePosition) &&
				(currentTime<=tEv.timePosition+tEv.rangeSize);
			break;
		case FILE_TIMESYNCH_RANGEBACKWARD:// the event size is distributed only before the event position			}
			tEv.isTimeToExecuteIt=
				(currentTime>=tEv.timePosition-tEv.rangeSize) &&
				(currentTime<=tEv.timePosition);
			break;
		}

		// if firstExeNoParam wasn't updated yet, and this event has been
		// marked for execution, firstExeNoParam have to point to this
		// event
		if (tEv.isTimeToExecuteIt && int(firstExeNoParam)==listSize+1) {
			firstExeNoParam=i;
		}
	}


	// return firstExecutedEvent if asked to...
	if (firstExecutedEvent!=NULL) {
		*firstExecutedEvent=firstExeNoParam;
	}

	return true;
}// analyzeTimeSynchData



#endif// FILE_TIMESYNCH_INCLUDED

