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


#include "ddwrap.h"
#include <SDL.h>
#include <SDL_image.h>

#define RectToSDLRect(r,rsdl) {(rsdl).x=(r).left; (rsdl).y=(r).top; (rsdl).w=(r).right-(r).left; (rsdl).h=(r).bottom-(r).top;}

// some static members definition (ddSurfaceClass)
SDL_Surface* ddSurfaceClass::screenSurface=NULL;// used to be lpDD

bool ddSurfaceClass::isDDReady=false;
int ddSurfaceClass::bpp=0;// screen bpp
directDrawClass* ddSurfaceClass::DDObject=NULL;// pointer to our DD object

// Methods of ddSurfaceClass
// may return error when one of the surfaces is lost
bool ddSurfaceClass::bltFrom(
	ddSurfaceClass& srf,// src surface
	int xD, int yD,// dest coordinates
	bool clip,// clip before blt? true by default
	bool transparent)// transparent blt? false by default
{
	return this->bltFrom(
		srf,// src surface
		xD, yD,// dest coordinates
		0, 0, srf.xRes, srf.yRes,// src coordinates
		clip, transparent// some flags
		);
}

// may return error when one of the surfaces is lost
bool ddSurfaceClass::bltFrom(
	ddSurfaceClass& srf,// src surface
	int xD, int yD,// dest coordinates
	int xS, int yS,// src coordinates
	int wS, int hS,// src size ([w]idth, [h]eight)
	bool clip,// clip before blt? true by default
	bool transparent)// transparent blt? false by default
{
	return this->bltFrom(
		srf,// src surface
		xD, yD,// dest coordinates
		xS, yS, wS, hS,// src coordinates
		0, 0, xRes, yRes,// clip values for the entire destination surface
		clip, transparent// some flags
		);
}

// That's the most complete version of bltFrom
// may return error when one of the surfaces is lost
bool ddSurfaceClass::bltFrom(
	ddSurfaceClass& srf,// src surface
	int xD, int yD,// dest coordinates
	int xS, int yS,// src coordinates
	int wS, int hS,// src size ([w]idth, [h]eight)
	int xClip, int yClip,// these four numbers are the clipping rectangle
	int wClip, int hClip,
	bool clip,// clip before blt? true by default
	bool transparent)// transparent blt? false by default
{
	RECT rectS;

	if (!isReady() || !srf.isReady()) return false;

	if (wS <= 0 || hS <= 0) return true;

	rectS.left=xS;
	rectS.top=yS;
	rectS.right=xS+wS;
	rectS.bottom=yS+hS;

	if (clip) {
		// is the blit necessary?
		if (xD+wS<=xClip || yD+hS<=yClip || xD>=xClip+wClip || yD>=yClip+hClip) {
			return true;
		}

		// clip left
		if (xD<xClip) {
			rectS.left=rectS.left-xD+xClip;
			xD=xClip;
		}

		// clip top
		if (yD<yClip) {
			rectS.top=rectS.top-yD+yClip;
			yD=yClip;
		}

		// clip right
		if (xD+wS>xClip+wClip) {
			rectS.right=rectS.left+xClip+wClip-xD;
		}

		// clip bottom
		if (yD+hS>yClip+hClip) {
			rectS.bottom=rectS.top+yClip+hClip-yD;
		}
	}

/*	if (FAILED(srfOffScreen->BltFast(
		xD, yD,// dest coordinates
		srf.srfOffScreen,// src surface
		&rectS,// src rect
		DDBLTFAST_WAIT |// options
		(transparent?DDBLTFAST_SRCCOLORKEY: DDBLTFAST_NOCOLORKEY)
		))) {

		return false;
	}
*/
	RECT rectD;
	rectD.left=xD;
	rectD.top=yD;
	rectD.bottom=rectD.top+rectS.bottom-rectS.top;
	rectD.right=rectD.left+rectS.right-rectS.left;

	SDL_Rect rectS2;
	SDL_Rect rectD2;

	RectToSDLRect(rectS, rectS2);
	RectToSDLRect(rectD, rectD2);

	if (SDL_BlitSurface(
		srf.srfOffScreen,// src surface
		&rectS2,// src rect
		srfOffScreen,// dest surface
		&rectD2// dest coordinates
		)) {

		return false;
	}

	return true;// all ok
}// bltFrom

// This method executes blts a stretched copy of the source image.
// Dimensions are then different for the source and destination images,
// and mirroring capabilities are also available in this method.
// Clipping is not implemented, as no algorithm was proven to work
// correctly and quickly through DX.
// It might return error when one of the surfaces is lost
bool ddSurfaceClass::stretchBltFrom(
	ddSurfaceClass& srf,// src surface
	int xD, int yD,// dest coordinates
	int wD, int hD,// dest size ([w]idth, [h]eight)
	int xS, int yS,// src coordinates
	int wS, int hS,// src size ([w]idth, [h]eight)
	bool mirrorX, bool mirrorY,// mirror horizontally / vertically? false by default
	bool transparent)// transparent blt? false by default
{
	RECT rectS;
	RECT rectD;
/*	DDBLTFX bltFx={sizeof(bltFx)};

	if (!isReady() || !srf.isReady()) return false;

	rectD.left=xD;
	rectD.top=yD;
	rectD.right=xD+wD;
	rectD.bottom=yD+hD;

	rectS.left=xS;
	rectS.top=yS;
	rectS.right=xS+wS;
	rectS.bottom=yS+hS;

	// Check for mirroring...
	if (mirrorX) {
		bltFx.dwDDFX|=DDBLTFX_MIRRORLEFTRIGHT;
	}
	if (mirrorY) {
		bltFx.dwDDFX|=DDBLTFX_MIRRORUPDOWN;
	}

	if (FAILED(srfOffScreen->Blt(
		&rectD,// dest rect
		srf.srfOffScreen,// src surface
		&rectS,// src rect
		DDBLT_WAIT |// options
		DDBLT_DDFX |// for ensuring mirrorX and mirrorY work properly if selected
		(transparent?DDBLT_KEYSRC: 0),
		&bltFx// ddBltFx for extra data...
		))) {

		return false;
	}
*/
	return true;// all ok
}// stretchBltFrom

// This method blts an image with an obscuring factor, in a given
// position of the surface. The obscuring factor (lightness) ranges from
// 0 to 255, 0 being a black image (too much dark), 255 a full lighted
// one (equal to the original one), and all the values in-between
// meaning a given amount of lightness / darkness.
// Clipping is not implemented in this method, for optimization (this
// method runs through software, it's only hardware accelerated when
// 'lightness' is 0 or 255), so this method returns error when the
// blitting would require clipping to be well done (that is, every time
// one tries to draw outside the destination surface).
bool ddSurfaceClass::obscuredBltFrom(
	ddSurfaceClass& srfSrc,// src surface
	int xD, int yD,// dest coordinates
	int lightness,// value of lightness, ranging from 0 to 255, 0 being the darkest
	bool transparent)// transparent background? false by default... that is: do we use the transparent color?
{
	return bltFrom(srfSrc, xD, yD, true, transparent);
/*	DWORD pitchDest, pitchSrc;// pitch of the surfaces in bytes
	WORD* ptrDest;// pointers to the surfaces
	WORD* ptrSrc;
	ddSurfaceClass& srfDest=(*this);// it isn't necessary at all
	register int heightSrc, widthSrc;// size of the source surface
	register int x, y;// temporary variables for our calculations
	register int rr, gg, bb;// temporary variables for our color calculations
	// Some variables used for optimization
	// First, the numbers to increment when going to pass to the next
	// line
	register int distBetweenLinesDest, distBetweenLinesSrc;
	// This value is used for obscuring every color component.
	register float colorMultiplier;
	// Used for cacheing the latest colors modified, so we can avoid
	// calculating all again.
	// cachedColor: original color
	// modifiedColor: cachedColor after light is applied
	register WORD cachedColor1, modifiedColor1;
	register WORD cachedColor2, modifiedColor2;
	register WORD wordForSwapping;

	if (!srfDest.isReady() || !srfSrc.isReady()) return false;

	// check that transparent blt isn't selected when a transparent
	// color hasn't already been selected
	if (transparent && !srfSrc.transparent) return false;

	// check the bits per pixel, this method only works in 16 bits
	if (bpp!=16) return false;

	// load the size of the source surface
	widthSrc=srfSrc.getWidth();
	heightSrc=srfSrc.getHeight();

	// do some checks
	// If we are out of the drawing area, don't ever try drawing
	// the image
	if (xD<0 || yD<0 ||
		xD+widthSrc>srfDest.getWidth() ||
		yD+heightSrc>srfDest.getHeight()) {
		// failed! tried to draw outside the destination rectangle
		return false;
	}
	// If lightness=0, just draw a blank rectangle, but only if we
	// haven't to consider transparency
	if (lightness==0 && !transparent) {
		srfDest.fillRect(0, 0, 0, xD, yD, widthSrc, heightSrc);
		return true;// all ok
	}
	// If lightness=255, just draw the image normally
	if (lightness==255) {
		srfDest.bltFrom(srfSrc, xD, yD, false, transparent);
		return true;// all ok
	}

	// lock the destination surface, as write only
	if (!srfDest.lock(false, true, &pitchDest, (LPVOID*)(&ptrDest))) {
		// it failed...
		return false;
	}
	// lock the source surface, as read only
	if (!srfSrc.lock(true, false, &pitchSrc, (LPVOID*)(&ptrSrc))) {
		// it failed...
		srfDest.unlock();// unlock the other one before going away
		return false;
	}

	// Displace ptrDest to make it be in the selected position.
	// In bytes, that'd be ptrDest+=((pitchDest*yD)+(2*xD)).
	ptrDest=(WORD*)(((BYTE*)(ptrDest))+(pitchDest*yD)+(2*xD));

	// load some values used for optimization
	// Optimization: increment for next line
	distBetweenLinesDest=pitchDest-(widthSrc*2);
	distBetweenLinesSrc=pitchSrc-(widthSrc*2);
	// Optimization: factor for all the color components
	colorMultiplier=(float)(lightness<=255?lightness:255)/float(255);
	// Optimization: load default values for the cached colors.
	cachedColor1=0; modifiedColor1=0;
	cachedColor2=0; modifiedColor2=0;

	for (y=0; y<heightSrc; y++) {
		for (x=0; x<widthSrc; x++) {
			if (transparent && (*ptrSrc)==srfSrc.transparentDword) {
				// transparent color... do nothing
			}else{
				// non transparent color, obscure this pixel color.
				// look for this color in our first cache
				if ((*ptrSrc)==cachedColor1) {
					(*ptrDest)=modifiedColor1;// it worked
				}else{
					// it wasn't there, look for the color in our second
					// cache
					if ((*ptrSrc)==cachedColor2) {
						(*ptrDest)=modifiedColor2;// it worked

						// now swap colors, let the latest color used be
						// in the first cache
						// SWAP(cachedColor1, cachedColor2)
						wordForSwapping=cachedColor1;
						cachedColor1=cachedColor2;
						cachedColor2=wordForSwapping;
						// SWAP(modifiedColor1, modifiedColor2)
						wordForSwapping=modifiedColor1;
						modifiedColor1=modifiedColor2;
						modifiedColor2=wordForSwapping;
					}else{
						// color is not in cache memories
						// Get the three color components
						rr=getRedFromDword((*DDObject), (*ptrSrc));
						gg=getGreenFromDword((*DDObject), (*ptrSrc));
						bb=getBlueFromDword((*DDObject), (*ptrSrc));

						// Modify every color component
						rr=int(rr*colorMultiplier);
						gg=int(gg*colorMultiplier);
						bb=int(bb*colorMultiplier);

						// Move cache 1 to cache 2
						cachedColor2=cachedColor1;
						modifiedColor2=modifiedColor1;

						// Compose the color back, directly to the cache
						cachedColor1=(*ptrSrc);
						modifiedColor1=
							getDwordFromRGBFast((*DDObject), rr, gg, bb);

						(*ptrDest)=modifiedColor1;
					}// if cache 2
				}// if cache 1
			}// if transparent

			// go to the next pixel
			// let's increment both pointers
			ptrSrc++;
			ptrDest++;
		}// for x

		// advance to the next line
		ptrSrc=(WORD*)(((BYTE*)(ptrSrc))+distBetweenLinesSrc);
		ptrDest=(WORD*)(((BYTE*)(ptrDest))+distBetweenLinesDest);
	}// for y

	// it's ready, unlock both surfaces and go away
	srfDest.unlock();
	srfSrc.unlock();
*/
	return true;// all ok
}// obscuredBltFrom

