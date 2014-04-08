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


#ifndef FILE_DDWRAP_INCLUDED
#define FILE_DDWRAP_INCLUDED

#define FILE_DDWRAP_VERSION "4-8-2003-H"// last modification: dd/mm/yy

#include "win-constants.h"

// If you want your code to be fully compatible with DX 5, keep this line
// uncommented... Otherwise, you'll need to comment it, and the generated
// code will only work for DirectX 7 and above.
// For code compatible with DX 6, keep this line uncommented.
// You can also try changing this number to make it be other values.
#define DIRECTDRAW_VERSION 0x0500 // comment it if necessary
#ifndef DIRECTDRAW_VERSION
	#define DIRECTDRAW_VERSION 0x0700
#endif // DIRECTDRAW_VERSION

// FILES TO ADD TO THE PROJECT FILE:
// This wrapper needs ddraw.lib and dxguid.lib
// from the "lib" directory of your compiler,
// to be added to the project it's being used in,
// otherwise it won't compile.

#include <SDL.h>
#include "types.h"

// This macro converts a color given in RGB format (with 8 bits per
// component), to the color format of the surface, in a dword.
// Only for NON-palettized video modes.
// ddObj is a directDrawClass object.
// Remember that r8LessSize = 8-rSize.
#define getDwordFromRGBFast(ddObj, rrr, ggg, bbb) \
	(((rrr)>>((ddObj).r8LessSize))<<(ddObj).rOff)| \
	(((ggg)>>((ddObj).g8LessSize))<<(ddObj).gOff)| \
	(((bbb)>>((ddObj).b8LessSize))<<(ddObj).bOff)

// The following macros return each color component (in 8 bits) from a
// Dword containing a color value, according to the surface color format.
// Only for NON-palettized video modes.
// ddObj is a directDrawClass object.
// Remember that r8LessSize = 8-rSize.
#define getRedFromDword(ddObj, color) \
	((((color) & (ddObj).rMask)>>(ddObj).rOff)<<(ddObj).r8LessSize)
#define getGreenFromDword(ddObj, color) \
	((((color) & (ddObj).gMask)>>(ddObj).gOff)<<(ddObj).g8LessSize)
#define getBlueFromDword(ddObj, color) \
	((((color) & (ddObj).bMask)>>(ddObj).bOff)<<(ddObj).b8LessSize)

enum alignEnum {FILE_DDWRAP_ALIGN_LEFT,
	FILE_DDWRAP_ALIGN_CENTER, FILE_DDWRAP_ALIGN_RIGHT};
enum valignEnum {FILE_DDWRAP_VALIGN_TOP,
	FILE_DDWRAP_VALIGN_CENTER, FILE_DDWRAP_VALIGN_BOTTOM};

class ddSurfaceClass;
class directDrawClass;
class ddSpriteClass;

class ddSurfaceClass{
private:
protected:
	int xRes, yRes;// size of the surface (useful part)
	int realWidth, realHeight;// real size of the surface (not of much use, for internal purposes mostly)
	static int bpp;
	bool inVideoMemory;// is in the video memory or in the system one?

	// transparent info will only be used in ddSpriteClass
	// bool transparent can only be true in ddSpriteClass and other
	// classes which could implement interfaces for this transparency
	// info.
	bool transparent;// is a transparent sprite
	DWORD transparentDword;// transparent color in dword format

	// pointer to our Direct Draw object
	static directDrawClass* DDObject;

	// info for locked surfaces
	bool locked;// is it locked?

	SDL_Surface* srfOffScreen;// main surface
/*
#if DIRECTDRAW_VERSION < 0x0700
	LPDIRECTDRAWSURFACE srfOffScreen;// surface for blitting
#else // DIRECTDRAW_VERSION
	LPDIRECTDRAWSURFACE7 srfOffScreen;// surface for blitting
#endif // DIRECTDRAW_VERSION
*/
	// two static variables that help to locate the direct draw
	// object
	static SDL_Surface* screenSurface;// direct draw object
	static bool isDDReady;
/*
	// two very useful methods, for the two versions of this class
#if DIRECTDRAW_VERSION < 0x0700
	virtual bool isLostSrf(bool* errorCode, LPDIRECTDRAWSURFACE& srf);
	virtual bool restoreSrf(LPDIRECTDRAWSURFACE& srf);
#else // DIRECTDRAW_VERSION
	virtual bool isLostSrf(bool* errorCode, LPDIRECTDRAWSURFACE7& srf);
	virtual bool restoreSrf(LPDIRECTDRAWSURFACE7& srf);
#endif // DIRECTDRAW_VERSION*/
public:
	ddSurfaceClass():locked(false), transparent(false) {}
	ddSurfaceClass(ddSurfaceClass&):locked(false), transparent(false) {}
	virtual ~ddSurfaceClass() {}

