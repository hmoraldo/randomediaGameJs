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
TIMEWRAP.*:


These files are the time wrapper, that's a small time library with a
very useful extra feature: the posibility of pausing time, for example,
every time the window focus is lost, and to continue it when the focus
is back.

*/

#ifndef FILE_TIMEWRAP_INCLUDED
#define FILE_TIMEWRAP_INCLUDED

#define FILE_TIMEWRAP_VERSION "1-1-2003-H"// last modification: dd/mm/yy

// Include some useful header files
#include "win-constants.h"

// timeWrapperClass
// that's our class for time
class timeWrapperClass{
private:
	// some important flags
	bool initialized;
	bool paused;

	// info for the clock state
	// Time at which the clock is supposed to have been started
	DWORD startTime;
	// Time to be added to obtain the actual current time
	// (used because of the pause and "wrap-around" functions)
	DWORD plusTime;
	// This variable tells the value by which time since startTime has
	// to be multiplied. Normal state is 1, values bigger than 1 define
	// faster time resolutions, and values smaller than 1 define slower
	// time resolutions. For example, with multiplier=2, the time
	// tells 2 seconds for every real second.
	double multiplier;
protected:
public:
	timeWrapperClass():initialized(false) {}
	virtual ~timeWrapperClass() {end();}

	bool isReady() {return initialized;}
	bool isPaused() {return paused;}

	bool init();
	bool end();
	bool pause();
	bool reset();
	bool play();// let the clock continue working, after being paused...
	DWORD getTime(bool* errorCode=NULL);// returns the time in milliseconds
	double getMultiplier() {return multiplier;}
	bool setMultiplier(double newmul);
};

#endif// FILE_TIMEWRAP_INCLUDED