// This method blts an image blended with the destination surface,
// using an alpha value which indicates the level of opacity of the
// source image (ranging from 0 to 255, being 0 the most transparent
// value, and 255 the fully opaque image).
// Clipping is not implemented in this method, for optimization (this
// method runs through software, it's only hardware accelerated when
// 'lightness' is 0 or 255), so this method returns error when the
// blitting would require clipping to be well done (that is, every time
// one tries to draw outside the destination surface).
bool ddSurfaceClass::alphaBltFrom(
	ddSurfaceClass& srfSrc,// src surface
	int xD, int yD,// dest coordinates
	int alphaValue,// value of alpha blending, between 0 and 255, 0 being totally transparent and 255 being opaque
	bool transparent)// transparent background? false by default... that is: do we use the transparent color?
{
/*	DWORD pitchDest, pitchSrc;// pitch of the surfaces in bytes
	WORD* ptrDest;// pointers to the surfaces
	WORD* ptrSrc;
	ddSurfaceClass& srfDest=(*this);// it isn't necessary at all
	register int heightSrc, widthSrc;// size of the source surface
	// temporary variables for our color calculations
	register int x, y;// coordinates
	register int rrS, ggS, bbS;// source color components
	register int rrD, ggD, bbD;// destination color components
	register int rr, gg, bb;// definitive color components
	// Some variables used for optimization
	// First, the numbers to increment when going to pass to the next
	// line
	register int distBetweenLinesDest, distBetweenLinesSrc;
	// Precalculated floats telling the factor we multiply the source
	// and destination colors to get the blended color
	register float srcColorMultiplier;
	register float destColorMultiplier;
	// Used for cacheing the latest colors modified, so we can avoid
	// calculating all again.
	// cachedColor: original color
	// modifiedColor: cachedColor after alpha is applied
	register WORD cachedSrcColor1;
	register WORD modifiedSrcR1, modifiedSrcG1, modifiedSrcB1;
	register WORD cachedSrcColor2;
	register WORD modifiedSrcR2, modifiedSrcG2, modifiedSrcB2;
	register WORD cachedDestColor1;
	register WORD modifiedDestR1, modifiedDestG1, modifiedDestB1;
	register WORD cachedDestColor2;
	register WORD modifiedDestR2, modifiedDestG2, modifiedDestB2;
	register WORD modifiedSrcR, modifiedSrcG, modifiedSrcB;
	register WORD modifiedDestR, modifiedDestG, modifiedDestB;

	if (!srfDest.isReady() || !srfSrc.isReady()) return false;

	// check that transparent blt isn't selected when a transparent
	// color hasn't already been selected
	if (transparent && !srfSrc.transparent) return false;

	// check the bits per pixel, this method only works in 16 bits
	if (bpp!=16) return false;

	// load the size of the source surface
	widthSrc=srfSrc.getWidth();
	heightSrc=srfSrc.getHeight();

	// do some checks
	// If we are out of the drawing area, don't ever try drawing
	// the image
	if (xD<0 || yD<0 ||
		xD+widthSrc>srfDest.getWidth() ||
		yD+heightSrc>srfDest.getHeight()) {
		// failed! tried to draw outside the destination rectangle
		return false;
	}
	// If alphaValue=0, do nothing
	if (alphaValue==0) {
		return true;// all ok
	}
	// If alphaValue=255, just draw the image normally
	if (alphaValue==255) {
		srfDest.bltFrom(srfSrc, xD, yD, false, transparent);
		return true;// all ok
	}

	// lock the destination surface, as read and write
	if (!srfDest.lock(true, true, &pitchDest, (LPVOID*)(&ptrDest))) {
		// it failed...
		return false;
	}
	// lock the source surface, as read only
	if (!srfSrc.lock(true, false, &pitchSrc, (LPVOID*)(&ptrSrc))) {
		// it failed...
		srfDest.unlock();// unlock the other one before going away
		return false;
	}

	// Displace ptrDest to make it be in the selected position.
	// In bytes, that'd be ptrDest+=((pitchDest*yD)+(2*xD)).
	ptrDest=(WORD*)(((BYTE*)(ptrDest))+(pitchDest*yD)+(2*xD));

	// load some values used for optimization
	// Optimization: increment for next line
	distBetweenLinesDest=pitchDest-(widthSrc*2);
	distBetweenLinesSrc=pitchSrc-(widthSrc*2);
	// Optimization: factors for all the color components
	srcColorMultiplier=(float)(alphaValue<=255?alphaValue:255)/float(255);
	destColorMultiplier=float(1)-srcColorMultiplier;
	// Optimization: load default values for the cached colors.
	cachedSrcColor1=0;
	modifiedSrcR1=0; modifiedSrcG1=0; modifiedSrcB1=0;
	cachedSrcColor2=0;
	modifiedSrcR2=0; modifiedSrcG2=0; modifiedSrcB2=0;
	cachedDestColor1=0;
	modifiedDestR1=0; modifiedDestG1=0; modifiedDestB1=0;
	cachedDestColor2=0;
	modifiedDestR2=0; modifiedDestG2=0; modifiedDestB2=0;

	for (y=0; y<heightSrc; y++) {
		for (x=0; x<widthSrc; x++) {
			if (transparent && (*ptrSrc)==srfSrc.transparentDword) {
				// transparent color... do nothing
			}else{
				// non transparent color, alpha blend this color

				// SOURCE color
				if ((*ptrSrc)==cachedSrcColor1) {
					// let's use the values in the first cache
					modifiedSrcR=modifiedSrcR1;
					modifiedSrcG=modifiedSrcG1;
					modifiedSrcB=modifiedSrcB1;
				}else{
					if ((*ptrSrc)==cachedSrcColor2) {
						// let's use the values in the second cache.
						modifiedSrcR=modifiedSrcR2;
						modifiedSrcG=modifiedSrcG2;
						modifiedSrcB=modifiedSrcB2;
						// for extra optimization, we could swap the
						// caches, but we won't do it this time...
					}else{
						// color is not in cache! calculate it
						// separate the source color in its three components
						rrS=getRedFromDword((*DDObject), (*ptrSrc));
						ggS=getGreenFromDword((*DDObject), (*ptrSrc));
						bbS=getBlueFromDword((*DDObject), (*ptrSrc));

						// compute the source new components
						modifiedSrcR=(WORD)(rrS*srcColorMultiplier);
						modifiedSrcG=(WORD)(ggS*srcColorMultiplier);
						modifiedSrcB=(WORD)(bbS*srcColorMultiplier);

						// move the first cache to the second cache
						cachedSrcColor2=cachedSrcColor1;
						modifiedSrcR2=modifiedSrcR1;
						modifiedSrcG2=modifiedSrcG1;
						modifiedSrcB2=modifiedSrcB1;
						// now save it in the first cache
						cachedSrcColor1=(*ptrSrc);
						modifiedSrcR1=modifiedSrcR;
						modifiedSrcG1=modifiedSrcG;
						modifiedSrcB1=modifiedSrcB;
					}
				}

				// DESTINATION color
				if ((*ptrDest)==cachedDestColor1) {
					// let's use the values in the first cache
					modifiedDestR=modifiedDestR1;
					modifiedDestG=modifiedDestG1;
					modifiedDestB=modifiedDestB1;
				}else{
					if ((*ptrDest)==cachedDestColor2) {
						// let's use the values in the second cache.
						modifiedDestR=modifiedDestR2;
						modifiedDestG=modifiedDestG2;
						modifiedDestB=modifiedDestB2;
						// for extra optimization, we could swap the
						// caches, but we won't do it this time...
					}else{
						// color is not in cache! calculate it
						// separate the destination color in its three components
						rrD=getRedFromDword((*DDObject), (*ptrDest));
						ggD=getGreenFromDword((*DDObject), (*ptrDest));
						bbD=getBlueFromDword((*DDObject), (*ptrDest));

						// compute the destination new components
						modifiedDestR=(WORD)(rrD*destColorMultiplier);
						modifiedDestG=(WORD)(ggD*destColorMultiplier);
						modifiedDestB=(WORD)(bbD*destColorMultiplier);

						// move the first cache to the second cache
						cachedDestColor2=cachedDestColor1;
						modifiedDestR2=modifiedDestR1;
						modifiedDestG2=modifiedDestG1;
						modifiedDestB2=modifiedDestB1;
						// now save it in the first cache
						cachedDestColor1=(*ptrDest);
						modifiedDestR1=modifiedDestR;
						modifiedDestG1=modifiedDestG;
						modifiedDestB1=modifiedDestB;
					}
				}

				// calculate the three components of the definitive color
				rr=modifiedSrcR+modifiedDestR;
				gg=modifiedSrcG+modifiedDestG;
				bb=modifiedSrcB+modifiedDestB;

				// save the color in the destination surface
				(*ptrDest)=getDwordFromRGBFast((*DDObject), rr, gg, bb);
			}// if transparent

			// go to the next pixel
			// let's increment both pointers
			ptrSrc++;
			ptrDest++;
		}// for x

		// advance to the next line
		ptrSrc=(WORD*)(((BYTE*)(ptrSrc))+distBetweenLinesSrc);
		ptrDest=(WORD*)(((BYTE*)(ptrDest))+distBetweenLinesDest);
	}// for y

	// it's ready, unlock both surfaces and go away
	srfDest.unlock();
	srfSrc.unlock();
*/
	return true;// all ok
}// alphaBltFrom

