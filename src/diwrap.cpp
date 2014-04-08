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


#include "diwrap.h"
#include <string.h>

// Every function here returns false in error...

// DIRECT INPUT BASE CLASS
// directInputBaseClass methods and stuff
int directInputBaseClass::initCount=0;
/*#if DIRECTINPUT_VERSION < 0x0700
	LPDIRECTINPUT directInputBaseClass::lpDI=NULL;
#else// DIRECTINPUT_VERSION
	LPDIRECTINPUT7 directInputBaseClass::lpDI=NULL;
#endif// DIRECTINPUT_VERSION
*/
// init
// initializes DirectInput if needed, keeping a "reference count"
// to see whether it's needed or not.
// Params:
// hInstance: instance of the application
bool directInputBaseClass::init()
{
	if (initCount==0) {
		// real init
	}
/*	if (initCount==0) {
#if DIRECTINPUT_VERSION < 0x0700
		if (FAILED(DirectInputCreate(
			hInstance, // hInstance
			DIRECTINPUT_VERSION, // version
			&lpDI,// new dinput object pointer
			NULL// don't care
			)))
			return false;
#else// DIRECTINPUT_VERSION
		if (FAILED(DirectInputCreateEx(
			hInstance, // hInstance
			DIRECTINPUT_VERSION, // version
			IID_IDirectInput7,// interface to create
			(VOID **) &lpDI,// new dinput object pointer
			NULL// don't care
			)))
			return false;
#endif// DIRECTINPUT_VERSION

	}
*/
	initCount++;

	return true;// all ok
}

// end
// Release the DirectInput object if it's needed.
bool directInputBaseClass::end()
{
	if (initCount==1) {
		// time to free it
//		lpDI->Release();
//		lpDI=NULL;
		// real deinit
	}

	initCount--;

	return true;// all ok
}

// DI KEYBOARD CLASS
// diKeyboardClass methods and stuff

// init
// Initializes DirectInput for the keyboard if needed.
// Params:
// hInstance: instance of the application
bool diKeyboardClass::init()
{
	if (initializedDI) return false;

	initializedDI=directInputBaseClass::init();

	return initializedDI;
}

// end
// Closes anything that remains open in the object, and then
// closes DirectInput if needed.
bool diKeyboardClass::end()
{
	if (!initializedDI) return false;

	if (initializedKB) {
		if (!closeKeyboard()) return false;
	}

	initializedDI=!directInputBaseClass::end();

	return !initializedDI;
}

FILE_DIWRAP_KEYS_TYPE *diKeyboardClass::createKeysArray(int numkeys)
{
	FILE_DIWRAP_KEYS_TYPE *tmpk;

	tmpk=new FILE_DIWRAP_KEYS_TYPE[numkeys];
	memset(tmpk, 0, sizeof(FILE_DIWRAP_KEYS_TYPE)*numkeys);// zero all the array

	return tmpk;
}


// setUpKeyboard
// Prepares the object to manage the keyboard.
// Params:
// hwnd: handle of the window that will manage it.
bool diKeyboardClass::setUpKeyboard()
{
	if (initializedKB) return false;
/*
	// create the device
	if (FAILED(lpDI->CreateDevice(
		GUID_SysKeyboard,// ref guid
		&lpDev,// pointer to the new device
		NULL// don't care
		)))
		return false;

	// set the keyboard data format
	if (FAILED(lpDev->SetDataFormat(
		&c_dfDIKeyboard// predefined data format
		))) {

		lpDev->Release();
		return false;
	}

	// set the cooperative level
	if (FAILED(lpDev->SetCooperativeLevel(
		hwnd,// window handle
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND// flags
		))) {

		lpDev->Release();
		return false;
	}

	// acquire the device... don't care if result is error
	acquire();
*/
	const int baseNumKeysUsed=256;
	maxNumKeysUsed=baseNumKeysUsed;

	// set up our arrays of keys
	keys=createKeysArray(maxNumKeysUsed);
	latestKeys=createKeysArray(maxNumKeysUsed);

	if (!getKeys(NULL)) return false;
	initializedKB=true;// change flag

	return true;// all ok
}

// closeKeyboard
// Closes the keyboard object.
bool diKeyboardClass::closeKeyboard()
{
	if (!initializedKB) return false;

	// bye bye all
	delete[] keys;
	delete[] latestKeys;
/*	unacquire();
	lpDev->Release();
*/
	initializedKB=false;// clear flag

	return true;// all ok
}

// acquire
// Prepares the keyboard to get the keys.
// Initialization does it automatically, redo if the input is lost.
// Note:
// Acquires aren't reference counted, and failed acquires are just
// a temporal problem
bool diKeyboardClass::acquire()
{
/*	return SUCCEEDED(lpDev->Acquire());*/
	return true;//2010
}

// unacquire
// Release the keyboard until the next acquire.
bool diKeyboardClass::unacquire()
{
/*	lpDev->Unacquire();*/
	return true;// always ok
}

