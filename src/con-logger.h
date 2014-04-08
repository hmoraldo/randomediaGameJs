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


#ifndef FILE_CONLOGGER_INCLUDED
#define FILE_CONLOGGER_INCLUDED

#define FILE_CONLOGGER_VERSION "10-6-2002-H"// last modification: dd/mm/yy

#include "console.h"

class conLoggerClass{
private:
	bool initialized;

	char* fileName;// name of the selected file
	char* strBuffer;// buffer for the conversion to string Z
	int strBufferSize;// size of strBuffer
	consoleClass* conPtr;// pointer to a console to log to
	bool fileReady;// was a file selected?

	// some logging private methods
	bool logToFile(char* text, bool noUnselError=true);
	bool logToConsole(char* text, bool noUnselError=true);
public:
	// some useful methods
	bool init(char* name=NULL,
		consoleClass* console=NULL, int bytesBufferName=100,
		int bytesStrBuffer=200);
	bool end();
	bool attachConsole(consoleClass* console);
	bool selectFile(char* name);
	bool logLine(char* text, bool file=true, bool console=true,
		bool noUnselError=true, ...);

	// constructor / destructor
	conLoggerClass():initialized(false) {}
	~conLoggerClass() {end();}
};// conLoggerClass

#endif// FILE_CONLOGGER_INCLUDED