// This method sets the color of a given pixel, using the GDI functions
// for that (so it's supposed to be extremely slow!).
bool ddSurfaceClass::putPixel(
	int x, int y,// pixel coordinates
	int r, int g, int b)// components of the color to use, from 0 to 255 each
{
/*	HDC hdc;

	if (!isReady()) return false;

	// get the dc handle, if error is WasStillDrawing, try again
	if (!getDC(&hdc)) return false;

	if (SetPixel(hdc, x, y, RGB(r, g, b))==-1) {
		// failed
		releaseDC(hdc);
		return false;
	}

	// bye bye dc
	releaseDC(hdc);
*/
	return true;// all ok
}// putPixel

// This method draws a line, using the GDI functions for that (so it's
// supposed to be very slow).
bool ddSurfaceClass::drawLine(
	int x, int y,// coordinates of the first point of the line
	int x2, int y2,// coordinates of the last point of the line
	int r, int g, int b)// components of the color to fill with
{
/*	HDC hdc;
	HPEN pen, oldpen;

	if (!isReady()) return false;

	// get the dc handle, if error is WasStillDrawing, try again
	if (!getDC(&hdc)) return false;

	// create the pen
	if ((pen=CreatePen(PS_SOLID, 0, RGB(r, g, b)))==NULL) {
		// failed
		releaseDC(hdc);
		return false;
	}

	// select the pen
	if ((oldpen=(HPEN)SelectObject(hdc, pen))==NULL) {
		// failed
		DeleteObject(pen);
		releaseDC(hdc);
		return false;
	}

	// move to the line start
	if (MoveToEx(hdc, x, y, NULL)==0) {
		// failed
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
		releaseDC(hdc);
		return false;
	}

	// draw the line
	if (LineTo(hdc, x2, y2)==FALSE) {
		// failed
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
		releaseDC(hdc);
		return false;
	}

	// bye bye pen
	SelectObject(hdc, oldpen);
	DeleteObject(pen);

	// bye bye dc
	releaseDC(hdc);

	// draw the last pixel (otherwise it isn't drawn by the GDI)
	if (!putPixel(x2, y2, r, g, b)) {
		return false;
	}
*/
	return true;// all ok
}// drawLine

// This method fills a rectangle with a color, using the GDI functions
// for that (so it's supposed to be very slow).
bool ddSurfaceClass::fillRect(
	int r, int g, int b,// components of the color to fill with
	int x, int y,// where to start filling
	int width, int height)// if any of both is zero, fill until the end
{
/*	HDC hdc;
	RECT box;
	HBRUSH brush;

	if (!isReady()) return false;

	// get the dc handle, if error is WasStillDrawing, try again
	if (!getDC(&hdc)) return false;

	// now make the rect box
	box.left=x;
	box.top=y;
	if (width==0) {
		box.right=xRes;
	}else{
		box.right=x+width;
	}
	if (height==0) {
		box.bottom=yRes;
	}else{
		box.bottom=y+height;
	}

	// now fill the box
	if ((brush=CreateSolidBrush(RGB(r, g, b)))==NULL) {
		// failed
		releaseDC(hdc);
		return false;
	}

	if (FillRect(hdc, &box, brush)==FALSE) {
		// failed
		DeleteObject(brush);
		releaseDC(hdc);
		return false;
	}

	// bye bye brush
	DeleteObject(brush);

	// bye bye dc
	releaseDC(hdc);
*/
	return true;// all ok
}// fillRect

// Fill the entire surface with a color, by using a hardware blt,
// or a software blt if it's not possible. Very fast compared to fillRect
bool ddSurfaceClass::fillAllDword(DWORD color)
{
/*	HRESULT res;
	DDBLTFX fx={sizeof(fx)};// used to pass the fill color

	if (!isReady()) return false;

	// the color...
	fx.dwFillColor=color;

	// try filling the surface until it can be done
	do {
		// fill the surface if possible
		res=srfOffScreen->Blt(
			NULL, NULL, NULL,// used for blittings only
			DDBLT_COLORFILL,// it's a filler
			&fx);// extra data
	}while(res==DDERR_WASSTILLDRAWING);

	// check for errors
	if (FAILED(res)) return false;
*/
	return true;// all ok
}// fillAllDword

// Writes a text to the surface.
// may return error if the surface is lost
// Be careful, if width wasn't initialized with some meaningful value,
// height will go to infinite (as chances are, width is a big number).
bool ddSurfaceClass::writeText(
	char* text,// text to print out, might include \n's meaning line breaks
	int r, int g, int b,// color of the text
	alignEnum align,// alignment for the text, might be any of the following: FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER, FILE_DDWRAP_ALIGN_RIGHT
	int x, int y,// coordinates for the text to be wrote
	int width, int height)// if any of both is zero, set it to the max in the surface
{
/*	HDC hdc;
	RECT box;
	HFONT font, oldFont;
	UINT alignFlag;

	if (!isReady()) return false;

	// get the DC
	if (!getDC(&hdc)) return false;

	// adjust transparency and color
	if (SetBkMode(hdc, TRANSPARENT)==0 ||
		SetTextColor(hdc, RGB(r, g, b))==CLR_INVALID) {

		// error!
		releaseDC(hdc);
		return false;
	}

	// select the desired font
	font=(HFONT)GetStockObject(ANSI_FIXED_FONT);
	if ((oldFont=(HFONT)SelectObject(hdc, font))==NULL) {
		// error!
		releaseDC(hdc);
		return false;
	}

	// check the text alignment
	switch (align) {
	case FILE_DDWRAP_ALIGN_LEFT:
		alignFlag=DT_LEFT;
		break;
	case FILE_DDWRAP_ALIGN_CENTER:
		alignFlag=DT_CENTER;
		break;
	case FILE_DDWRAP_ALIGN_RIGHT:
		alignFlag=DT_RIGHT;
		break;
	}

	// time to draw the text
	box.left=x;
	box.top=y;
	if (width==0) {
		box.right=xRes;// set it to the max
	}else{
		box.right=x+width;
	}
	if (height==0) {
		box.bottom=yRes;// set it to the max
	}else{
		box.bottom=y+height;
	}
	DrawText(
		hdc,// hdc where to tdraw
		text, -1,// text, and size of the text
		&box,// pos and dimensions of the text box
		alignFlag | DT_WORDBREAK
		| DT_NOPREFIX);// some extra data

	// back to the old font
	SelectObject(hdc, oldFont);

	// bye bye dc
	releaseDC(hdc);
*/
	return true;// all ok
}// writeText