// getKeys
// Take the current state of the keyboard. After it, keyboard stuff
// can be known through the KEYDOWN macro.
// If this method returns false, and isLost isn't NULL, the variable
// pointed by isLost will be true if the error was caused by an
// unaquired device (which needs the acquire method to be called), or
// false if the error is anything else.
bool diKeyboardClass::getKeys(bool* isLost)
{
	FILE_DIWRAP_KEYS_TYPE* tmpKeys;
	int numkeys;

/*	HRESULT res;

	if (!isReady()) return false;

	// update the latest state information
	memcpy(latestKeys, keys, FILE_DIWRAP_KEYS_SIZE);

	// get the current state of the device
	res=lpDev->GetDeviceState(
		FILE_DIWRAP_KEYS_SIZE,// size of the buffer (256)
		(LPVOID *)keys// address of the buffer
		);

	// check for lost input
	if (res==DIERR_INPUTLOST) {
		// input lost
		if (isLost!=NULL) *isLost=true;
	}else{
		// input not lost
		if (isLost!=NULL) *isLost=false;
	}

	// check for errors
	if (FAILED(res))
		return false;
*/
	#ifdef EMSCRIPTEN
		tmpKeys=SDL_GetKeyboardState(&numkeys);
	#else
		tmpKeys=SDL_GetKeyState(&numkeys);
	#endif
	
	// if we need bigger buffers, recreate them
	if (numkeys>maxNumKeysUsed) {
		maxNumKeysUsed=numkeys;

		// todo 2010, this erases previous key data!!
		delete[] keys;
		delete[] latestKeys;

		keys=createKeysArray(maxNumKeysUsed);
		latestKeys=createKeysArray(maxNumKeysUsed);
	}

	memcpy(latestKeys, keys, sizeof(FILE_DIWRAP_KEYS_TYPE)*maxNumKeysUsed);// latestKeys=keys
	memcpy(keys, tmpKeys, sizeof(FILE_DIWRAP_KEYS_TYPE)*maxNumKeysUsed);// keys=tmpKeys

	return true;// all ok
}

// DI MOUSE CLASS
// diMouseClass methods and stuff

// init
// Initializes DirectInput for the mouse if needed.
// Params:
// hInstance: instance of the application
bool diMouseClass::init()
{
	if (initializedDI) return false;

	initializedDI=directInputBaseClass::init();

	return initializedDI;
}

// end
// Closes anything that remains open in the object, and then
// closes DirectInput if needed.
bool diMouseClass::end()
{
	if (!initializedDI) return false;

	if (initializedM) {
		if (!closeMouse()) return false;
	}

	initializedDI=!directInputBaseClass::end();

	return !initializedDI;
}

// setUpMouse
// Prepares the object to manage the mouse.
// Params:
// hwnd: handle of the window that will manage it.
bool diMouseClass::setUpMouse()
{
	mouseData.mouse_abs_x=0;
	mouseData.mouse_abs_y=0;
	mouseData.mouse_x=0;
	mouseData.mouse_y=0;
	mouseData.mouse_state=0;

/*	if (initializedM) return false;

	// create the device
	if (FAILED(lpDI->CreateDevice(
		GUID_SysMouse,// ref guid
		&lpDev,// pointer to the new device
		NULL// don't care
		)))
		return false;

	// set the mouse data format
	if (FAILED(lpDev->SetDataFormat(
		&c_dfDIMouse// predefined data format
		))) {

		lpDev->Release();
		return false;
	}

	// set the cooperative level
	if (FAILED(lpDev->SetCooperativeLevel(
		hwnd,// window handle
		DISCL_EXCLUSIVE | DISCL_FOREGROUND// flags
		))) {

		lpDev->Release();
		return false;
	}

	// zero the buffers, the information in these isn't reliable at all
	memset(&mouseData, 0, sizeof(mouseData));
	memset(&latestMouseData, 0, sizeof(latestMouseData));

	// acquire the device... don't care if result is error
	acquire();
*/
	initializedM=true;// change flag

	return true;// all ok
}

// closeMouse
// Closes the mouse object.
bool diMouseClass::closeMouse()
{
	if (!initializedM) return false;
/*
	// bye bye all
	unacquire();
	lpDev->Release();
*/
	initializedM=false;// clear flag

	return true;// all ok
}

// acquire
// Prepares the mouse to get the keys.
// Initialization does it automatically, redo if the input is lost.
// Note:
// Acquires aren't reference counted, and failed acquires are just
// a temporal problem
bool diMouseClass::acquire()
{
/*	return SUCCEEDED(lpDev->Acquire());*/
	return true;
}

// unacquire
// Release the mouse until the next acquire.
bool diMouseClass::unacquire()
{
/*	lpDev->Unacquire();*/
	return true;// always ok
}

// getState
// Take the current state of the mouse. After it, mouse stuff
// can be known through the MOUSE* macros.
// If this method returns false, and isLost isn't NULL, the variable
// pointed by isLost will be true if the error was caused by an
// unaquired device (which needs the acquire method to be called), or
// false if the error is anything else.
bool diMouseClass::getState(bool* isLost)
{
/*	HRESULT res;*/
	mouseDataStruct latestMouseDataCopy;

	if (!isReady()) return false;
	if (isLost!=NULL) *isLost=false;//2010
/*
	// update the latest state information
	memcpy(&latestMouseData, &mouseData, sizeof(mouseData));

	// get the state of this device
	res=lpDev->GetDeviceState(
		sizeof(mouseData),// size of the buffer
		(LPVOID *)&mouseData// address of the buffer
		);

	// check for lost input
	if (res==DIERR_INPUTLOST) {
		// input lost
		if (isLost!=NULL) *isLost=true;
	}else{
		// input not lost
		if (isLost!=NULL) *isLost=false;
	}

	// check for errors
	if (FAILED(res))
		return false;
*/
	latestMouseDataCopy=latestMouseData;

	latestMouseData=mouseData;
	mouseData.mouse_state=SDL_GetMouseState(&(mouseData.mouse_abs_x), &(mouseData.mouse_abs_y));
	mouseData.mouse_x=mouseData.mouse_abs_x-latestMouseDataCopy.mouse_abs_x;
	mouseData.mouse_y=mouseData.mouse_abs_y-latestMouseDataCopy.mouse_abs_y;

	return true;// all ok
}
