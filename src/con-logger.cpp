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


// Con-logger is used for logging, both to a file and to a "console.*"
// console (if there is any).

#include "con-logger.h"
#include <string.h>// for strcpy
#include <stdio.h>// for files
#include <stdarg.h>// for multiple arguments and _vsnprintf

// initializes the conLogger object
bool conLoggerClass::init(
	char* name,// name of the file to select, null for no one yet
	consoleClass* console,// pointer to the console object, null for no one yet
	int bytesBufferName,// amount of bytes to be reserved for the file name
	int bytesStrBuffer)// amount of bytes to be reserved for the string buffer
{
/*	if (initialized) return false;

	// initialize the data
	fileReady=false;
	fileName=new char[bytesBufferName];
	strBufferSize=bytesStrBuffer;
	strBuffer=new char[strBufferSize];

	// set the flag before using selectFile and attachConsole
	initialized=true;

	if (!selectFile(name) || !attachConsole(console)) {
		// something failed
		delete[] fileName;
		delete[] strBuffer;
		initialized=false;
		return false;
	}
*/
	return true;// all ok
}// init

// end the use of this object
bool conLoggerClass::end()
{
/*	if (!initialized) return false;

	// time to free this
	delete[] fileName;
	delete[] strBuffer;
	initialized=false;
*/
	return true;// all ok
}// end

// attaches a console for logging
bool conLoggerClass::attachConsole(consoleClass* console)
{
/*	if (!initialized) return false;

	// attach the console itself
	conPtr=console;
*/
	return true;// all ok
}// attachConsole

// select a file where to log in
// if name is NULL, it selects nothing
bool conLoggerClass::selectFile(char* name)
{
/*	if (!initialized) return false;

	if (name!=NULL) {
		strcpy(fileName, name);
		fileReady=true;
	}else
		fileReady=false;// select nothing
*/
	return true;// all ok
}// selectFile

// Log a text line to the selected file.
// If there is no selected file, return error only if noUnselError
// is false (meaning it has to be seen as an error).
bool conLoggerClass::logToFile(char* text, bool noUnselError)
{
/*	FILE* file;

	if (!initialized) return false;

	// is a file selected
	if (!fileReady) {
		// we won't be able to write anything...
		// should we return an error code?
		if (noUnselError) return true;// no
		return false;// yes
	}

	// open for writing, binary appending
	file=fopen(fileName, "a+b");
	if (file==NULL) {
		return false;// failed the opening
	}

	// write the text...
	fwrite(text, 1, strlen(text), file);

	// write the newline
	fwrite("\x00D\x00A", 1, 2, file);

	// time to close the file
	fclose(file);
*/
	return true;// all ok
}// logToFile

// Log a text line to the selected console.
// If there is no selected console, return error only if noUnselError
// is false (meaning it has to be seen as an error).
bool conLoggerClass::logToConsole(char* text, bool noUnselError)
{
/*	if (!initialized) return false;

	// is a console attached?
	if (conPtr==NULL) {
		// we won't be able to write anything...
		// should we return an error code?
		if (noUnselError) return true;// no
		return false;// yes
	}

	// now log our text
	if (!conPtr->writeLine(text)) return false;
*/
	return true;// all ok
}// logToConsole

// Log a text line to the selected console or file (or both, or no one).
// If there is no selected console or file, return error only if
// noUnselError is false (meaning it has to be seen as an error).
bool conLoggerClass::logLine(
	char* text,// text to log
	bool file,// log to file?
	bool console,// log to console?
	bool noUnselError,// return an error if those are unselected?
	...// that's for the printf like params
) {
	va_list paramList;
	va_start(paramList, noUnselError);
	vprintf(text, paramList);// NEW 2010
	printf("\n");
	va_end(paramList);

/*	bool failed=false;
	va_list paramList;

	if (!initialized) return false;

	// get the first param of the list
	va_start(paramList, noUnselError);

	// let's compose the string
	// We decrement strBufferSize in one, because
	// we want _vsnprintf to keep a byte for the 0.
	// I guess it happens because, for vsnprintf, the last zero is
	// wrote only if it's able to, without taking it as an error if it's
	// not able to do it.
	if (_vsnprintf(
		strBuffer,// storage for the final string
		strBufferSize-1,// size of the buffer (keeping a space for the 0)
		text,// format string
		paramList// list of parameters
		)<0) return false;// it means error

	// add the zero if necessary
	strBuffer[strBufferSize-1]=0;

	// we won't use the param list anymore
	va_end(paramList);

	// log to console first
	if (console) {
		if (!logToConsole(strBuffer, noUnselError)) failed=true;
	}

	// log to file
	if (file) {
		if (!logToFile(strBuffer, noUnselError)) failed=true;
	}

	// did it fail?
	if (failed) return false;
*/
	return true;// all ok
}// logLine