// Calculates the size a text would have, if drawn using the writeText
// method.
// width and heigh are used to pass the same values that for
// writeText, and calculateText will change those for the real values
// that make the box where the text is.
// may return error if the surface is lost
bool ddSurfaceClass::calculateText(
	char* text,// text to print out, might include \n's meaning line breaks
	alignEnum align,// alignment for the text, might be any of the following: FILE_DDWRAP_ALIGN_LEFT, FILE_DDWRAP_ALIGN_CENTER, FILE_DDWRAP_ALIGN_RIGHT
	int &width, int &height)// if any of both is zero, set it to the max in the surface
{
/*	HDC hdc;
	RECT box;
	HFONT font, oldFont;
	UINT alignFlag;

	if (!isReady()) return false;

	// get the DC
	if (!getDC(&hdc)) return false;

	// adjust transparency only (it could not be necessary)
	if (SetBkMode(hdc, TRANSPARENT)==0) {

		// error!
		releaseDC(hdc);
		return false;
	}

	// select the desired font
	font=(HFONT)GetStockObject(ANSI_FIXED_FONT);
	if ((oldFont=(HFONT)SelectObject(hdc, font))==NULL) {
		// error!
		releaseDC(hdc);
		return false;
	}

	// check the text alignment
	switch (align) {
	case FILE_DDWRAP_ALIGN_LEFT:
		alignFlag=DT_LEFT;
		break;
	case FILE_DDWRAP_ALIGN_CENTER:
		alignFlag=DT_CENTER;
		break;
	case FILE_DDWRAP_ALIGN_RIGHT:
		alignFlag=DT_RIGHT;
		break;
	}

	// fill the box rect
	box.left=0;
	box.top=0;
	if (width==0) {
		box.right=xRes;// set it to the max
	}else{
		box.right=width;
	}
	if (height==0) {
		box.bottom=yRes;// set it to the max
	}else{
		box.bottom=height;
	}

	// now calcule the values
	DrawText(
		hdc,// hdc where to tdraw
		text, -1,// text, and size of the text
		&box,// pos and dimensions of the text box
		alignFlag | DT_WORDBREAK
		| DT_NOPREFIX | DT_CALCRECT);// some extra data

	// now change the coordinates to the real values
	width=box.right;
	height=box.bottom;

	// back to the old font
	SelectObject(hdc, oldFont);

	// bye bye dc
	releaseDC(hdc);
*/
	return true;// all ok
}// calculateText

// This method saves a surface to a file, in the BMP format. Sure it's
// very strange reading, but it works...
bool ddSurfaceClass::saveAsBmpFile(
	char* filename,// path and name of the file to save
	bool overwrite)// boolean telling whether we are allowed to overwrite existing files
{
/*	HDC hdc;
	HBITMAP hbitmap;// a bitmap object
	HDC tempHdc;// for loading the right data in the bitmap object
	HBITMAP tempOldBm;// to be used together with tempHdc
	BITMAP bm;// an object containing information about a bitmap
	PBITMAPINFO bitmapInfo;
	BITMAPFILEHEADER header;// two variables for the header data
	PBITMAPINFOHEADER hdrInfo;
	WORD clrBits;// bits per pixel
	BYTE* imgMem;// memory for the bitmap itself

	// file stuff
	HANDLE file;
	DWORD toWrite, written, totalWritten;

	// let's get the dc first
	if (!getDC(&hdc)) return false;

	// create a compatible bitmap from where to take all the
	// information
	if ((hbitmap=CreateCompatibleBitmap(hdc, getWidth(), getHeight()))
		==NULL) {

		// failed, go out!
		releaseDC(hdc);
		return false;
	}

	// create a compatible DC, which we'll use to load the bitmap
	// object with the right information
	if ((tempHdc=CreateCompatibleDC(hdc))==NULL) {
		// failed, go out!
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
		return false;
	}

	// select the bitmap in the temporary DC
	if ((tempOldBm=(HBITMAP)SelectObject(tempHdc, hbitmap))==NULL) {
		// failed, go out!
		DeleteDC(tempHdc);
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
		return false;
	}

	// now load the data of our surface to the new bitmap
	if (BitBlt(
		tempHdc,// destination dc
		0, 0, getWidth(), getHeight(),// destination coordinates
		hdc,// source dc
		0, 0,// source coordinates
		SRCCOPY)// blt mode, copy the pixels
		==0) {

		// failed, go out!
		SelectObject(tempHdc, tempOldBm);
		DeleteDC(tempHdc);
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
		return false;
	}

	// deselect and delete our temporary DC, its time is over now
	SelectObject(tempHdc, tempOldBm);
	DeleteDC(tempHdc);

	// get some information from the bitmap...
	if (GetObject(hbitmap, sizeof(BITMAP), (LPVOID)&bm)==NULL) {
		// failed, go out!
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
		return false;
	}

	// calculate the bits per pixel... and make the value be one of the
	// most used formats
	clrBits=(WORD)(bm.bmPlanes*bm.bmBitsPixel);
	if (clrBits!=1 && clrBits<=4) clrBits=4;
		else if (clrBits<=8) clrBits=8;
		else if (clrBits<=16) clrBits=16;
		else if (clrBits<=24) clrBits=24;
		else clrBits=32;;

	// allocate memory for bitmapInfo
	if (clrBits!=24) {
		// allocate for bitmap info header + color info
		bitmapInfo=(PBITMAPINFO)
			(new BYTE[sizeof(BITMAPINFOHEADER)+
			sizeof(RGBQUAD)*(2^clrBits)]);
	}else{
		// allocate for bitmap info header only
		bitmapInfo=(PBITMAPINFO)
			(new BYTE[sizeof(BITMAPINFOHEADER)]);
	}
	hdrInfo=(PBITMAPINFOHEADER)bitmapInfo;
	
	// load all the data of bitmapInfo
	bitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	// fill the info from the bm structure
	bitmapInfo->bmiHeader.biWidth=bm.bmWidth;
	bitmapInfo->bmiHeader.biHeight=bm.bmHeight;
	bitmapInfo->bmiHeader.biPlanes=bm.bmPlanes;
	bitmapInfo->bmiHeader.biBitCount=bm.bmBitsPixel;
	// why this check is done, I'm not sure
	if (clrBits<24) bitmapInfo->bmiHeader.biClrUsed=2^clrBits;

	// some other info
	bitmapInfo->bmiHeader.biCompression=BI_RGB;
	bitmapInfo->bmiHeader.biSizeImage=
		(bm.bmWidth+7)/8*bm.bmHeight*clrBits;
	bitmapInfo->bmiHeader.biClrImportant=0;

	// fill the info for the header
	header.bfType=0x4d42;// "BM"
	header.bfSize=(DWORD)(sizeof(BITMAPFILEHEADER)+
		hdrInfo->biSize+
		hdrInfo->biClrUsed*sizeof(RGBQUAD)+
		hdrInfo->biSizeImage);
	header.bfReserved1=0;
	header.bfReserved2=0;
	header.bfOffBits=(DWORD)(sizeof(BITMAPFILEHEADER)+
		hdrInfo->biSize+
		hdrInfo->biClrUsed*sizeof(RGBQUAD));

	// now allocate memory for the bitmap itself
	imgMem=new BYTE[bitmapInfo->bmiHeader.biSizeImage];

	// obtain the bitmap memory
	if (GetDIBits(
		hdc,// hdc of the image to take the memory
		hbitmap,// our bitmap handle
		0,// first scan
		bitmapInfo->bmiHeader.biHeight,// scan lines
		imgMem,// bitmap bits
		bitmapInfo,// bitmap data structure
		DIB_RGB_COLORS)// RGB or palette index
		==NULL) {

		// failed, go out
		delete[] imgMem;
		delete[] bitmapInfo;
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
	}

	// Now save all that information to the disk...
	// open the file
	if ((file=CreateFile(
		filename,// file name
		GENERIC_WRITE,// desired access

		0,// share mode, don't share it
		NULL,// security attributes, don't care
		(overwrite?CREATE_ALWAYS:CREATE_NEW),// creation distribution
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,// flags
		NULL))// template file
		==INVALID_HANDLE_VALUE) {

		// failed, go out
		delete[] imgMem;
		delete[] bitmapInfo;
		DeleteObject((HGDIOBJ)hbitmap);
		releaseDC(hdc);
	}

	// save the header
	written=0;
	toWrite=sizeof(header);
	totalWritten=0;
	while (toWrite>0) {
		if (WriteFile(
			file,// file to write to
			(&header)+totalWritten,// buffer
			toWrite,// bytes to write
			&written,// bytes written, to be returned...
			NULL)// overlapped stuff
			==0) {

			// failed, go out
			CloseHandle(file);
			delete[] imgMem;
			delete[] bitmapInfo;
			DeleteObject((HGDIOBJ)hbitmap);
			releaseDC(hdc);
		}

		toWrite-=written;
		totalWritten+=written;
		written=0;
	}

	// save the bitmapInfoHeader and RGBQUAD
	totalWritten=0;
	written=0;
	toWrite=sizeof(BITMAPINFOHEADER)+hdrInfo->biClrUsed*sizeof(RGBQUAD);
	while (toWrite>0) {
		if (WriteFile(
			file,// file to write to
			hdrInfo+totalWritten,// buffer
			toWrite,// bytes to write
			&written,// bytes written, to be returned...
			NULL)// overlapped stuff
			==0) {

			// failed, go out
			CloseHandle(file);
			delete[] imgMem;
			delete[] bitmapInfo;
			DeleteObject((HGDIOBJ)hbitmap);
			releaseDC(hdc);
		}

		toWrite-=written;
		totalWritten+=written;
		written=0;
	}

	// save the color indices
	totalWritten=0;
	written=0;
	toWrite=hdrInfo->biSizeImage;
	while (toWrite>0) {
		if (WriteFile(
			file,// file to write to
			imgMem+totalWritten,// buffer
			toWrite,// bytes to write
			&written,// bytes written, to be returned...
			NULL)// overlapped stuff
			==0) {

			// failed, go out
			CloseHandle(file);
			delete[] imgMem;
			delete[] bitmapInfo;
			DeleteObject((HGDIOBJ)hbitmap);
			releaseDC(hdc);
		}

		toWrite-=written;
		written=0;
	}

	// time to release all the resources allocated until now
	CloseHandle(file);
	delete[] imgMem;
	delete[] bitmapInfo;
	DeleteObject((HGDIOBJ)hbitmap);
	releaseDC(hdc);
*/
	return true;// all ok
}// saveAsBmpFile
/*
// getDC obtains the DC attached to the surface. It also locks the surface,
// so it cannot be used for anything until it's released with releaseDC.
// getDC has to be used quite carefully, and the obtained DC has to be
// released as soon as possible (look at the getDC documentation in the DX
// SDK for more info).
bool ddSurfaceClass::getDC(HDC* hdc)
{
	HRESULT err;

	if (!isReady()) return false;

	// get the dc handle, if error is WasStillDrawing, try again
	do {
		err=srfOffScreen->GetDC(hdc);
	}while(err==DDERR_WASSTILLDRAWING);
	if (FAILED(err)) return false;

	return true;// all ok
}// getDC

bool ddSurfaceClass::releaseDC(HDC hdc)
{
	if (!isReady()) return false;

	if (FAILED(srfOffScreen->ReleaseDC(hdc))) return false;

	return true;// all ok
}// releaseDC
*/

