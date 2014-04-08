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


#include "timewrap.h"// for using the time wrapper class
#include <SDL.h>

#define MAX_TICKS 4294967295 // DWORD max-1= (2^32) -1

// init ***************
// Initialize the class and reset the clock
bool timeWrapperClass::init()
{
	if (isReady()) return false;

	// initialize all
	initialized=true;
	paused=false;
	multiplier=1;// normal value

	// now reset the clock
	if (!reset()) {
		// error!
		initialized=false;
		return false;
	}

	return true;// all ok
}// init

// end ****************
// End the class...
bool timeWrapperClass::end()
{
	if (!isReady()) return false;

	// finalize it...
	initialized=false;

	return true;// all ok
}// end

// reset ***************
// Reset our internal "clock"...
// It doesn't change the paused state
bool timeWrapperClass::reset()
{
	if (!isReady()) return false;

	// reset it
	startTime=SDL_GetTicks();
	plusTime=0;
	multiplier=1;// normal value

	return true;// all ok
}// reset

// pause ****************
// Pause the clock for a while.
bool timeWrapperClass::pause()
{
	if (!isReady()) return false;
	if (paused) return true;// no need to pause it again

	// pause the clock...
	// It works because the way the "play" method is supposed to work...
	plusTime=getTime();

	// set our flag
	paused=true;

	return true;// all ok
}// pause

// play *****************
// Continue after being paused...
bool timeWrapperClass::play()
{
	if (!isReady()) return false;
	if (!paused) return true;// no need to play it if it's not paused

	// continue from where it was paused
	// It works because the way the "pause" and "getTime" methods are
	// supposed to work...
	startTime=SDL_GetTicks();

	// clear our flag
	paused=false;

	return true;// all ok
}// play

// setMultiplier *********
// Sets a new value for the multiplier (to change the speed of time).
bool timeWrapperClass::setMultiplier(double newmul) {
	// Check for errors...
	if (newmul<=0) return false;

	// make the clock start from this instant (to avoid changing the
	// speed of the time which has already passed)...
	// It works because the way the "getTime" method is supposed
	// to work...
	plusTime=getTime();
	startTime=SDL_GetTicks();

	// set the multiplier
	multiplier=newmul;

	return true;// all ok
}// setMultiplier

// returns the time in milliseconds
DWORD timeWrapperClass::getTime(bool* errorCode)
{
	DWORD tickCount;
	DWORD tempTime;

	if (!isReady()) {
		// error!
		if (errorCode!=NULL) *errorCode=false;
		return 0;
	}

	// check for the wrap around zero "feature" of getTickCount
	tickCount=SDL_GetTicks();
	if (tickCount<startTime) {
		// it has wrapped around zero, avoid errors by updating the
		// clock rightfully
		// With no multiplier, it would be:
		// plusTime=MAX_TICKS-startTime+1;
		plusTime=(DWORD)((MAX_TICKS-startTime-plusTime+1)*multiplier)
			+plusTime;
		startTime=0;
	}

	if (paused) {
		// when the clock is paused, we return always the same time
		tempTime=plusTime;
	}else{
		// get the current time (considering the multiplier value)
		// That is:
		// (tickCount-startTime): the time since the latest multiplier
		// was set
		// (tickCount-startTime)*multiplier: the time since the latest
		// multiplier was set, in such time speed
		// +plusTime: the value for initial time we use
		tempTime=(DWORD)((tickCount-startTime)*multiplier)+plusTime;
	}

	// all ok
	if (errorCode!=NULL) *errorCode=true;

	return tempTime;
}// getTime

