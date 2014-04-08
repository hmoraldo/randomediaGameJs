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


// Console.* uses "scrollTextClass" to make a very useful console
// work.

#include "console.h"
#include <string.h>

// Set up the console for this program
bool consoleClass::init(int width, int height, int maxLetters)
{
/*	if (initialized) return false;

	if (!scroll.init(width, height, 255, 255, 255, 100, 100, 100))
		return false;

	// initialize all the variables involved
	maxChars=maxLetters;
	lastLine=new char[maxChars];// used for storing the command line
	textParam=new char[maxChars];// used for getting text params
	lastLine[0]=0;
	currentLetter=0;
	nextParam=0;
	lastLineBuffered=false;// there is no text to be buffered there
	initialized=true;
*/
	return true;// all ok
}// init

// Ends the use of the console
bool consoleClass::end()
{
/*	if (!initialized) return false;

	delete[] lastLine;
	delete[] textParam;
	initialized=false;
*/
	return scroll.end();
}// end

// Used when a key is pressed
bool consoleClass::keyUp(char key)
{
/*	if (!initialized) return false;

	// reset the address of the next param to be read
	nextParam=0;

	// every time a key is pressed, it means the buffer is working
	// with the current text line... so, the last one is no more
	// buffered
	lastLineBuffered=false;

	switch(key) {
	case '\r':// line break
		// let's see if this line isn't a blank one
		if (currentLetter==0) {
			// it's blank, then let's clear the buffer by
			// putting the zero to the string
			lastLine[currentLetter]=0;
		}

		// we change it because the buffer (lastLine) currently contains
		// the oldest line, and not the void one that appears when
		// you press enter.
		lastLineBuffered=true;

		// scroll once
		if (!scroll.scrollLines(1)) return false;

		// go back to the beginning of the line
		currentLetter=0;

		break;
	case '\b':// backspace
		if (currentLetter>0) currentLetter--;

		break;
	default:// some common letter
		if (currentLetter<maxChars-1) {
			// it's a common letter, add it to the latest line
			lastLine[currentLetter]=key;
			currentLetter++;
		}

		break;
	}
	
	// check... it's not supposed to change a buffered old line
	// anyway, when there is a buffered old line, it means current one
	// is empty.
	if (!lastLineBuffered) {
		// put the zero to the string
		lastLine[currentLetter]=0;

		// write the latest line
		if (!scroll.writeLatestLine(lastLine)) return false;
	}
*/
	return true;// all ok
}// keyUp


// Draws the console in a surface
bool consoleClass::draw(ddSurfaceClass& srf, int x, int y)
{
	if (!initialized) return false;

	if (!scroll.blt(srf, x, y)) return false;

	return true;// all ok
}// draw

// Writes a line of text on the scrolling surface.
// It doesn't modify the command line at all, and writes it (the
// command line) again as the last line.
// It helps keep the text typed by the user, always as the last line.
bool consoleClass::writeLine(char* text)
{
/*	if (!initialized) return false;

	// write the line (only if it's not void), and then scroll up
	// once
	if (text[0]!=0)
		if (!scroll.writeLatestLine(text)) return false;

	if (!scroll.scrollLines(1)) return false;

	// update the last line, with the text typed by the user until now,
	// only if it contains useful info
	if (lastLine[0]!=0 && lastLineBuffered==false)
		if (!scroll.writeLatestLine(lastLine)) return false;
*/
	return true;// all ok
}// writeLine

// Skip all the spaces between params, and make the nextParam point
// to the next non-space character. If it finds any zero there, it
// makes nextParam point to the zero, and returns false (error).
// It's a private method.
bool consoleClass::skipSpaces()
{
/*	if (!initialized) return false;

	// we only can work with params if the text in lastText is the
	// latest text typed (meaning a break line has just been pressed)
	if (!lastLineBuffered) return false;

	// skip the spaces
	while (lastLine[nextParam]==' ') nextParam++;

	// error if the string ends there
	if (lastLine[nextParam]==0) return false;
*/
	return true;// all ok
}// skipSpaces