// Don't lock your surfaces unless you really know what you are doing!
// While a surface is locked, it cannot be used for anything until it's
// unlocked... You should unlock a locked surface as soon as possible,
// to avoid problems with system halts and stuff (if missused)
// (Look at the getDC documentation in the DX SDK for more info).
bool ddSurfaceClass::lock(bool onlyRead, bool onlyWrite, DWORD* pitch, LPVOID* pointer)
{
/*#if DIRECTDRAW_VERSION < 0x0700
	DDSURFACEDESC sd={sizeof(sd)};
#else// DIRECTDRAW_VERSION
	DDSURFACEDESC2 sd={sizeof(sd)};
#endif// DIRECTDRAW_VERSION

	if (!isReady()) return false;
	if (locked) return false;

	if (FAILED(srfOffScreen->Lock(
		NULL,// rectangle to lock, as it's null, it locks it all
		&sd,// will come back with important info
		DDLOCK_WAIT | // wait if the surface is busy
		(onlyWrite?DDLOCK_WRITEONLY:DDLOCK_WRITEONLY) | // write only?
		(onlyRead?DDLOCK_READONLY:DDLOCK_READONLY),// read only?
		NULL))) {// non used parameter

		// lock failed
		return false;
	}

	// return important info
	*pitch=sd.lPitch;
	*pointer=sd.lpSurface;

	// mark the surface as locked and go away
	locked=true;*/
	return true;// all ok
}// lock

// It's what you use to unlock a surface.
bool ddSurfaceClass::unlock()
{
/*	if (!isReady()) return false;
	if (!locked) return false;// it should be locked to be unlocked

	if (FAILED(srfOffScreen->Unlock(NULL))) {
		return false;// unlock failed!
	}

	// mark the surface as unlocked and go away
	locked=false;*/
	return true;// all ok
}// unlock

// This method obtains the color of a pixel in the surface, in a DWORD.
// can fail if the surface is lost
bool ddSurfaceClass::getDwordFromPixel(int x, int y, DWORD& color)
{
/*#if DIRECTDRAW_VERSION < 0x0700
	DDSURFACEDESC ddsd={sizeof(ddsd)};// MS's trick to zero it
#else// DIRECTDRAW_VERSION
	DDSURFACEDESC2 ddsd={sizeof(ddsd)};// MS's trick to zero it
#endif// DIRECTDRAW_VERSION

	// lock the surface
	if (FAILED(srfOffScreen->Lock(
		NULL,// dest rect
		&ddsd,// surface desc
		DDLOCK_NOSYSLOCK|DDLOCK_WAIT,// flags
		NULL// event, future param
		)))
		return false;

	// get the pixel
	color=*((DWORD*)(
		((BYTE*)ddsd.lpSurface)+// original pointer
		(x*(bpp/8))+
		(y*ddsd.lPitch)
		));
	// take the useless bits away
	color&=((1<<bpp)-1);

	// unlock the surface
	srfOffScreen->Unlock(NULL);
*/
	return true;
}

// This method converts an RGB value to a DWORD, according to this
// surface's properties
// can fail if the surface is lost
bool ddSurfaceClass::getDwordFromRGB(int r, int g, int b, DWORD& color)
{
/*#if DIRECTDRAW_VERSION < 0x0700
	DDSURFACEDESC ddsd={sizeof(ddsd)};// MS's trick to zero it
#else// DIRECTDRAW_VERSION
	DDSURFACEDESC2 ddsd={sizeof(ddsd)};// MS's trick to zero it
#endif// DIRECTDRAW_VERSION
	HDC hdc;
	COLORREF oldColor;
	bool err;

	// COLOR MATCH IT!
	// 1- set the pixel in (0,0) with the asked color
	if (!getDC(&hdc)) return false;
	oldColor=GetPixel(hdc, 0, 0);
	SetPixel(hdc, 0, 0, RGB(r, g, b));
	releaseDC(hdc);

	// 2- get the changed pixel's equivalent DWORD
	err=getDwordFromPixel(0, 0, color);

	// 3- set the pixel color back!
	if (!getDC(&hdc)) return false;
	SetPixel(hdc, 0, 0, oldColor);
	releaseDC(hdc);

	// 4- now see at getDwordFromPixel's return value
	if (!err) return false;
*/
	// that's been all
	return true;
}
/*
// Tell us whether this surface is lost or not
// (that is, if it needs to be restored)
// (protected version)
#if DIRECTDRAW_VERSION < 0x0700
bool ddSurfaceClass::isLostSrf(bool* errorCode, LPDIRECTDRAWSURFACE& srf)
#else // DIRECTDRAW_VERSION
bool ddSurfaceClass::isLostSrf(bool* errorCode, LPDIRECTDRAWSURFACE7& srf)
#endif // DIRECTDRAW_VERSION
{
	HRESULT res;
	bool lost;

	// it's wrong until we show the contrary
	if (errorCode!=NULL) *errorCode=false;

	// is it lost?
	res=srf->IsLost();

	// test whether it's lost, restored or in an error state
	if (res==DDERR_SURFACELOST) {
		// the surface is lost
		lost=true;
	}else{
		if (FAILED(res)) {
			// it's been an error, return before time!
			return true;// error and lost, that's why it returns true
		}else{
			// the surface isn't lost
			lost=false;
		}
	}

	// all ok
	if (errorCode!=NULL) *errorCode=true;

	// time to inform the state of this
	return lost;
}// isLostSrf

// Restore a lost surface (protected version)
#if DIRECTDRAW_VERSION < 0x0700
bool ddSurfaceClass::restoreSrf(LPDIRECTDRAWSURFACE& srf)
#else // DIRECTDRAW_VERSION
bool ddSurfaceClass::restoreSrf(LPDIRECTDRAWSURFACE7& srf)
#endif // DIRECTDRAW_VERSION
{
	// restore the surface if possible
	if (FAILED(srf->Restore()))
		return false;// it failed

	// it's all ok!
	return true;
}// restoreSrf
*/
// Tell us whether this surface is lost or not
// (that is, if it needs to be restored)
bool ddSurfaceClass::isLost(bool* errorCode)
{
/*	// it's wrong until we show the contrary
	if (errorCode!=NULL) *errorCode=false;

	// are we ready for this?
	if (!isReady()) return true;// error and lost, that's why it returns true

	// see whether it's lost or not
	return isLostSrf(errorCode, srfOffScreen);*/
	return false;
}// isLost

// Restore this surface, if it's lost
bool ddSurfaceClass::restore()
{
/*	// are we ready for this?
	if (!isReady()) return false;// error

	// restore the surface...
	return restoreSrf(srfOffScreen);*/
	return true;
}// restore

// Methods of the class directDrawClass
// have to be called before any other method of any other class
// inherited of ddSurfaceClass
bool directDrawClass::init()
{
	if (isDDReady) return false;

	// change static methods of the base
	isDDReady=true;

/*	DDObject=this;// to let all the ddSurface objects access to their DirectDraw main object*/
	initialized=true;// change flag
	videoModeReady=false;// clean flag

	return true;// all ok
}

bool directDrawClass::end()
{
	if (initialized==false) return false;

	// close the video mode
	if (videoModeReady) {
		if (!closeVideoMode()) return false;
	}

	// bye bye dd
	screenSurface=NULL;
	srfOffScreen=NULL;

	// clear the static variable
	isDDReady=false;

/*	DDObject=NULL;// so it's obvious the object doesn't exist anymore*/
	initialized=false;// clean flag
	videoModeReady=false;// clean flag

	return true;// all ok
}