	// blitting methods, clipping in the destination surface (or not)
	virtual bool bltFrom(ddSurfaceClass& srf, int xD, int yD,
		bool clip=true, bool transparent=false);
	virtual bool bltFrom(ddSurfaceClass& srf, int xD, int yD, 
		int xS, int yS, int wS, int hS,
		bool clip=true, bool transparent=false);
	virtual bool bltFrom(ddSurfaceClass& srf, int xD, int yD, 
		int xS, int yS, int wS, int hS,
		int xClip, int yClip, int wClip, int hClip,
		bool clip=true, bool transparent=false);
	virtual bool stretchBltFrom(ddSurfaceClass& srf, int xD, int yD,
		int wD, int hD, int xS, int yS,	int wS, int hS,
		bool mirrorX=false, bool mirrorY=false, bool transparent=false);
	virtual bool obscuredBltFrom(ddSurfaceClass& srf, int xD, int yD,
		int lightness, bool transparent=false);
	virtual bool alphaBltFrom(ddSurfaceClass& srf, int xD, int yD,
		int alphaValue, bool transparent=false);
	// other graphical methods
	virtual bool writeText(char* text, int r, int g, int b, alignEnum align, int x, int y, int width=0, int height=0);
	virtual bool calculateText(char* text, alignEnum align, int& width, int& height);
	virtual bool fillRect(int r, int g, int b, int x, int y, int width=0, int height=0);
	virtual bool fillAllDword(DWORD color);
/*	virtual bool getDC(HDC* hdc);
	virtual bool releaseDC(HDC hdc);*/
	virtual bool getDwordFromPixel(int x, int y, DWORD& color);
	virtual bool getDwordFromRGB(int r, int g, int b, DWORD& color);
	virtual bool putPixel(int x, int y, int r, int g, int b);
	virtual bool drawLine(int x, int y, int x2, int y2, int r, int g, int b);

	// for saving the surface to disk
	virtual bool saveAsBmpFile(char* filename, bool overwrite=false);

	// Warning!
	// Don't play with locks under you really know what you are doing!
	virtual bool lock(bool onlyRead, bool onlyWrite, DWORD* pitch, LPVOID* pointer);
	virtual bool unlock();

	virtual bool isInVideoMemory() {return inVideoMemory;}
	virtual int getWidth() {return xRes;}
	virtual int getHeight() {return yRes;}
	virtual int getRealWidth() {return realWidth;}// don't use it unless you are real sure you need it
	virtual int getRealHeight() {return realHeight;}// don't use it unless you are real sure you need it
	virtual bool isLost(bool* errorCode=NULL);
	virtual bool restore();
	virtual bool isReady()=0;

	// a small function for obtaining the main DD object from any
	// surface attached to it.
	virtual directDrawClass* getDDObject() {return DDObject;}
};

class directDrawClass:public ddSurfaceClass{
private:
/*	// dd data
#if DIRECTDRAW_VERSION < 0x0700
	LPDIRECTDRAWSURFACE srfOnScreen;// main surface
#else// DIRECTDRAW_VERSION
	LPDIRECTDRAWSURFACE7 srfOnScreen;// main surface
#endif// DIRECTDRAW_VERSION
*/
	bool initialized;// was init executed ever?
	bool videoModeReady;// was the video mode initialized ever?
	bool palettized;// has this video mode a palette?
	int backbuffers;// how many back buffers does this video mode have?

public:
	bool init();
	bool end();

	bool setVideoMode(int x, int y, int bpp, int backbuffers=1);
	bool updateColorInfo();
	bool closeVideoMode();

	bool flip();

	// derived from ddSurfaceClass
	virtual bool isReady();
	virtual bool isLost(bool* errorCode=NULL);
	virtual bool restore();

	bool isVideoModeReady() {return videoModeReady;}
	bool isPalettized() {return palettized;}
	int getBackBuffersCount() {return backbuffers;}

	// for conversions of palette
	bool convertPaletteTo8(const BYTE* palette, BYTE* pal8, int entries);
	bool convertPaletteTo16(const BYTE* palette, WORD* pal16, int entries);

	// constructors / destructors
	directDrawClass():initialized(false), videoModeReady(false)
	{
		inVideoMemory=true;
	}
	~directDrawClass() {end();}

