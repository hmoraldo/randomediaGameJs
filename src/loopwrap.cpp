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


#include "loopwrap.h"// for generic frame loops code

// Methods for managing the loop

bool loopWrapperClass::loopCycle(int framesToExecutePerSecond, int maxExecutedFramesWithoutDrawing, DWORD msecToCalculateFPS, bool* done) {
	// declare some variables...
	int i;// for repetitive control structures

	// some other for measuring the frames per second
	DWORD timeEnd;

	// some other for maintaining a fixed "virtual" frame rate
	DWORD frameNumber, frameTime;
	int skippedFrames;

	// draw the current frame
	if (!drawFrame()) return false;

	// continue the timer, even if it wasn't paused
	timer.play();

	// Check desired fps and execute the necessary amount of frames...
	// Check desired FPS and calculate the frames to "skip" (execute)
	frameTime=timer.getTime();
	frameNumber=(framesToExecutePerSecond*frameTime)/1000;
	skippedFrames=(int)(frameNumber-lastFrameNumber);
	// There is a maximum for skippedFrames, to avoid time deadlocks...
	if (skippedFrames>maxExecutedFramesWithoutDrawing) {
		skippedFrames = maxExecutedFramesWithoutDrawing;
	}

	// Execute the frames
	for (i=0; i<skippedFrames; i++) {
		// execute a virtual frame
		if (!executeFrame(framesExecuted)) return false;
		framesExecuted++;
	}// for i
	lastFrameNumber=frameNumber;
	lastFrameTime=frameTime;

	// check the frames per second, and inform that value...
	frames++;
	timeEnd=frameTime;
	if (timeEnd-timeInit>msecToCalculateFPS) {

		if (!informFramesPerSecond(
			double(frames)/(double(timeEnd-timeInit)/1000.0)))
			return false;

		// reset it all
		timeInit=timer.getTime();
		frames=0;
	}

	// while there are messages to get, dispatch them
/*	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)  // exit main loop on WM_QUIT
			(*done)=true;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}// while peeking

*/
	// pause the timer if our app isn't active...
	if (!activeLoop) timer.pause();

/*	// if our app isn't active, keep dispatching messages only
	// (unless it's execution has ended)
	while (!activeLoop && !(*done)) {
		GetMessage(&msg, NULL, 0, 0);

		if (msg.message == WM_QUIT) {  // exit main loop on WM_QUIT
			(*done)=true;
		}


		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/
	// Exit the loop if necessary
	if (externalEnded) (*done)=true;

	// all ok
	return true;
}

bool loopWrapperClass::setupMainLoop(int framesToExecutePerSecond) {
	// initialize some useful variables...
	loopWrapperClass::framesToExecutePerSecond=framesToExecutePerSecond;
	externalEnded=false;
	activeLoop=true;
	if (!timer.isReady()) timer.init();// the timer too

	// get the timer for the first time
	timeInit=timer.getTime();

	// For measuring frames per second only
	frames=0;

	// The frames for us, drawFrame and executeFrame start from zero
	// every time this method is called
	framesExecuted=0;

	// set all for the independant frame rate
	lastFrameTime=timeInit;// timer.getTime()
	// current frame number, actually
	lastFrameNumber=(framesToExecutePerSecond*lastFrameTime)/1000;

	// execute what's required before the first frame
	if (!beforeLoop()) return false;

	return true;
}

// executeMainLoop *
// This method is used to execute the loop itself. It's supposed to return
// only when the loop has ended. If it returns false, it means there has
// been a fatal error during the execution...
// Params are:
// framesToExecutePerSecond: a value telling how many frames how to be
// "executed" per second.
// maxExecutedFramesWithoutDrawing: value used to enable a mechanism which
// avoids program halts when executeFrame is very slow. In that case, if
// the program executes too much frames at once, it'll go even slower, which
// will make the program execute even more frames, and so on until falling
// in a deadlock.
// To avoid this, we put this limit to the frames executed (so, if
// either executeFrame or drawFrame goes too much slow, the amount of
// frames skipped -executed- will stop being proportional to the time
// elapsed for every physic frame).
// This system has been tested and works very well. Values of 1 or 2 seconds
// are usual.
// msecToCalculateFPS: tells how many milliseconds have to pass before
// informFramesPerSecond() is called.
bool loopWrapperClass::executeMainLoop(
	int framesToExecutePerSecond,// This value is the amount of frames that have to be executed every second (executeFrame), without caring about the real amount of frames (drawFrame) drawn.
	int maxExecutedFramesWithoutDrawing,// see above
	DWORD msecToCalculateFPS)// how many milliseconds have to pass before informing the current amount of frames per second
{
	bool ended=false;

	if (!setupMainLoop(framesToExecutePerSecond)) return false;

	while(!ended) {
		if (!loopCycle(framesToExecutePerSecond, maxExecutedFramesWithoutDrawing, msecToCalculateFPS, &ended)) {
			return false;
		}
	}

	if (!afterLoop()) return false;

	return true;
}// executeMainLoop
