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
GLOBALS.*:
(repository of global data)


This is our repository of global data. Here we store the variables which
have to be "global", as they are resources used everywhere in the program.

*/

#ifndef FILE_GLOBALS_INCLUDED
#define FILE_GLOBALS_INCLUDED

#define FILE_GLOBALS_VERSION "8-4-2002-H"// last modification: dd/mm/yy

// Include some useful header files
#include "types.h"
#include "ddwrap.h"
#include "diwrap.h"
/*#include "audiowrap.h"*/
#include "console.h"
#include "con-logger.h"
#include "mainloop.h"// for accessing to the main loop object
#include "timewrap.h"// for using the time wrapper class
#include "scriptman.h"// used for accessing to the script manager object

// Some variables to be used everywhere
extern languageEnum currentLanguage;// language currently selected
// Macro for language selection
// Example of use:
// string=LAN_SEL("Hola", "Hello");
#ifndef LAN_SEL_DEFINED
#define LAN_SEL_DEFINED
#define LAN_SEL(sp,en) ((char*)(currentLanguage==language_spanish?(sp):(en)))
#endif// LAN_SEL_DEFINED

// Some useful strings (at least it's useful to have them inside the
// executable file).
// Program compiled version info (a string containing such information)
extern char programVersionInfoString[];
// A string containing the copyright information.
extern char programCopyrightString[];
/*
// Data for the windows management
extern HWND hwnd;
extern HINSTANCE hInstance;
extern int nCmdShow;
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
*/
// Our directDraw object
extern directDrawClass DD;

// Our keyboard and mouse objects
extern diKeyboardClass keyboard;
extern diMouseClass mouse;

// Is the sound currently active? Global for the entire program.
// Notice this boolean doesn't tell if the sound is active or not in
// a given moment, it tells whether sound can or cannot be used
// during the entire execution of the program.
extern bool programSoundActive;
// This other boolean tells, instead, whether the sound could be
// used or not in a given moment, according to the current program
// configuration
extern bool programSoundEnabled;

// Data for the console
extern consoleClass console;
extern bool showConsole;
extern conLoggerClass logger;

// Our time object
extern timeWrapperClass timeWrapper;

// Frames per second
// This value is the amount of frames that have to be executed
// every second (executeFrame), without caring about the real amount of
// frames (drawFrame) drawn.
extern const int desiredFramesPerSecond;

// These consts store the max amount of frames which can be executed
// without drawing a frame on screen. ByTime is used together with
// desiredFramesPerSecond to know the real amount.
// Both are used to enable a mechanism which avoids program halts
// when executeFrame is very slow, then if the program executes too
// much frames at once, it'll go even slower, which will make the program
// execute even more frames, and so on until falling in a deadlock.
// To avoid this, we put this limit to the frames executed (so, if
// either executeFrame or drawFrame goes too much slow, the amount of
// frames skipped -executed- will stop being proportional to the time
// elapsed for every physic frame).
// This system has been tested and works very well.
extern const double maxExecutedFramesWithoutDrawingByTime;
extern const int maxExecutedFramesWithoutDrawing;
extern const int msecToCalculateFPS;



// Here follows some variables which store information related to the
// program itself.
extern mainLoopClass mainLoopObject;// this is the object used for executing the main program loop

// Program sections:
extern int currentProgramZone;// program zone the program is currently in
extern int currentProgramSubZone;// program sub zone the program is currently in
extern int currentProgramSubSubZone;// program sub sub zone the program is currently in
// Previous program sections:
extern int previousProgramZone;// previous program zone the program was in
extern int previousProgramSubZone;// previous program sub zone the program was in
extern int previousProgramSubSubZone;// previous program sub sub zone the program was in

// Some global functions related to the resources in globals.*.
extern bool initializeFirst();// initialize the first resources needed (on program start)
extern bool checkLostResources();// look for lost resources and take care of them
extern bool closeAll(char* errorText=NULL);// destroy all the opened resources
extern bool drawConsole();// shows the console on screen

// Some globals functions which modify the software state, whatever the
// loop in execution is.
bool globalExit();
bool globalSetActiveState(bool actived);

// Other useful global functions...
bool logDateAndTime(char* stringToUse);// a function to be used to log texts together with the current date and time

#endif// FILE_GLOBALS_INCLUDED