bool directDrawClass::closeVideoMode()
{
	if (!videoModeReady) return false;

	// bye bye surface...
/*	srfOnScreen->Release();
	srfOnScreen=NULL;*/

	videoModeReady=false;

	return true;// all ok
}

bool directDrawClass::setVideoMode(int x, int y, int bpp, int backbuffers)
{
	if (!initialized) return false;

	if (backbuffers<1) return false;

	screenSurface=SDL_SetVideoMode(x, y, bpp, SDL_ANYFORMAT);
	srfOffScreen=screenSurface;

	xRes=x; yRes=y; directDrawClass::bpp=bpp;
/*	directDrawClass::backbuffers=backbuffers;*/
	videoModeReady=true;// change flag

	// get the info for color handling
/*	if (!updateColorInfo()) {
		// failed
		srfOnScreen->Release();// bye bye my surface
		videoModeReady=false;
		return false;
	}
*/
	return true;// all ok
}

// fill the *Mask, *Off and *Size variables for color handling.
bool directDrawClass::updateColorInfo()
{
/*	DDPIXELFORMAT pf={sizeof(pf)};
	int i, j, tMask, tSize, tOff;// temp variables
	int dwordSize=sizeof(DWORD)*8;// to know the size in bits of a dword

	if (!isVideoModeReady()) return false;

	// fill pf
	if (FAILED(srfOnScreen->GetPixelFormat(&pf))) return false;

	// fill the palettized field
	palettized=false;

	if (bpp<=8) {
		// it's a palettized mode
		palettized=true;

		// In such a case, we'll just set all to zero, and quit
		// without error
		rOff=0; rSize=0; r8LessSize=8;
		gOff=0; gSize=0; g8LessSize=8;
		bOff=0; bSize=0; b8LessSize=8;

		return true;
	}

	// now fill *Mask
	rMask=pf.dwRBitMask;
	gMask=pf.dwGBitMask;
	bMask=pf.dwBBitMask;

	// now fill *Off and *Size
	// We use the same process for the three components (r, g, b)
	for (i=0; i<3; i++) {
		// load our component
		switch(i) {
		case 0:// time for red
			tMask=rMask; break;
		case 1:// time for green
			tMask=gMask; break;
		case 2:// time for blue
			tMask=bMask; break;
		}

		// now see what's the offset for this component
		for (j=0; j<dwordSize; j++) {
			if ((tMask&1)==1) break;// break as soon as a set bit is found

			// time for the next bit
			tMask>>=1;
		}// for j

		if (j==dwordSize) {
			// if it happens, tMask was zero
			// that's bad news, as it's not a palettized mode
			return false;// error!
		}else{
			// now we know what's this component offset in bits
			tOff=j;
		}

		// let's see what's the size for this component
		for (; j<dwordSize; j++) {
			if ((tMask&1)==0) break;// break as soon as a 0 bit is found

			// time for the next bit
			tMask>>=1;
		}// for j

		// now we know what's this component size in bits
		tSize=j-tOff;

		// save this component info
		switch(i) {
		case 0:// time for red
			rOff=tOff; rSize=tSize;
			r8LessSize=8-rSize;
			break;
		case 1:// time for green
			gOff=tOff; gSize=tSize;
			g8LessSize=8-gSize;
			break;
		case 2:// time for blue
			bOff=tOff; bSize=tSize;
			b8LessSize=8-bSize;
			break;
		}
	}// for i
*/
	return true;// all ok
}// updateColorInfo

// convert 'palette' to the current video mode (only if it's an 8 bits
// video mode).
// This 8 bit version is very slow, as it uses some system locks for
// every color entry.
// The format of 'palette' is the one for PCXs, that is, three bytes
// (8 bits R, G and B) for every entry. 'pal8' is an only byte for every
// entry.
bool directDrawClass::convertPaletteTo8(
	const BYTE* palette,// original palette in PCX format (see above)
	BYTE* pal8,// resulting palette (memory should be allocated already)
	int entries)// amount of entries (colors)
{
/*	int i;
	DWORD dwColor;
	const BYTE* pal=palette; BYTE* palTo=pal8;

	if (!isVideoModeReady()) return false;
	if (bpp!=8) return false;// only for 8 bits video modes

	// convert every entry to its 8 bit color
	for (i=0; i<entries; i++) {
		if (!getDwordFromRGB(pal[0], pal[1], pal[2], dwColor))
			return false;// error

		// we only use 8 bits, discard the unused 24 bits
		*palTo=(BYTE)(dwColor & 255);

		pal+=3;
		palTo++;
	}// for i
*/
	return true;// all ok
}// convertPaletteTo8

// convert 'palette' to the current video mode (only if it's a 16 bits
// video mode)
// The format of 'palette' is the one for PCXs, that is, three bytes
// (8 bits R, G and B) for every entry. 'pal16' is an only word for every
// entry.
bool directDrawClass::convertPaletteTo16(
	const BYTE* palette,// original palette in PCX format (see above)
	WORD* pal16,// resulting palette (memory should be allocated already)
	int entries)// amount of entries (colors)
{

/*	int i;
	DWORD dwColor;
	const BYTE* pal=palette; WORD* palTo=pal16;

	if (!isVideoModeReady()) return false;
	if (bpp!=16) return false;// only for 16 bits video modes

	// convert every entry to its 16 bit color
	for (i=0; i<entries; i++) {
		dwColor=getDwordFromRGBFast(*this, pal[0], pal[1], pal[2]);

		// we only use 16 bits, discard the unused word
		*palTo=(WORD)(dwColor & 65535);

		pal+=3;
		palTo++;
	}// for i
*/
	return true;
}// convertPaletteTo16

bool directDrawClass::flip()
{
/*	HRESULT ret;

	if (FAILED(ret=srfOnScreen->Flip(
		NULL,// next surface (any)
		DDFLIP_WAIT)// wait until it's flipped
		)) {

		// was the surface lost???
		if (ret==DDERR_SURFACELOST) {
			// bla bla
		}

		return false;
	}
*/
	SDL_UpdateRect(screenSurface, 0, 0, 0, 0) ;

	return true;// all ok
}

// was the surface initialized enough as to be used?
bool directDrawClass::isReady()
{
	return initialized && videoModeReady;
}

// Tell us whether the main surface is lost or not
// (that is, if it needs to be restored)
bool directDrawClass::isLost(bool* errorCode)
{
/*	// it's wrong until we show the contrary
	if (errorCode!=NULL) *errorCode=false;

	// are we ready for this?
	if (!isReady()) return true;// error and lost, that's why it returns true

	// see whether it's lost or not
	return isLostSrf(errorCode, srfOnScreen);*/
	return false;
}// isLost

// Restore the main surface
bool directDrawClass::restore()
{
/*	// are we ready for this?
	if (!isReady()) return false;// error

	// restore the surface...
	return restoreSrf(srfOnScreen);*/
	return true;
}// restore

// Methods of ddSpriteClass
bool ddSpriteClass::create(int x, int y, bool videoMemory)
{
	xRes=x; yRes=y;

	srfOffScreen=SDL_CreateRGBSurface(videoMemory?SDL_HWSURFACE:SDL_SWSURFACE, x, y, bpp, 0, 0, 0, 0);
	if (srfOffScreen==NULL) return false;

	xRes=x;
	yRes=y;
	realWidth=x;
	realHeight=y;
	inVideoMemory=videoMemory;
	initialized=true;// change flag

/*#if DIRECTDRAW_VERSION < 0x0700
	DDSURFACEDESC ddsd={sizeof(ddsd)};// MS's trick to zero it
#else// DIRECTDRAW_VERSION
	DDSURFACEDESC2 ddsd={sizeof(ddsd)};// MS's trick to zero it
#endif// DIRECTDRAW_VERSION

	if (!isDDReady) return false;// direct draw wasn't initialized
	if (initialized) return false;// we have to create it only once

	// fill the struct...
	// fields to be used
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps=// capabilities
		DDSCAPS_OFFSCREENPLAIN |
		(videoMemory?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY);
	ddsd.dwWidth=x; ddsd.dwHeight=y;// size
	// call the api itself
	if (FAILED(lpDD->CreateSurface(
		&ddsd,// surface description
		&srfOffScreen,// new surface pointer
		NULL// future param
		))) {

		// failed... maybe because lacking of video memory
		if (videoMemory=true) {
			return create(x, y, false);
		}
		return false;
	}

	// actualize fields
	xRes=x;
	yRes=y;
	realWidth=x;
	realHeight=y;
	inVideoMemory=videoMemory;
	initialized=true;// change flag
*/
	return true;
}

bool ddSpriteClass::free()
{
/*	if (!initialized) return false;

	srfOffScreen->Release();
	srfOffScreen=NULL;

	initialized=false;
*/
	return true;
}

