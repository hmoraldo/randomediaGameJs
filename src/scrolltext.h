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


#ifndef FILE_SCROLLTEXT_INCLUDED
#define FILE_SCROLLTEXT_INCLUDED

#define FILE_SCROLLTEXT_VERSION "23-5-2002-H"// last modification: dd/mm/yy

#include "ddwrap.h"

class scrollTextClass {
private:
	bool initialized;
//	ddSpriteClass surface;

	int r, g, b;// color of the text
	int rB, gB, bB;// color of the background
	int fontHeight, numLines;// numLines: amount of lines
public:
	// main methods
	bool init(int width, int height, int r, int g, int b, int rB, int gB, int bB, bool videoMemory=false);
	bool end();
	bool scrollLines(int lines);
	bool writeLatestLine(char* text);
	bool blt(ddSurfaceClass& srf, int x, int y);
	bool clearSurface();

	// methods to manage lost surfaces
	bool isLost(bool* errorCode=NULL);
	bool restore();

	// constructor / destructor
	scrollTextClass():initialized(false) {}
	~scrollTextClass() {end();}

	// simple interfaces
	// most are self explaining
	bool isReady() {return initialized;}
	int getFontHeight() {return fontHeight;}
	int getNumLines() {return numLines;}// get the amount of lines
//	ddSpriteClass* getSurfacePointer() {return &surface;}// use with care
	void getTextColor(int& rT, int& gT, int& bT) {rT=r; gT=g; bT=b;}
	void getBackgroundColor(int& rB, int& gB, int& bB)
	{
		rB=scrollTextClass::rB;
		gB=scrollTextClass::g;
		bB=scrollTextClass::b;
	}// getBackgroundColor
};

#endif// FILE_SCROLLTEXT_INCLUDED
