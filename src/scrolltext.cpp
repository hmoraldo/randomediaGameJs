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


// That's a little class used to draw scrolling texts in the screen

#include "ddwrap.h"
#include "scrolltext.h"


// Initialize the scrollText object...
bool scrollTextClass::init(
	int width, int height,// size of the surface to write into
	int r, int g, int b,// color of the text
	int rB, int gB, int bB,// color of the background
	bool videoMemory)// is in video memory?
{
/*	// set all up for the calculate text
	int tempInt=width;
	fontHeight=0;

	if (initialized) return false;

	// initialize the surface and calculate the amount of lines
	if (!surface.create(width, height, videoMemory)) return false;
	if (!surface.calculateText("z\nz", FILE_DDWRAP_ALIGN_LEFT,
		tempInt, fontHeight)) {

		surface.free();
		return false;
	}
	fontHeight/=2;// I measured two lines
	numLines=height/fontHeight;

	// remember the colors to use
	scrollTextClass::r=r;
	scrollTextClass::g=g;
	scrollTextClass::b=b;
	scrollTextClass::rB=rB;
	scrollTextClass::gB=gB;
	scrollTextClass::bB=bB;

	// time to clear the surface
	clearSurface();

	initialized=true;*/
	return true;// all ok
}// init

// Close the scroll object.
bool scrollTextClass::end()
{
/*	if (!initialized) return false;

	// bye bye surface
	if (!surface.free()) return false;

	initialized=false;*/
	return true;// all ok
}// end

// Scroll some amount of lines up.
bool scrollTextClass::scrollLines(int lines)
{
/*	int linesToMove=numLines-lines;
	int yBlock=lines*fontHeight;
	int hBlock=linesToMove*fontHeight;

	if (!initialized) return false;

	if (!surface.bltFrom(
		surface,// copy from this one
		0, 0,// destination pos
		0, yBlock,// source block
		surface.getWidth(), hBlock// size of the block to move
		)) return false;

	// fill the remaining space with the background color
	if (!surface.fillRect(rB, gB, bB,
		0, hBlock)) return false;
*/
	return true;// all ok
}// scrollLines

// Writes a text in the latest line of the scrolling text.
bool scrollTextClass::writeLatestLine(char* text)
{
/*	if (!initialized) return false;

	if (!surface.fillRect(rB, gB, bB, 0, (numLines-1)*fontHeight))
		return false;

	// write the text
	if (!surface.writeText(text, r, g, b, FILE_DDWRAP_ALIGN_LEFT,
		0, (numLines-1)*fontHeight)) return false;
*/
	return true;// all ok
}// writeLatestLine


// Copies the current image of the scroll text surface to some
// graphic object (surface), in the coordinates specified.
bool scrollTextClass::blt(ddSurfaceClass& srf, int x, int y)
{
	if (!initialized) return false;

	// go blit it
//	if (!srf.bltFrom(surface, x, y)) return false;

	return true;// all ok!
}// blt

// it's used to paint the background of the surface, so clearing it
// (and also making it be ready, if it's just being initialized)
bool scrollTextClass::clearSurface()
{
/*	// paint the console background now
	if (!surface.fillRect(rB, gB, bB, 0, 0)) return false;
*/
	return true;
}// clearSurface

// Tell us if the surface where we store the scrolling text
// is lost (meaning it has to be restored)
bool scrollTextClass::isLost(bool* errorCode)
{
	// check if it's lost
//	return surface.isLost(errorCode);
	return true;
}// isLost

// Restore the surface where we use to draw the scrolling object.
// The surface obtained could not keep the text, as it could have been
// lost together with the surface.
bool scrollTextClass::restore()
{
/*	// restore the surface itself
	if (!surface.restore())
		return false;// something failed
*/
	// now clear the surface
	return clearSurface();
}// restore
