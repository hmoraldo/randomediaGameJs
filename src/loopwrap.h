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
LOOPWRAP.*:


These files contain the generic code to be used in frame loops which
require independence from the computer speed (measured in frames per
second). Through C++ inheritance, these files allows to implement such
frame loops just by derivating classes from loopWrapperClass.

*/

#ifndef FILE_LOOPWRAP_INCLUDED
#define FILE_LOOPWRAP_INCLUDED

#define FILE_LOOPWRAP_VERSION "7-6-2002-H"// last modification: dd/mm/yy

#include "timewrap.h"// for using the time wrapper class

// loopWrapperClass
// that's our class for generic frame loops code.
class loopWrapperClass{
public:
	// constructors / destructors
	loopWrapperClass() {}
	virtual ~loopWrapperClass() {end();}

	// loop management methods
	virtual bool executeMainLoop(int desiredFramesPerSecond, int maxExecutedFramesWithoutDrawing, DWORD msecToCalculateFPS);
	virtual bool end() {exitLoop(); return true;}

	// some simple interfaces
	virtual void exitLoop() {externalEnded=true;}
	virtual bool isLoopActive() {return activeLoop;}
	virtual void setActiveState(bool active) {activeLoop=active;}
	virtual timeWrapperClass* getTimerObject() {return &timer;}
	virtual int getFramesToExecutePerSecond() {return framesToExecutePerSecond;}

	// Methods to be inherited
	// All of them have to return true if all went right, otherwise, false
	// for fatal errors.
	virtual bool executeFrame(DWORD frameNumber)=0;// method called once per frame
	virtual bool drawFrame()=0;// method called every time a frame has to be drawn in the screen
	virtual bool beforeLoop()=0;// method called just before starting the main loop
	virtual bool afterLoop()=0;// method called just after finishing with the main loop
	virtual bool informFramesPerSecond(double fps)=0;// method called every time the frames por second information is calculated, in a timely basis

protected:
	// main loop helpers
	bool loopCycle(int framesToExecutePerSecond, int maxExecutedFramesWithoutDrawing, DWORD msecToCalculateFPS, bool* done);
	bool setupMainLoop(int framesToExecutePerSecond);

private:
	// internal timer object
	timeWrapperClass timer;

	// stores the amount of frames to be executed every second
	int framesToExecutePerSecond;

	volatile bool externalEnded;// to know when it's time to stop the main loop
	volatile bool activeLoop;// to know when it's time to pause the main loop, for reasons like the app losing the user focus

	// hacky: some variables to share state between loopCycle and executeMainLoop
	DWORD timeInit, frames;
	DWORD lastFrameNumber, lastFrameTime;
	DWORD framesExecuted;

};// loopWrapperClass

#endif// FILE_LOOPWRAP_INCLUDED

