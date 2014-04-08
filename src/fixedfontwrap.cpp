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


#include "fixedfontwrap.h"// for creating and displaying fixed bitmapped fonts
#include "generalmaths.h"// for mathematical functions


// ****************
// FIXED FONT CLASS
// ****************

// Protected and private methods:
// ==============================

// calculateTextLine ********
// According to the font characteristics, this method separates the first
// line of text it finds in a given multiline text, while keeping
// the characters which determine line break (0xA and 0xD) out of sight.
bool fixedFontClass::calculateTextLine(
	char* text,// string where to find the line
	int textSize,// string size
	int maxWidth,// max width for the text, in pixels
	char** startOfLine,// pointer to a char* which will point to the beginning of the next line of text, after executing this method
	int* lineSize)// pointer to an integer which will contain the size of the line starting in *startOfLine, after executing this method
{
	bool endOfLoop=false;// this will tell us when we finished the calculation
	int maxWidthInChars=maxWidth/charW;
	char* thisChar=text;
	char* lastAcceptableEnd;// pointer to the char next to the latest character for this line
	char* tmpStartOfLine;

	// avoid all the initial spaces...
	while (thisChar-text<textSize && *thisChar==' ') thisChar++;
	tmpStartOfLine=thisChar;
	lastAcceptableEnd=thisChar;

	// for every word...
	do{
		// check whether it's time to jump out of the text
		if (thisChar-text>=textSize ||
			*thisChar==0xA ||
			*thisChar==0xD) {

			lastAcceptableEnd=thisChar;
			endOfLoop=true;
		}

		if (!endOfLoop) {
			// look for the next non space character
			while (thisChar-text<textSize && *thisChar!=' ' && *thisChar!=0xA && *thisChar!=0xD)
				thisChar++;

			// check if this word fits in the text box
			if (thisChar-tmpStartOfLine>maxWidthInChars) {
				// It doesn't fit... it's time to decide whether to
				// partition it or not (only partition it if
				// real necessary)
				if (lastAcceptableEnd==tmpStartOfLine) {
					// it's a word wider than the text box... draw it
					// in parts
					lastAcceptableEnd=tmpStartOfLine+maxWidthInChars;
				}

				// in any case, it's time to end the loop
				endOfLoop=true;
			}

			if (!endOfLoop) {
				// if it fitted, let's considerate this word into the
				// accepted line until now
				lastAcceptableEnd=thisChar;

				// avoid all the spaces...
				while (thisChar-text<textSize && *thisChar==' ') thisChar++;
			}
		}
	}while (!endOfLoop);

	// save the values to return
	(*startOfLine)=tmpStartOfLine;
	(*lineSize)=lastAcceptableEnd-tmpStartOfLine;

	return true;// all ok
}// calculateNextLine

// privateDrawTextLine *************
// This method is used to draw a line of text in a given surface,
// with any alignment, or to calculate the size of such a text line...
// private method to be used only by the class itself
bool fixedFontClass::privateDrawTextLine(
	ddSurfaceClass& srfTo,// surface where to draw the text
	char* text,// text to draw
	int textSize,// size of the string to draw
	int x, int y,// position where to draw the text
	int boxWidth, int boxHeight,// size of the text box (used for clipping)
	bool transparent,// will the text be transparent?
	alignEnum alignment,// text alignment, either FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER or FILE_DDWRAP_ALIGN_RIGHT
	bool drawText,// tells whether we'll draw the text, or only calculate it
	int* definitiveX, int* definitiveY,// pointer to variables where to return the actual x and y coordinates (and box size) where the text was placed... if null, they aren't used
	int* definitiveW, int* definitiveH)
{
	int realX, realY;// coordinates of the text, after aligned
	int textWidth=textSize*charW;// width of the text to draw

	// assume left alignment...
	realX=x; realY=y;

	// calculate the text coordinates after aligned, but only if
	// text is not wider than the text box
	if (textWidth<=boxWidth) {
		switch (alignment) {
		case FILE_DDWRAP_ALIGN_LEFT:// left align
		default:
			realX=x;
			realY=y;
			break;
		case FILE_DDWRAP_ALIGN_CENTER:// center align
			realX=(boxWidth-textWidth)/2+x;
			realY=y;
			break;
		case FILE_DDWRAP_ALIGN_RIGHT:// right align
			realX=x+(boxWidth-textWidth);
			realY=y;
			break;
		}
	}

	// fill the definitive coordinates of the text, to be returned
	if (definitiveX!=NULL) (*definitiveX)=realX;
	if (definitiveY!=NULL) (*definitiveY)=realY;
	if (definitiveW!=NULL) (*definitiveW)=MIN(textWidth, boxWidth);
	if (definitiveH!=NULL) (*definitiveH)=MIN(charH, boxHeight);

	// call the low level draw text method (implementation dependent)
	if (drawText) {
		// draw it
		return lowLevelDrawTextLine(
			srfTo,// surface
			text, textSize,// text and text size
			realX, realY,// text coordinates
			x, y,// clip box, starting coordinates
			boxWidth, boxHeight,// size of the clip box
			transparent);// transparency
	}else{
		return true;// only calculate the text size
	}
}// privateDrawTextLine