/*bool ddSpriteClass::genericLoadBmp(char* name, bool isResource, HINSTANCE hInstance, bool videoMemory)
{
	HBITMAP hbm;// bitmap handle
	HBITMAP hbmOld;// other bitmap handle
	BITMAP bm;// for info
	HDC hdcImage, hdc;// two hdcs for the blitting
	bool created=false;// just to know whether we've created the surface or not

	// load the image itself
	if (isResource) {
		hbm=(HBITMAP)LoadImage(
			hInstance,
			name,// resource name
			IMAGE_BITMAP,
			0, 0,// dest coordinates
			LR_CREATEDIBSECTION);
	}else{
		hbm=(HBITMAP)LoadImage(
			NULL,// hInstance
			name,// path
			IMAGE_BITMAP,
			0, 0,// dest coordinates
			LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	}

	if (hbm==NULL) return false;

	// to see the size of the bitmap
	GetObject(hbm, sizeof(bm), &bm);

	// create the surface needed... or work with the existent one
	if (!initialized) {
		// create the surface
		if (!create(bm.bmWidth, bm.bmHeight, videoMemory)) {
			// error, bye bye all
			DeleteObject(hbm);
			return false;
		}
		created=true;// we created it
	}else{
		// is this surface able to handle the bitmap???
		if (realWidth<bm.bmWidth || realHeight<bm.bmHeight) {
			// error, bye bye all
			DeleteObject(hbm);
			return false;
		}
	}

	// now get both hdcs
	// 1- image
	hdcImage=CreateCompatibleDC(NULL);// it's ok even if it sounds bad
	hbmOld=(HBITMAP)SelectObject(hdcImage, hbm);
	// 2- surface
	if (!getDC(&hdc)) {
		// error, bye bye all
		SelectObject(hdcImage, hbmOld);
		DeleteDC(hdcImage);
		DeleteObject(hbm);
		// if we created the surface, let's kill it
		if (created) free();
		return false;
	}

	// copy that image
	BitBlt(
		hdc,// hdc dest
		0, 0,// x y dest
		bm.bmWidth, bm.bmHeight,// size
		hdcImage,// hdc source
		0, 0,// // x y src
		SRCCOPY// blt mode
		);

	// update the "virtual" size
	xRes=bm.bmWidth;
	yRes=bm.bmHeight;

	// free all
	releaseDC(hdc);
	SelectObject(hdcImage, hbmOld);
	DeleteDC(hdcImage);
	DeleteObject(hbm);

	return true;
}
*/
bool ddSpriteClass::loadBmpFile(char* path, bool videoMemory)
{
/*	return genericLoadBmp(path, false, 0, videoMemory);*/
	return true;
}

/*bool ddSpriteClass::loadBmpResource(char* resource, HINSTANCE hInstance, bool videoMemory)
{
	return genericLoadBmp(resource, true, hInstance, videoMemory);
}*/

// create an 8 bits surface from an RLE (run length encoding, as in PCX),
// Currently, this method assumes that rleLen is right, so it does
// not check it to see whether the image is valid or not.
// IMPORTANT NOTE ABOUT THE bytesPerLine PARAMETER:
// "bytesPerLine" is, supposedly, the width of the image.
// However, in PCX files, it can be different, ie when the image width
// is an odd value. Because of that, both values have to be used
// differently: this way, bytesPerLine is only used for parsing the
// RLE data and creating the surface for the image obtained, and width
// is used for anything else (blitting and working with the image itself,
// for example).
// Unless you use the RLE methods directly (instead of the PCX functions),
// you shouldn't care about it, as this wrapper hides the bytesPerLine
// value under the name of realWidth, so helping the surfaces work
// coherently.
bool ddSpriteClass::createFromRLE8(
	BYTE* rle,// pointer to the memory with the RLE information (in 8 bits)
	DWORD rleLen,// size in bytes of the memory in 'rle'
	int bytesPerLine,// real width of the encoded image
	int width, int height,// size of the encoded image
	BYTE* palette,// palette as returned by convertPaletteTo8
	bool videoMemory)// is the surface going to be in video memory?
{
/*	DWORD pitch;// surface pitch in bytes
	BYTE* ptr;// ptr to the screen
	bool created=false;
	int x, y;
	int length;// for run length encoding... (compressed bytes)

	// variables for optimization in the decompression
	BYTE* ptrMax;
	BYTE colorTemp;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=8) return false;

	// create the surface needed... or work with the existent one
	if (!initialized) {
		// create the surface
		if (!create(bytesPerLine, height, videoMemory))
			return false;// error creating the surface
		created=true;// set the flag
	}else{
		// is this surface able to handle the bitmap???
		if (realWidth<bytesPerLine || realHeight<height)
			return false;// the surface doesn't fit
	}

	// time to lock it as read only!
	if (!lock(false, true, &pitch, (LPVOID*)(&ptr))) {
		// it failed...
		// only terminate the surface if we've created it here
		if (created) free();
		return false;
	}

	// let's uncompress the RLE while blitting to the surface
	for (y=0; y<height; y++) {
		for (x=0; x<bytesPerLine;) {
			if (((*rle) & 192)==192) {
				// it's a compressed line... let's see its color and
				// size
				length=(*rle)&63;// size
				rle++;// *rle is the color
				x+=length;

				// if the RLE is wrong and tries to bypass the size
				// limits of the image, don't let it happen.
				if (x>bytesPerLine) {
					length-=x-bytesPerLine;
					x=bytesPerLine;
				}

				// draw the line
				ptrMax=ptr+length;// max pointer
				colorTemp=palette[*rle];// color to be blt
				for (; ptr<ptrMax; ptr++) {
					*ptr=colorTemp;
				}// for ptr

			}else{
				// it's a single pixel
				*ptr=palette[*rle];
				ptr++;
				x++;
			}

			// go to the next char
			rle++;
		}// for x

		// advance to the next line
		ptr+=(pitch-bytesPerLine);
	}// for y

	// it's ready, go away
	unlock();

	// update the "virtual" size of this surface
	xRes=width;
	yRes=height;
*/
	return true;// all ok
}// createFromRLE8

// create an 16 bits surface from an RLE (run length encoding, as in PCX),
// Currently, this method assumes that rleLen is right, so it does
// not check it to see whether the image is valid or not.
// IMPORTANT NOTE ABOUT THE bytesPerLine PARAMETER:
// "bytesPerLine" is, supposedly, the width of the image.
// However, in PCX files, it can be different, ie when the image width
// is an odd value. Because of that, both values have to be used
// differently: this way, bytesPerLine is only used for parsing the
// RLE data and creating the surface for the image obtained, and width
// is used for anything else (blitting and working with the image itself,
// for example).
// Unless you use the RLE methods directly (instead of the PCX functions),
// you shouldn't care about it, as this wrapper hides the bytesPerLine
// value under the name of realWidth, so helping the surfaces work
// coherently.
bool ddSpriteClass::createFromRLE16(
	BYTE* rle,// pointer to the memory with the RLE information (in 8 bits)
	DWORD rleLen,// size in bytes of the memory in 'rle'
	int bytesPerLine,// real width of the encoded image
	int width, int height,// size of the encoded image
	WORD* palette,// palette as returned by convertPaletteTo16
	bool videoMemory)// is the surface going to be in video memory?
{
/*	DWORD pitch;// surface pitch in bytes
	WORD* ptr;// ptr to the screen
	bool created=false;
	int x, y;
	int length;// for run length encoding... (compressed bytes)

	// variables for optimization in the decompression
	WORD* ptrMax;
	WORD colorTemp;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=16) return false;

	// create the surface needed... or work with the existent one
	if (!initialized) {
		// create the surface
		if (!create(bytesPerLine, height, videoMemory))
			return false;// error creating the surface
		created=true;// set the flag
	}else{
		// is this surface able to handle the bitmap???
		if (realWidth<bytesPerLine || realHeight<height)
			return false;// the surface doesn't fit
	}

	// time to lock it as read only!
	if (!lock(false, true, &pitch, (LPVOID*)(&ptr))) {
		// it failed...
		// only terminate the surface if we've created it here
		if (created) free();
		return false;
	}

	// let's uncompress the RLE while blitting to the surface
	for (y=0; y<height; y++) {
		for (x=0; x<bytesPerLine;) {
			if (((*rle) & 192)==192) {
				// it's a compressed line... let's see its color and
				// size
				length=(*rle)&63;// size
				rle++;// *rle is the color
				x+=length;

				// if the RLE is wrong and tries to bypass the size
				// limits of the image, don't let it happen.
				if (x>bytesPerLine) {
					length-=x-bytesPerLine;
					x=bytesPerLine;
				}

				// draw the line
				ptrMax=ptr+length;// max pointer
				colorTemp=palette[*rle];// color to be blt
				for (; ptr<ptrMax; ptr++) {
					*ptr=colorTemp;
				}// for ptr

			}else{
				// it's a single pixel
				*ptr=palette[*rle];
				ptr++;
				x++;
			}

			// go to the next char
			rle++;
		}// for x

		// advance to the next line
		ptr=(WORD*)(((BYTE*)(ptr))+pitch-bytesPerLine*2);
	}// for y

	// it's ready, go away
	unlock();

	// update the "virtual" size of this surface
	xRes=width;
	yRes=height;
*/
	return true;// all ok
}// createFromRLE16

