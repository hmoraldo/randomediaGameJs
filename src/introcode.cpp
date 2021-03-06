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

#include "introcode.h"// for accesing to the methods related to the program zone corresponding to the intro
#include "gameglobals.h"// for using the game objects
#include "globals.h"// for using the program objects
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them
#include <SDL.h>
#include <string.h>

// *****************************
// MAIN FUNCTIONS
// Addressed as IntroLoopCode::*
// *****************************

// English texts
static char* presText_en="H. Hern�n Moraldo productions\nis proud to present";
static char* finalText_en="- - R A N D O M E D I A - -";
static char* skipText_en="(PRESS SPACE TO SKIP)";
static char* loadingText_en="R A N D O M E D I A\n\nNow loading...";

// Spanish texts
static char* presText_sp="H. Hern�n Moraldo productions\npresenta con extremo orgullo";
static char* finalText_sp="- - R A N D O M E D I A - -";
static char* skipText_sp="(PRESIONE ESPACIO PARA OMITIR LA INTRO)";
static char* loadingText_sp="R A N D O M E D I A\n\nCargando...";

// Some other variables
static char* textToShow;
static double textLightness;// 0 to 255, lightness factor
static double lightingSpeedPres;// how much to light per frame
static double obscuringSpeedPres;// how much to obscure per frame
static double lightingSpeedFinal;// how much to light per frame
static double obscuringSpeedFinal;// how much to obscure per frame
static int framesToWaitPres;// how much to wait there
static int framesToWaitFinal;// how much to wait there
static enum {nowObscuringFinal, nowWaitingFinal, nowLightingFinal}
	actionInProcess;
static int framesWaitingLeft;

// drawFrame *************
// Draw the current frame on screen.
// Only return false for non-recover errors (not for lost surfaces,
// for example).
bool IntroLoopCode::drawFrame()
{
	ddSpriteClass* buffer;

	// get the buffer's surface
	buffer=getSurfaceByResourceName("srfScreenSizedBuffer");

	// draw either a text or the logo images
	if (false) {
		// draw a black blackground
		buffer->fillAllDword(0);

		// draw the intro text
		fntDahFont.drawMultilineText(
			*(buffer),// where to draw the text to
			textToShow,// text to draw
			strlen(textToShow),// text size
			0,// horizontal position
			0,// vertical position, changed to be somewhat higher in the screen
			DD.getWidth(), DD.getHeight(),// text size
			true, FILE_DDWRAP_ALIGN_CENTER,// transparency, alignment
			FILE_DDWRAP_VALIGN_CENTER);// vertical alignment
	}else{
		DWORD color;
		ddSurfaceClass* srf;

		srf=getSurfaceByResourceName("srfIntroRediaLogo");

		// draw the Randomedia logo
		buffer->bltFrom(*srf, 0, 0);
	}

	// now blt the buffer to the screen, obscured
	DD.obscuredBltFrom(*(buffer),// surface
		0, 0,// coordinates
		(unsigned char)(textLightness),// lightness
		false);// transparent?

	// draw a black rectangle
	{
	int x, y, w, h;
	fntEdiaFont.calculateMultilineTextSize(
		DD,// where to draw the text to
		LAN_SEL(skipText_sp,skipText_en),// text to draw
		strlen(LAN_SEL(skipText_sp,skipText_en)),// text size
		0,// horizontal position
		0,// vertical position, changed to be somewhat higher in the screen
		DD.getWidth(), DD.getHeight()-40,// text size
		FILE_DDWRAP_ALIGN_CENTER,// alignment
		FILE_DDWRAP_VALIGN_BOTTOM,// vertical alignment
		&x, &y, &w, &h);
	DD.fillRect(0, 0, 0, x-2, y-2, w+4, h+4);
	}// block

	// draw the text telling how to skip
	fntEdiaFont.drawMultilineText(
		DD,// where to draw the text to
		LAN_SEL(skipText_sp,skipText_en),// text to draw
		strlen(LAN_SEL(skipText_sp,skipText_en)),// text size
		0,// horizontal position
		0,// vertical position, changed to be somewhat higher in the screen
		DD.getWidth(), DD.getHeight()-40,// text size
		true, FILE_DDWRAP_ALIGN_CENTER,// transparency, alignment
		FILE_DDWRAP_VALIGN_BOTTOM);// vertical alignment

	return true;// all ok
}// drawFrame

