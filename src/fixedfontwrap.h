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
FIXEDFONTWRAP.*:


These files contain the code which manages loading and drawing fixed
bitmapped fonts to a surface.

*/

#ifndef FILE_FIXEDFONTWRAP_INCLUDED
#define FILE_FIXEDFONTWRAP_INCLUDED

#define FILE_FIXEDFONTWRAP_VERSION "6-8-2003-H"// last modification: dd/mm/yy

#include "ddwrap.h"

// characterStruct **********
struct characterStruct {
	int x, y;// position of a character in the font surface
	bool isAllowed;// tells whether the character is available for this font, or not
};// characterStruct

// fixedFontClass ***********
class fixedFontClass{
private:
protected:
	bool initialized;// was this font object initialized?

	int charW, charH;// width and height of the characters in this font

	// important protected methods implemented here
	virtual bool calculateTextLine(char* text, int textSize, int maxWidth, char** startOfLine, int* lineSize);

	// important methods to be used by the class itself, implemented in
	// the subclasses
	virtual bool lowLevelDrawTextLine(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y,	int xClip, int yClip, int wClip, int hClip, bool transparent)=0;
	virtual bool privateDrawTextLine(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int boxWidth, int boxHeight, bool transparent, alignEnum alignment=FILE_DDWRAP_ALIGN_LEFT, bool drawText=true, int* definitiveX=NULL, int* definitiveY=NULL, int* definitiveW=NULL, int* definitiveH=NULL);
	virtual bool privateDrawMultilineText(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int boxWidth, int boxHeight, bool transparent, alignEnum alignment=FILE_DDWRAP_ALIGN_LEFT, bool drawText=true, int* definitiveX=NULL, int* definitiveY=NULL, int* definitiveW=NULL, int* definitiveH=NULL);
public:
	// constructors / destructors
	fixedFontClass():initialized(false) {}
	virtual ~fixedFontClass() {}

	// initialization / deinitialization
	virtual bool end()=0;

	// simple interfaces
	virtual bool isReady() {return initialized;}
	virtual int getCharWidth() {return charW;}
	virtual int getCharHeight() {return charH;}

	// important methods
	virtual bool drawTextLine(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int boxWidth, int boxHeight, bool transparent, alignEnum alignment=FILE_DDWRAP_ALIGN_LEFT) {
		return privateDrawTextLine(srfTo, text, textSize, x, y, boxWidth, boxHeight, transparent, alignment);
	}// drawTextLine
	virtual bool calculateMultilineTextSize(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int boxWidth, int boxHeight, alignEnum alignment=FILE_DDWRAP_ALIGN_LEFT, valignEnum verticalAlignment=FILE_DDWRAP_VALIGN_TOP, int* definitiveX=NULL, int* definitiveY=NULL, int* definitiveW=NULL, int* definitiveH=NULL);
	virtual bool drawMultilineText(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int boxWidth, int boxHeight, bool transparent, alignEnum alignment=FILE_DDWRAP_ALIGN_LEFT, valignEnum verticalAlignment=FILE_DDWRAP_VALIGN_TOP);
};// fixedFontClass


// bitmappedFixedFontClass ***********
class bitmappedFixedFontClass:public fixedFontClass{
private:
protected:
	characterStruct* characters;// array storing the position of every character in the surface attached to a font object

	ddSpriteClass* surface;// surface which stores the image of the font

	// important methods to be used by the class itself
	virtual bool lowLevelDrawTextLine(ddSurfaceClass& srfTo, char* text, int textSize, int x, int y, int xClip, int yClip, int wClip, int hClip, bool transparent);
public:
	// constructors / destructors
	bitmappedFixedFontClass() {}
	virtual ~bitmappedFixedFontClass() {end();}

	// initialization / deinitialization
	virtual bool init(ddSpriteClass* fontSurface, int charWidth, int charHeight);
	virtual bool end();

	// simple interfaces
	characterStruct* getCharacters() {return characters;}

	// important methods
	bool setUpCharactersInMatrix(char* string, int xMat, int yMat, int matWidthChars, int hSpace, int vSpace);
};// bitmappedFixedFontClass


#endif // FILE_FIXEDFONTWRAP_INCLUDED
