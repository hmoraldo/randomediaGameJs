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
PROGRAMINFO.*:


This file contains general information about the program itself, and
the main defines for the conditional compilation of this program in its
many modes (demo on off, debug on off, etcetera).

Use the following documented line:
#include "programinfo.h"// for general info about the program itself
for including.
*/

#ifndef FILE_PROGRAMINFO_INCLUDED
#define FILE_PROGRAMINFO_INCLUDED

#define FILE_PROGRAMINFO_VERSION "1-11-2002-H"// last modification: dd/mm/yy



// MAIN DEFINES FOR CONDITIONAL COMPILATION
// (All of them, called as PROGMODE_*)

// Define used for defining the current compilation mode as
// illustrations demo, or real game.
#undef PROGMODE_SCREENSHOTS_DEMO_YES
//#define PROGMODE_SCREENSHOTS_DEMO_YES// comment / uncomment as needed
#ifndef PROGMODE_SCREENSHOTS_DEMO_YES
	#define PROGMODE_SCREENSHOTS_DEMO_NO
#endif// PROGMODE_SCREENSHOTS_DEMO_YES

// Define used for defining the current compilation mode as demo program,
// or final release.
#undef PROGMODE_DEMO_YES
//#define PROGMODE_DEMO_YES// comment / uncomment as needed
#ifndef PROGMODE_DEMO_YES
	#define PROGMODE_DEMO_NO
#endif// PROGMODE_DEMO_YES

// Define used for defining the current compilation mode as developer
// mode (internal version), or common compilation for the users.
// Useful for enabling developer's only features in the internal version
// of this software.
// Disable this one for the final version (either in demo or release
// mode).
#undef PROGMODE_DEVELOPER_YES
#define PROGMODE_DEVELOPER_YES// comment / uncomment as needed
#ifndef PROGMODE_DEVELOPER_YES
	#define PROGMODE_DEVELOPER_NO
#endif// PROGMODE_DEVELOPER_YES

// Define used for defining the language active by default
#undef PROGMODE_DEFAULTLANGUAGE_SPANISH
#define PROGMODE_DEFAULTLANGUAGE_SPANISH// comment / uncomment as needed
#ifndef PROGMODE_DEFAULTLANGUAGE_SPANISH
	#define PROGMODE_DEFAULTLANGUAGE_ENGLISH
#endif// PROGMODE_DEFAULTLANGUAGE_SPANISH

// DEFINES OF USEFUL INFORMATION
// (All of them, called as PROGINFO_*).

// Executable code (identifying the program)
#define PROGINFO_PROGCODE "CRedia"

// Copyright of the program
#define PROGINFO_COPYRIGHT "Copyright 2002 - 2003 by H. Hernán Moraldo, all rights reserved"

// Version of the program
#define PROGINFO_VERSIONTEXT "Ver. 1.01"

// Program compilation date and time
#define PROGINFO_COMPILATIONTIME __DATE__ " - " __TIME__

// Program version full information
#define PROGINFO_FULLVERSIONTEXT PROGINFO_PROGCODE " - " PROGINFO_VERSIONTEXT " - " PROGINFO_COMPILATIONTIME

// Name for the window class (see main.cpp)
#define PROGINFO_WINCLASSNAME "class453CREH"

// Text for the program window
#define PROGINFO_WINDOWTEXT "Concepto Randomedia"


#endif// FILE_PROGRAMINFO_INCLUDED