// executeFrame ***********
// Execute a frame, without drawing it on screen. It's what I call
// "virtual frames".
// Only return false for non-recover errors.
bool IntroLoopCode::executeFrame(DWORD frameNumber)
{
	// do whatever is needed now
	switch(actionInProcess) {
	case nowLightingFinal:
		// start lighting the text
		textLightness+=lightingSpeedFinal;

		// when it's done, go to the waiting stage
		if (textLightness>=255) {
			textLightness=255;

			// wait
			actionInProcess=nowWaitingFinal;
			framesWaitingLeft=framesToWaitFinal;
		}
		break;
	case nowWaitingFinal:
		// wait some time
		framesWaitingLeft--;
		if (framesWaitingLeft==0) actionInProcess=nowObscuringFinal;
		break;
	case nowObscuringFinal:
		// obscure it
		textLightness-=obscuringSpeedFinal;

		// when it's done, go to the menues
		if (textLightness<=0) {
			textLightness=0;

			if (!switchToProgramZone(pgzon_gameMenues, 0, 0, true)) {
				return false;
			}
		}
		break;
	}

	// Check for keys...
	// Space and escape go to menues
	if (!showConsole  &&
		((KEYEVENTDOWN(keyboard.keys, keyboard.latestKeys, SDLK_SPACE) ||
		KEYEVENTDOWN(keyboard.keys, keyboard.latestKeys, SDLK_ESCAPE)))) {

		// go to the menues
		if (!switchToProgramZone(pgzon_gameMenues, 0, 0, true))
			return false;
	}

	return true;// all ok
}// executeFrame

// beforeLoop **********
// This is the code to be executed every time a program zone linked to
// these functions is switched to.
bool IntroLoopCode::beforeLoop()
{
	double secLightPres=0.8;
	double waitPres=2.46-secLightPres;
	double secObsPres=3.2-(secLightPres+waitPres);
	double secLightFinal=4.0-(secLightPres+waitPres+secObsPres);
	double waitFinal=6.0-(secLightPres+waitPres+secObsPres+secLightFinal);
	double secObsFinal=9.0-(secLightPres+waitPres+secObsPres+secLightFinal+waitFinal);

	// calculate the speeds for pres
	// obscure all in secObsPres seconds
	obscuringSpeedPres=(255/double(desiredFramesPerSecond))/secObsPres;
	// wait waitPres seconds
	framesToWaitPres=int(waitPres*desiredFramesPerSecond);
	// light all in secLightPres seconds
	lightingSpeedPres=(255/double(desiredFramesPerSecond))/secLightPres;

	// calculate the speeds for final
	// obscure all in secObsFinal seconds
	obscuringSpeedFinal=(255/double(desiredFramesPerSecond))/secObsFinal;
	// wait waitFinal seconds
	framesToWaitFinal=int(waitFinal*desiredFramesPerSecond);
	// light all in secLightFinal seconds
	lightingSpeedFinal=(255/double(desiredFramesPerSecond))/secLightFinal;

	// reset the lightness information
	textLightness=0;
	textToShow=LAN_SEL(presText_sp,presText_en);
	actionInProcess=nowLightingFinal;

	return true;// all ok
}// beforeLoop


// afterLoop ***********
// This is the code to be executed every time it's time to switch to
// another program zone, being a program zone currently linked to
// these functions.
bool IntroLoopCode::afterLoop()
{
	char* textToShow=LAN_SEL(loadingText_sp, loadingText_en);

	// write a 'now loading message on the screen'
	// draw a black blackground
	DD.fillAllDword(0);

	// draw the intro text
	fntDahFont.drawMultilineText(
			DD,// where to draw the text to
			textToShow,// text to draw
			strlen(textToShow),// text size
			0,// horizontal position
			0,// vertical position, changed to be somewhat higher in the screen
			DD.getWidth(), DD.getHeight(),// text size
			true, FILE_DDWRAP_ALIGN_CENTER,// transparency, alignment
			FILE_DDWRAP_VALIGN_CENTER);// vertical alignment

	// show it
	DD.flip();

	return true;// all ok
}// afterLoop


