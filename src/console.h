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


#ifndef FILE_CONSOLE_INCLUDED
#define FILE_CONSOLE_INCLUDED

#define FILE_CONSOLE_VERSION "10-6-2002-H"// last modification: dd/mm/yy

#include "scrolltext.h"

class consoleClass{
private:
	// our scroll text object
	scrollTextClass scroll;

	bool initialized;

	// console data
	int maxChars;// max amount of chars + 1 for zero
	char* lastLine;// where to place the typed string
	int currentLetter;// pointer to the next byte to be written in lastLine
	char* textParam;// where to copy the text params
	int nextParam;// pointer to the next byte to be read
	bool lastLineBuffered;// tells if lastLine contains the current or old typed text

	// used when getting the params
	bool skipSpaces();
public:
	// some useful methods
	bool init(int width, int height, int maxChars=200);
	bool end();
	bool clear();
	bool keyUp(char key);
	bool draw(ddSurfaceClass& srf, int x, int y);
	bool writeLine(char* text);
	bool getTextParam(char* &param);
	bool getNoSpacedTextParam(char* &param);
	bool getIntegerParam(int &param);
	bool resetParamPointer();

	// methods for dealing with lost surfaces
	bool isLost(bool* errorCode=NULL);
	bool restore();

	// simple methods
	bool isReady() {return initialized;}

	// constructor / destructor
	consoleClass():initialized(false) {}
	~consoleClass() {end();}
};// consoleClass

#endif// FILE_CONSOLE_INCLUDED

