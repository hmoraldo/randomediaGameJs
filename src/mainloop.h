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
MAINLOOP.*:

Here goes the code which interfaces directly with the loop wrapper, and
which decides what kind of frame is going to be executed and drawn next
(game frame, intro frame, etc).

*/

#ifndef FILE_MAINLOOP_INCLUDED
#define FILE_MAINLOOP_INCLUDED

#define FILE_MAINLOOP_VERSION "14-7-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "win-constants.h"
#include "loopwrap.h"// for generic frame loops code

// This is the class which manages the main loop for the program.
// The code related to the frames management is actually in the
// loopWrapperClass definition.
class mainLoopClass:public loopWrapperClass{
private:
protected:
public:
	// constructors / destructors
	mainLoopClass() {}
	virtual ~mainLoopClass() {}

	// Methods which are pure virtual at the base class
	virtual bool executeFrame(DWORD frameNumber);// method called once per frame
	virtual bool drawFrame();// method called every time a frame has to be drawn in the screen
	virtual bool beforeLoop();// method called just before starting the main loop
	virtual bool afterLoop();// method called just after finishing with the main loop
	virtual bool informFramesPerSecond(double fps);// method called every time the frames por second information is calculated, in a timely basis

#ifdef EMSCRIPTEN
	virtual bool executeMainLoop(int framesToExecutePerSecond, int maxExecutedFramesWithoutDrawing, DWORD msecToCalculateFPS);
#endif

};// mainLoopClass

#endif// FILE_MAINLOOP_INCLUDED