// createFromRLE generic version
// Create a surface from an RLE (run length encoding, as in PCX),
// whatever the current video mode is (currently, only working for
// 8 and 16 bits).
// Currently, this method assumes that rleLen is right, so it does
// not check it to see whether the image is valid or not.
// IMPORTANT NOTE ABOUT THE bytesPerLine PARAMETER:
// "bytesPerLine" is, supposedly, the width of the image.
// However, in PCX files, it can be different, ie when the image width
// is an odd value. Because of that, both values have to be used
// differently: this way, bytesPerLine is only used for parsing the
// RLE data and creating the surface for the image obtained, and width
// is used for anything else (blitting and working with the image itself,
// for example).
// Unless you use the RLE methods directly (instead of the PCX functions),
// you shouldn't care about it, as this wrapper hides the bytesPerLine
// value under the name of realWidth, so helping the surfaces work
// coherently.
bool ddSpriteClass::createFromRLE(
	BYTE* rle,// pointer to the memory with the RLE information (in 8 bits)
	DWORD rleLen,// size in bytes of the memory in 'rle'
	int bytesPerLine,// real width of the encoded image
	int width, int height,// size of the encoded image
	BYTE* palette,// raw palette (as saved in PCX)
	bool videoMemory)// is the surface going to be in video memory?
{
/*	BYTE* palConv8;
	WORD* palConv16;
	bool allOk;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=16 && bpp!=8) return false;

	// now let's convert the palette to our video mode
	switch (bpp) {
	case 8:
		// let's create an 8 bits palette
		palConv8=new BYTE[256];

		// convert it
		if (!DDObject->convertPaletteTo8(palette, palConv8, 256)) {
			delete[] palConv8;
			return false;// it failed
		}
		break;
	case 16:
		// let's create a 16 bits palette
		palConv16=new WORD[256];

		// convert it
		if (!DDObject->convertPaletteTo16(palette, palConv16, 256)) {
			delete[] palConv16;
			return false;// it failed
		}
		break;
	}

	// now go create the surface
	switch(bpp) {
	case 8:
		// create the surface itself
		allOk=createFromRLE8(
			rle, rleLen, bytesPerLine, width, height,
			palConv8, videoMemory);
		break;
	case 16:
		// create the surface itself
		allOk=createFromRLE16(
			rle, rleLen, bytesPerLine, width, height,
			palConv16, videoMemory);
		break;
	}

	// destroy our converted palette
	switch(bpp) {
	case 8:
		delete[] palConv8;
		break;
	case 16:
		delete[] palConv16;
		break;
	}

	return allOk;// return error code or ok*/
	return true;
}// createFromRLE

// createFromPCX
// Create a surface from a 8 bits PCX file content.
// Currently, this method assumes that the file is valid, so no much
// checks are done. Invalid files could lead the PC to halt.
// Currently, PCX can only be loaded in 8 and 16 screen video modes.
bool ddSpriteClass::createFromPCX(
	BYTE* pcx,// pointer to the data of a 8 bits PCX file
	DWORD pcxLen,// size of the file
	bool videoMemory)// is the surface going to be in video memory?
{
/*	WORD* pcxWord=(WORD*)pcx;
	BYTE* palette;
	BYTE* rleInfo;
	DWORD rleLen;
	WORD width, height, bytesPerLine;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=16 && bpp!=8) return false;

	// check if the file is big enough as to carry any data
	// 768= 256 colors * 3 bytes per color... space for the palette
	// 120= size of the header
	if (pcxLen<=768+128) return false;

	// go read the header, check it's a valid PCX file
	if (
		(pcx[0]!=10) || // PCX identifier
		(pcx[1]!=5) || // PCX version we parse
		(pcx[2]!=1) || // encoding, has to be 1
		(pcx[3]!=8) || // bpp
		(pcx[65]!=1)) // number of color planes

		return false;// it's not a valid PCX file

	// go get some important data
	// width=xMax-xMin+1
	width=pcxWord[4]-pcxWord[2]+1;
	// height=yMax-yMin+1
	height=pcxWord[5]-pcxWord[3]+1;
	// it's the real width a surface has to have for us to interpret
	// the RLE code...
	bytesPerLine=pcxWord[33];
	// the palette is the last thing on the entire file
	palette=pcx+pcxLen-768;
	// rle (run length encoding) image, it's just after the header
	rleInfo=pcx+128;
	// size of the rle data (the entire file, less the palette and the header
	rleLen=pcxLen-128-768;

	// now create the surface, if possible, and return its error code
	return createFromRLE(
		rleInfo, rleLen, bytesPerLine, width, height, palette, videoMemory);*/
	return true;
}// createFromPCX

// createFromGeneralImage
// Create a surface from a general image file.
bool ddSpriteClass::createFromGeneralImage(
	BYTE* filename,// pointer to the data of a 8 bits PCX file
	bool videoMemory)// is the surface going to be in video memory?
{

	if (!isDDReady || initialized) return false;// direct draw wasn't initialized

	srfOffScreen=IMG_Load((char*)filename);
	if (!srfOffScreen) return false;

	xRes=srfOffScreen->w;
	yRes=srfOffScreen->h;
	realWidth=xRes;
	realHeight=yRes;
	inVideoMemory=videoMemory;
	initialized=true;

	return true;
}// createFromGeneralImage

// Create a surface from a Pcx file.
// Currently, PCX can only be loaded in 8 and 16 screen video modes.
bool ddSpriteClass::loadPcxFile(
	char* path,// path of the Pcx file
	bool videoMemory)// is the surface going to be in video memory?
{
/*	HANDLE hFile;
	BYTE* pcxData;
	DWORD fileSize;
	HANDLE fileMap;
	bool allOk;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=16 && bpp!=8) return false;

	// Open the file for reading
	if ((hFile=CreateFile(
		path,//name of the file
		GENERIC_READ,//desired access
		FILE_SHARE_READ,//share mode
		NULL,//security attributes
		OPEN_EXISTING,//creation disposition
		FILE_ATTRIBUTE_NORMAL,//flags and attr
		NULL)//template file
		)==INVALID_HANDLE_VALUE)

		return false;

	// get the file size
	if ((fileSize=GetFileSize(hFile,NULL))==0xFFFFFFFF) {

		// failed
		CloseHandle(hFile);
		return false;
	}

	// create the file mapping object
	if ((fileMap=CreateFileMapping(
		hFile,//handle
		NULL,//security
		PAGE_READONLY,//flProtect
		0,0,//max size
		NULL))//name
		==NULL) {

		// failed
		CloseHandle(hFile);
		return false;
	}

	// obtain a pointer to the file
	if ((pcxData=(BYTE*)MapViewOfFile(
		fileMap,FILE_MAP_READ,0,0,0))
		==NULL) {

		// failed
		CloseHandle(fileMap);
		CloseHandle(hFile);
		return false;
	}

	// create the surface, if possible
	allOk=createFromPCX(pcxData, fileSize, videoMemory);

	// close the file
	UnmapViewOfFile(pcxData);
	CloseHandle(fileMap);
	CloseHandle(hFile);

	// time to go away
	return allOk;*/
	return true;
}// loadPcxFile

// Create a surface from a Pcx resource.
// Currently, PCX can only be loaded in 8 and 16 screen video modes.
bool ddSpriteClass::loadPcxResource(
	char* resource,// name of the resource
	char* resourceType,// type of the custom resource...
	bool videoMemory)// is the surface going to be in video memory?
{
/*	HRSRC resInfo;// resource as obtained by FindResource
	HGLOBAL glD;// global data
	BYTE* pcxData;
	DWORD pcxSize;
	bool allOk;

	if (!isDDReady) return false;// direct draw wasn't initialized

	// check the bits per pixel
	if (bpp!=16 && bpp!=8) return false;

	// look for the resource
	if ((resInfo=FindResource(
		NULL,// module,
		resource,// resource name
		resourceType// resource type
		))==NULL)
		return false;// error: there is no resource named that way

	// get its size
	if ((pcxSize=SizeofResource(
		NULL,// module
		resInfo// resource info object
		))==0)
		return false;// error

	// load it...
	if ((glD=LoadResource(
		NULL,// module
		resInfo// data returned by FindResource
		))==NULL)
		return false;// error...

	// let's obtain a pointer to the resource
	if ((pcxData=(BYTE*)LockResource(glD))==NULL) {
		// let's go out
		FreeResource(glD);
		return false;// error!
	}

	// create the surface
	allOk=createFromPCX(pcxData, pcxSize, videoMemory);

	// now it's time to free the locked resource (it will also unlock it)
	UnlockResource(glD);
	FreeResource(glD);

	// exit, returning the error code found in createFromPCX
	return allOk;*/
	return true;
}// loadPcxResource

// the following three can fail if the surface
// is lost
bool ddSpriteClass::setBlackTransparentColor()
{
	DWORD color;

	if (!getDwordFromRGB(0, 0, 0, color)) return false;
	return setTransparentColorFromDword(color);
}

bool ddSpriteClass::setWhiteTransparentColor()
{
	DWORD color;

	if (!getDwordFromRGB(255, 255, 255, color)) return false;
	return setTransparentColorFromDword(color);
}

// This method sets the transparent color of the surface, from the
// color that results from the combination of red, green and blue
// values.
bool ddSpriteClass::setTransparentColorFromRGB(BYTE r, BYTE g, BYTE b)
{
	DWORD color;

	if (!getDwordFromRGB(r, g, b, color)) return false;
	return setTransparentColorFromDword(color);
}// setTransparentColorFromRGB

// This method sets the transparent color of the surface, from the
// color in a given pixel.
bool ddSpriteClass::setTransparentColorFromPixel(int x, int y)
{
	DWORD color;

	if (!getDwordFromPixel(x, y, color)) return false;
	return setTransparentColorFromDword(color);
}// setTransparentColorFromDword

// This method sets the transparent color of the surface, from a
// DWORD (useful for blittings).
bool ddSpriteClass::setTransparentColorFromDword(DWORD color)
{
/*	DDCOLORKEY colorKey={sizeof(colorKey)};// and zero it!

	colorKey.dwColorSpaceHighValue=color;
	colorKey.dwColorSpaceLowValue=color;

	if (FAILED(srfOffScreen->SetColorKey(
		DDCKEY_SRCBLT,// kind of color key
		&colorKey// color structure
		)))
		return false;

	// set the transparency info
	transparentDword=color;// remember the value of our transparent color
	transparent=true;// change flag
*/
	return true;
}