// Points char* param to a zero ended string containing the name of
// the first parameter (since the latest one read)... If no more params
// are available, returns false (error). Spaces are skipped.
// Integers are also parsed into a string.
//
// This stringZ is very volatile, it could become garbage as soon as
// another method of consoleClass is executed. That is, if you need to
// save this string somewhere, you have to copy it anywhere else to be
// sure it will last.
bool consoleClass::getTextParam(char* &param)
{
/*	char *src, *dst;// source and destine strings

	if (!initialized) return false;

	// we only can work with params if the text in lastText is the
	// latest text typed (meaning a break line has just been pressed)
	if (!lastLineBuffered) return false;

	// skip the spaces, and check for zero.
	if (!skipSpaces()) return false;

	// reset the pointers
	src=lastLine+nextParam;
	dst=textParam;

	// copy the param to its temp location
	while (*src!=' ' && *src!=0) {
		*dst=*src;
		dst++; src++; nextParam++;
	}

	// return the string
	*dst=0;
	param=textParam;
*/
	return true;// all ok
}// getTextParam

// Changes int param to have the value of the first parameter (since
// the latest one read)... If no more integer params
// are available, returns false (error). Spaces are skipped.
bool consoleClass::getIntegerParam(int &param)
{
/*	int number=0;
	char* src;

	if (!initialized) return false;

	// we only can work with params if the text in lastText is the
	// latest text typed (meaning a break line has just been pressed)
	if (!lastLineBuffered) return false;

	// skip the spaces, and check for zero.
	if (!skipSpaces()) return false;

	// reset the pointer
	src=lastLine+nextParam;

	// read the number
	while (*src!=' ' && *src!=0) {
		// fail if it's not a digit
		if (*src<'0' || *src>'9') return false;

		// add it to the number
		number=number*10+(*src-'0');

		src++; nextParam++;
	}

	// return the right number
	param=number;
*/
	return true;// all ok
}// getIntegerParam

// Points char* param to a zero ended string containing the name of
// the first parameter (since the latest one read)... If no more params
// are available, returns false (error). Spaces are skipped.
// Integers are also parsed into a string.
// IMPORTANT:
// This method does only get a 0 char as an indicator of param
// finalization, if you want to finish reading a param when a
// space character is encountered, use getTextParam instead.
//
// This stringZ is very volatile, it could become garbage as soon as
// another method of consoleClass is executed. That is, if you need to
// save this string somewhere, you have to copy it anywhere else to be
// sure it will last.
bool consoleClass::getNoSpacedTextParam(char* &param)
{
/*	char *src, *dst;// source and destine strings

	if (!initialized) return false;

	// we only can work with params if the text in lastText is the
	// latest text typed (meaning a break line has just been pressed)
	if (!lastLineBuffered) return false;

	// skip the spaces, and check for zero.
	if (!skipSpaces()) return false;

	// reset the pointers
	src=lastLine+nextParam;
	dst=textParam;

	// copy the param to its temp location
	while (*src!=0) {
		*dst=*src;
		dst++; src++; nextParam++;
	}

	// return the string
	*dst=0;
	param=textParam;
*/
	return true;// all ok
}// getNoSpacedTextParam

// It sets the param pointer to zero, useful for reading the param
// line more than once.
bool consoleClass::resetParamPointer()
{
	// do it
//	nextParam=0;

	return true;// all ok, always
}// resetParamPointer

// It clears the surface of the console (as if there is no text there)
bool consoleClass::clear()
{
	// clear the surface
	return scroll.clearSurface();
}// clear

// Tell us if the surface where we store the console graphics
// is lost (meaning it has to be restored)
bool consoleClass::isLost(bool* errorCode)
{
	// check if it's lost or not
	return scroll.isLost(errorCode);
}// isLost

// Restore the surface where we use to draw the console graphics
// The surface obtained could not keep the text, as it could have been
// lost together with the surface.
bool consoleClass::restore()
{
	// restore it
	return scroll.restore();
}// restore