// privateDrawMultilineText ********
// This method is used to draw a text in a given surface, with any
// alignment, dividing it in multiple lines as it's needed, or to
// calculate the size of such a text, or both... method to be used only
// by the class itself
bool fixedFontClass::privateDrawMultilineText(
	ddSurfaceClass& srfTo,// surface where to draw the text
	char* text,// text to draw
	int textSize,// size of the string to draw
	int x, int y,// position where to draw the text
	int boxWidth, int boxHeight,// size of the text box (used for clipping)
	bool transparent,// will the text be transparent?
	alignEnum alignment,// text alignment, either FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER or FILE_DDWRAP_ALIGN_RIGHT
	bool drawText,// tells whether we'll draw the text, or only calculate it
	int* definitiveX, int* definitiveY,// pointer to variables where to return the actual x and y coordinates (and box size) where the text was placed... if null, they aren't used
	int* definitiveW, int* definitiveH)
{
	char* currChar=text;
	int currX=x, currY=y;
	int lineSize;

	// temporary variables to be used while calculating the definitive
	// text size...
	int tempDX=x+boxWidth, tempDY=y+boxHeight, tempDX2=x, tempDY2=y;
	int lastDX, lastDY, lastDW, lastDH;

	// one exceptional case: if width is lower than the character
	// width, there is nothing to be drawn
	if (boxWidth<charW) {
		// return an ok error code, but do nothing really, and
		// notify the box is an empty one
		if (definitiveX!=NULL) (*definitiveX)=x;
		if (definitiveY!=NULL) (*definitiveY)=y;
		if (definitiveW!=NULL) (*definitiveW)=0;
		if (definitiveH!=NULL) (*definitiveH)=0;
		return true;
	}

	while(currChar-text<textSize) {
		// calculate the next text line
		if (!calculateTextLine(
			currChar,  textSize-(currChar-text),// text and text size
			boxWidth,// maxWidth
			&currChar, &lineSize)) {// to be returned by the function

			// error
			return false;
		}

		// draw the line...
//		if (lineSize!=0) {
			if (!privateDrawTextLine(
				srfTo,// the surface
				currChar, lineSize,// text and text size
				currX, currY,// line position
				boxWidth, boxHeight,// text box size
				transparent, alignment,// some other stuff
				drawText,// should we draw the text?
				&lastDX, &lastDY, &lastDW, &lastDH// where to store the real text box coordinates and size
				)) {

				// error
				return false;
			}
//		}

		// keep calculating the smallest text box which covers the text
		tempDX=MIN(tempDX, lastDX);
		tempDY=MIN(tempDY, lastDY);
		tempDX2=MAX(tempDX2, lastDX+lastDW);
		tempDY2=MAX(tempDY2, lastDY+lastDH);

		// increment the position in the text...
		currChar+=lineSize;
		if (currChar-text<textSize) {
			// if we found a 0xD, let's see if there is a 0xA grouped with
			// it...
			if (*currChar==0xD) {
				currChar++;
				if (currChar-text<textSize && *currChar==0xA) currChar++;
			} else if (*currChar==0xA) {
				currChar++;
			}
				
		}

		// increment y...
		currY+=charH;
	}

	// return the text size data
	if (definitiveX!=NULL) (*definitiveX)=tempDX;
	if (definitiveY!=NULL) (*definitiveY)=tempDY;
	if (definitiveW!=NULL) (*definitiveW)=tempDX2-tempDX;
	if (definitiveH!=NULL) (*definitiveH)=tempDY2-tempDY;

	return true;
}// privateDrawMultilineText

