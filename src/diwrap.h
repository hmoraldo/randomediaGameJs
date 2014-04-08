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


#ifndef FILE_DIWRAP_INCLUDED
#define FILE_DIWRAP_INCLUDED

#define FILE_DIWRAP_VERSION "11-11-2002-H"// last modification: dd/mm/yy

// FILES TO ADD TO THE PROJECT FILE:
// This wrapper needs dinput.lib and dxguid.lib
// from the "lib" directory of your compiler,
// to be added to the project it's being used in,
// otherwise it won't compile.

#include <SDL.h>
#include "win-constants.h"
#include "types.h"

// If you want your code to be fully compatible with DX 5, keep this line
// uncommented... Otherwise, you'll need to comment it, and the generated
// code will only work for DirectX 7 and above.
// For code compatible with DX 6, keep this line uncommented.
// You can also try changing this number to make it be other values.
#define DIRECTINPUT_VERSION 0x0500 // comment it if necessary
#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0700
#endif // DIRECTINPUT_VERSION

typedef Uint8 FILE_DIWRAP_KEYS_TYPE;
typedef int FILE_DIWRAP_SINGLEKEY_TYPE;

// Macros for the keyboard
// buffer is: keyObject.keys
// or keyObject.latestKeys if looking for the information of the latest
// state of the keyboard
// Is a given key pressed?
#define KEYDOWN(buffer, key) ((buffer)[(key)])
// The following macros are useful for obtaining the dynamic keyboard
// state rather than the static one. All of these need to compare the
// new buffer with the old one (keyObject.keys vs. keyObject.latestKeys).
// Was that key just pressed?
#define KEYEVENTDOWN(newBuffer, oldBuffer, key) \
	((KEYDOWN((newBuffer), (key))) && (!KEYDOWN((oldBuffer), (key))))
// Was that key just released?
#define KEYEVENTUP(newBuffer, oldBuffer, key) \
	((!KEYDOWN((newBuffer), (key))) && (KEYDOWN((oldBuffer), (key))))

#define MOUSE_BUTTON_LEFT 1

// Macros for the mouse
// buffer is: mouseObject.mouseData
// or mouseObject.latestMouseData if looking for the information of the
// latest mouse state
// Is a given mouse button pressed?
#define MOUSEDOWN(buffer, button) (((buffer).mouse_state) & SDL_BUTTON(button))
// What are the current mouse coordinates?
#define MOUSEX(buffer) ((buffer).mouse_x)
#define MOUSEY(buffer) ((buffer).mouse_y)
// The following macros are useful for obtaining the dynamic mouse
// state rather than the static one. All of these need to compare the
// new buffer with the old one (mouseObject.mouseData vs.
// mouseObject.latestMouseData).
// Was that button just pressed?
#define MOUSEEVENTDOWN(newBuffer, oldBuffer, button) \
	((MOUSEDOWN((newBuffer), (button))) && (!MOUSEDOWN((oldBuffer), (button))))
// Was that button just released?
#define MOUSEEVENTUP(newBuffer, oldBuffer, button) \
	((!MOUSEDOWN((newBuffer), (button))) && (MOUSEDOWN((oldBuffer), (button))))

// used for storing the buffered mouse data
struct mouseDataStruct{
	int mouse_x, mouse_y;// relative position
	int mouse_abs_x, mouse_abs_y;// absolute position
	Uint8 mouse_state;
};

// directInputBaseClass
// That's the pure virtual class that wraps the init and ending
// of the DirectInput object
class directInputBaseClass{
private:
	static int initCount;
protected:
/*#if DIRECTINPUT_VERSION < 0x0700
	static LPDIRECTINPUT lpDI;
#else// DIRECTINPUT_VERSION
	static LPDIRECTINPUT7 lpDI;
#endif// DIRECTINPUT_VERSION*/
	bool init();
	bool end();
public:
	directInputBaseClass() {}
	virtual ~directInputBaseClass() {}

	virtual bool isReady()=0;
};

// diKeyboardClass
// That's the class that wraps the use of the keyboard through
// DirectInput.
class diKeyboardClass:public directInputBaseClass{
private:
	bool initializedDI;// is dinput initialized?
	bool initializedKB;// is the keyboard initialized?

	int maxNumKeysUsed;
/*	LPDIRECTINPUTDEVICE lpDev;*/

	FILE_DIWRAP_KEYS_TYPE *createKeysArray(int numkeys);

	// the = operator shouln't be used
	// the same with the copy constructor
	diKeyboardClass& operator=(diKeyboardClass& from) {return *this;}
	diKeyboardClass(diKeyboardClass& from) {}
public:
	FILE_DIWRAP_KEYS_TYPE *latestKeys;
	FILE_DIWRAP_KEYS_TYPE *keys;// buffer for the latest keys, previous to the ones at keys

	diKeyboardClass():initializedDI(false), initializedKB(false) {}
	~diKeyboardClass() {end();}

	bool isReady() {return initializedDI && initializedKB;}

	bool init();
	bool end();

	bool setUpKeyboard();
	bool closeKeyboard();

	bool acquire();
	bool unacquire();

	bool getKeys(bool* isLost=NULL);
};

// diMouseClass
// That's the class that wraps the use of the mouse through
// DirectInput
class diMouseClass:public directInputBaseClass{
private:
	bool initializedDI;// is dinput initialized?
	bool initializedM;// is the mouse initialized?

/*	LPDIRECTINPUTDEVICE lpDev;*/

	// the = operator shouln't be used
	// the same with the copy constructor
	diMouseClass& operator=(diMouseClass& from) {return *this;}
	diMouseClass(diMouseClass& from) {}
public:
/*	DIMOUSESTATE mouseData;
	DIMOUSESTATE latestMouseData;// buffer for the latest mouse info, previous to the ones at mouseData*/
	mouseDataStruct mouseData;// buffer for the latest mouse info, previous to the ones at mouseData
	mouseDataStruct latestMouseData;// buffer for the latest mouse info, previous to the ones at mouseData

	diMouseClass():initializedDI(false), initializedM(false) {}
	~diMouseClass() {end();}

	bool isReady() {return initializedDI && initializedM;}

	bool init();
	bool end();

	bool setUpMouse();
	bool closeMouse();

	bool acquire();
	bool unacquire();

	bool getState(bool* isLost=NULL);
};

#endif // FILE_DIWRAP_INCLUDED