	// Important data for color handling
	// We assume it doesn't vary from a surface to other one, so
	// we put it here to be updated every time a video mode is set.
	// Color masks:
	DWORD rMask,gMask,bMask;
	// Offset for every color
	DWORD rOff,gOff,bOff;
	// Bits in every color
	DWORD rSize,gSize,bSize;
	DWORD r8LessSize, g8LessSize, b8LessSize;// used for faster calculations

/*	// Important data about the driver and HAL (hardware abstraction
	// layer) and the emulation capabilities of DirectX, in the HEL
	// (hardware emulation layer).
	// The emulation capabilities are only from DX, no HHM additions are
	// taken into account. And the helCaps don't change from machine to
	// machine, so they are hardly useful.
	// These caps are only updated in the initialization.
	DDCAPS halCaps;// hardware capabilities, read only data
	DDCAPS helCaps;// emulation capabilities, read only data

	// Some useful interfaces for retrieving info about the hardware
	// capabilities. When you can, use these instead of the caps
	// structures presented above.
	// You can use this information to decide whether a surface will go
	// in video or system memory, considering that hardware operations
	// generally work faster in video memory, and emulated operations
	// work faster in system memory.
	bool doesHardwareBlt() {// is blitting executed by the hardware?
		return (halCaps.dwCaps & DDCAPS_BLT)==DDCAPS_BLT;
	}// doesHardwareBlt
	bool doesHardwareTransparentBlt() {// is blitting with transparency executed by the hardware?
		return (halCaps.dwCKeyCaps & DDCKEYCAPS_SRCBLT)==DDCKEYCAPS_SRCBLT;
	}// doesHardwareTransparentBlt
	bool doesHardwareStretchX() {// is x stretching executed by the hardware?
		return (halCaps.dwFXCaps & DDFXCAPS_BLTSTRETCHX)==DDFXCAPS_BLTSTRETCHX;
	}// doesHardwareStretchX
	bool doesHardwareStretchY() {// is y stretching executed by the hardware?
		return (halCaps.dwFXCaps & DDFXCAPS_BLTSTRETCHY)==DDFXCAPS_BLTSTRETCHY;
	}// doesHardwareStretchY
	bool doesHardwareMirrorX() {// is x mirroring executed by the hardware?
		return (halCaps.dwFXCaps & DDFXCAPS_BLTMIRRORLEFTRIGHT)==DDFXCAPS_BLTMIRRORLEFTRIGHT;
	}// doesHardwareMirrorX
	bool doesHardwareMirrorY() {// is y mirroring executed by the hardware?
		return (halCaps.dwFXCaps & DDFXCAPS_BLTMIRRORUPDOWN)==DDFXCAPS_BLTMIRRORUPDOWN;
	}// doesHardwareMirrorY*/
};// directDrawClass

class ddSpriteClass:public ddSurfaceClass{
private:
	bool initialized;// does the surface exist?

/*	bool genericLoadBmp(char* name, bool isResource, HINSTANCE hInstance, bool videoMemory);*/
public:
	// constructors / destructors
	ddSpriteClass():initialized(false) {}
	~ddSpriteClass() {free();}

	bool create(int x, int y, bool videoMemory=true);
	bool free();
	bool loadBmpFile(char* path, bool videoMemory=true);
/*	bool loadBmpResource(char* resource, HINSTANCE hInstance, bool videoMemory=true);*/

	// methods for PCX
	bool createFromRLE8(BYTE* rle, DWORD rleLen, int bytesPerLine, int width, int height, BYTE* palette, bool videoMemory=true);
	bool createFromRLE16(BYTE* rle, DWORD rleLen, int bytesPerLine, int width, int height, WORD* palette, bool videoMemory=true);
	bool createFromRLE(BYTE* rle, DWORD rleLen, int bytesPerLine, int width, int height, BYTE* palette, bool videoMemory=true);
	bool createFromPCX(BYTE* pcx, DWORD pcxLen, bool videoMemory=true);
	bool loadPcxFile(char* path, bool videoMemory=true);
	bool loadPcxResource(char* resource, char* resourceType, bool videoMemory=true);
	bool createFromGeneralImage(BYTE* filename, bool videoMemory);

	bool isTransparent() {return transparent;}
	bool setBlackTransparentColor();
	bool setWhiteTransparentColor();
	bool setTransparentColorFromDword(DWORD color);
	bool setTransparentColorFromPixel(int x, int y);
	bool setTransparentColorFromRGB(BYTE r, BYTE g, BYTE b);

	// derived from ddSurfaceClass
	virtual bool isReady() {return initialized;}
};

#endif // FILE_DDWRAP_INCLUDED