// Methods:
// ========

// calculateMultilineTextSize ********
// This method is used to calculate the size of a given multiline text
// as drawn by drawMultilineText.
bool fixedFontClass::calculateMultilineTextSize(
	ddSurfaceClass& srfTo,// surface where to draw the text
	char* text,// text to draw
	int textSize,// size of the string to draw
	int x, int y,// position where to draw the text
	int boxWidth, int boxHeight,// size of the text box (used for clipping)
	alignEnum alignment,// text alignment, either FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER or FILE_DDWRAP_ALIGN_RIGHT
	valignEnum verticalAlignment,// text vertical alignment, either FILE_DDWRAP_VALIGN_TOP, FILE_DDWRAP_VALIGN_CENTER or FILE_DDWRAP_VALIGN_BOTTOM
	int* definitiveX, int* definitiveY,// pointer to variables where to return the text box coordinates and size
	int* definitiveW, int* definitiveH)
{
	bool errorCode;
	int tempDX, tempDY, tempDW, tempDH;

	// let's calculate the text real size
	errorCode=privateDrawMultilineText(
		srfTo, text, textSize,// surface and text
		x, y, boxWidth, boxHeight,// text box coordinates and size
		false, alignment, false,// transparency, alignment, and drawable
		&tempDX, &tempDY, &tempDW, &tempDH);// where to save the text size data

	// let's take into consideration the vertical alignment
	switch(verticalAlignment) {
	case FILE_DDWRAP_VALIGN_TOP:
		// there is nothing to do in this case
		break;
	case FILE_DDWRAP_VALIGN_CENTER:
		tempDY=(boxHeight-tempDH)/2+tempDY;
		break;
	case FILE_DDWRAP_VALIGN_BOTTOM:
		tempDY=(boxHeight-tempDH)+tempDY;
		break;
	}

	// return the text size data
	if (definitiveX!=NULL) (*definitiveX)=tempDX;
	if (definitiveY!=NULL) (*definitiveY)=tempDY;
	if (definitiveW!=NULL) (*definitiveW)=tempDW;
	if (definitiveH!=NULL) (*definitiveH)=tempDH;

	return errorCode;
}// calculateMultilineTextSize

// drawMultilineText ********
// This method is used to draw a text in a given surface, with any
// vertical and horizontal alignment, dividing it in multiple lines
// as it's needed.
bool fixedFontClass::drawMultilineText(
	ddSurfaceClass& srfTo,// surface where to draw the text
	char* text,// text to draw
	int textSize,// size of the string to draw
	int x, int y,// position where to draw the text
	int boxWidth, int boxHeight,// size of the text box (used for clipping)
	bool transparent,// will the text be transparent?
	alignEnum alignment,// text alignment, either FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER or FILE_DDWRAP_ALIGN_RIGHT
	valignEnum verticalAlignment)// text vertical alignment, either FILE_DDWRAP_VALIGN_TOP, FILE_DDWRAP_VALIGN_CENTER or FILE_DDWRAP_VALIGN_BOTTOM
{
	// calculate the coordinates we'll have to use
	int tempDX, tempDY, tempDW, tempDH;

	if (!calculateMultilineTextSize(
		srfTo, text, textSize,// surface and text
		x, y, boxWidth, boxHeight,// text box coordinates size
		alignment, verticalAlignment,// text alignment
		&tempDX, &tempDY, &tempDW, &tempDH// where to save the text size data
		)) {

		return false;// error!
	}

	// now it's time to draw the text
	return privateDrawMultilineText(
		srfTo, text, textSize,// surface and text
		tempDX, tempDY, tempDW, tempDH,// text box coordinates and size
		transparent, alignment);// transparency, alignment
}// drawMultilineText

// **************************
// BITMAPPED FIXED FONT CLASS
// **************************

// Protected and private methods:
// ==============================

// lowLevelDrawTextLine *****
// This method manages the low level drawing of a one line text
// in a surface. It might be different for every subclass of
// fixedFontClass.
bool bitmappedFixedFontClass::lowLevelDrawTextLine(
	ddSurfaceClass& srfTo,// surface where to draw the text
	char* text,// text to draw
	int textSize,// size of the string to draw
	int x, int y,// position where to draw the text
	int xClip, int yClip,// clip box
	int wClip, int hClip,
	bool transparent)// will the text be transparent?
{
	int i;
	characterStruct* currCharInfo;// current char information
	char* currChar=text;// current char
	int xTemp=x, yTemp=y;

	// check it's initialized
	if (!initialized) return false;

	// check for the clipping area being totally outside the surface
	if (!INTERSECTINGRECTANGLES(
		0, 0, srfTo.getWidth(), srfTo.getHeight(),// surface rectangle
		xClip, yClip, xClip+wClip, yClip+hClip)) {// clipping rectangle

		// nothing to draw in such case
		return true;// all ok
	}

	// calculate the clipping rectangle (correct it if it goes further than
	// the surface)
	if (xClip<0) {
		wClip+=xClip;
		xClip=0;
	}
	if (yClip<0) {
		hClip+=yClip;
		yClip=0;
	}
	if (xClip+wClip>srfTo.getWidth()) wClip=srfTo.getWidth()-xClip;
	if (yClip+hClip>srfTo.getHeight()) hClip=srfTo.getHeight()-yClip;

	// draw every character
	for (i=0; i<textSize; i++) {
		currCharInfo=&(characters[(unsigned char)(*currChar)]);

		if (currCharInfo->isAllowed) {
			// draw a character in the destination surface
			if (!srfTo.bltFrom(
				*surface,// source surface
				xTemp, yTemp,// destination coordinates
				currCharInfo->x, currCharInfo->y,// source coordinates
				charW, charH,// image size
				xClip, yClip,// rectangle for clipping
				wClip, hClip,
				true,// clip: true
				transparent))// transparency?
				return false;
		}

		// go to the next character
		currChar++;
		xTemp+=charW;
	}

	return true;// all ok
}// lowLevelDrawTextLine


// Public methods:
// ===============

// init *************
// This method initializes a fixed font object.
bool bitmappedFixedFontClass::init(
	ddSpriteClass* fontSurface,// surface containing the font data
	int charWidth,// fixed width for the characters
	int charHeight)// fixed height for the characters
{
	int i;

	// check it's not already initialized
	if (initialized) return false;

	// initialize the characters data
	characters=new characterStruct[256];	
	for (i=0; i<256; i++) characters[i].isAllowed=false;

	// set other data
	surface=fontSurface; charW=charWidth; charH=charHeight;
	initialized=true;
	
	return true;// all ok
}// init

// end **************
// Method which gives end to the fixed font object.
bool bitmappedFixedFontClass::end()
{
	// check it's initialized
	if (!initialized) return false;

	delete[] characters;
	
	initialized=false;
	
	return true;// all ok
}// end

// setUpCharactersInMatrix ****
// This methods prepares the characters information, in the case
// characters are organized as a matrix in the font surface.
// For example, if you have a characters matrix like the following one:
//		ABCD
//		EFab
//		cdef
// You'd use:
// setUpCharactersInMatrix("ABCDEFabcdef", 0, 0, 4, 0, 0);
bool bitmappedFixedFontClass::setUpCharactersInMatrix(
	char* string,// string showing all the characters present in the matrix, in the order they appear there (from left to right, then from top to bottom)
	int xMat, int yMat,// coordinates where the matrix starts
	int matWidthChars,// width of the matrix in characters
	int hSpace,// horizontal space, in pixels, between characters
	int vSpace)// vertical space, in pixels, between characters
{
	char* currChar;// current character
	characterStruct* currCharInfo;// current char information
	int x=xMat, y=yMat;// coordinates for every character

	for (currChar=string; *currChar!=0; currChar++) {
		// save the coordinates for this character
		currCharInfo=&(characters[(unsigned char)(*currChar)]);
		currCharInfo->isAllowed=true;
		currCharInfo->x=x;
		currCharInfo->y=y;

		// calculate the next character coordinates
		x+=charW+hSpace;
		// check whether we've reached the right border of the matrix or not
		if (x>=matWidthChars*(charW+hSpace)+xMat) {
			x=xMat;
			y+=charH+vSpace;
		}
	}

	return true;
}// setUpCharactersInMatrix

