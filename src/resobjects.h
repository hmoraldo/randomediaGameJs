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
RESOBJECTS.*:


These files contain the classes hierarchy for resource objects, where
resource objects are actually objects that contain, for example, a
ddSpriteClass, midiMusicClass, or waveFileClass object.

*/

#ifndef FILE_RESOBJECTS_INCLUDED
#define FILE_RESOBJECTS_INCLUDED

#define FILE_RESOBJECTS_VERSION "1-10-2002-H"// last modification: dd/mm/yy

// some useful header files
#include "ddwrap.h"
/*#include "audiowrap.h"*/

// These are the possible types for a resourceClass object.
enum resourceTypeEnum {
	FILE_RESOBJECTS_GRAPHIC,// it's a ddSurfaceClass object
	FILE_RESOBJECTS_MIDI,// it's a midiMusicClass object
	FILE_RESOBJECTS_WAVE// it's a waveFileClass object
};// resourceTypeEnum


// RESOURCE CLASS
// This is the class whose objects contain one of multiple resource
// types.
class resourceClass{
private:
protected:
	// Some characteristics of the object
	char* resourceName;// the name of this resource
public:
	// Constructor / destructor
	resourceClass():resourceName(NULL) {}
	virtual ~resourceClass() {}

	// Some basic interfaces
	virtual bool needsToBeReloaded()=0;// tells whether it should be reloaded before using
	virtual bool isLoaded()=0;// tells whether the object is currently loaded or not
	virtual resourceTypeEnum getType()=0;// tells the type of this resource
	virtual char* getResourceName() {return resourceName;}// tell the name of this resource

	// Basic object manipulation
	virtual bool load()=0;// loads the resource
	virtual bool reload()=0;// reloads the resource
	virtual bool unload()=0;// unloads the resource
};// resourceClass


// GRAPHIC RESOURCE CLASS
// This is the class for graphic objects
class graphicResourceClass:public resourceClass{
private:
	ddSpriteClass surface;// surface where the graphic is stored

	// Information for loading
	char* fileName;// file from where we'll take the graphic when loading
	char* resourceType;// if the graphic is stored in a pcx resource, it could need to know the resource type
	bool isFileOrResource;// true if it's a file, and not a resource
	bool allocateInTheFastestMemory;// let the object decide whether to go to video or system memory, according to where it should blt faster? it supposes isTransparent=true if it's going to be blt transparently always
	bool useVideoMemory;// should the surface be stored in video memory, whenever possible?
	bool isBMPorPCX;// true if it's a bmp file, and not a pcx file
	bool isTransparent;// does it use a transparent background?
	bool usesTransparentRGB;// does it use the three color components for the transparent color, or just the color coordinates? true for a dword
	int xTransparent, yTransparent;// coordinates of the transparent color
	int transparentR, transparentG, transparentB;// components of the transparent color
	int desiredWidth, desiredHeight;// desired width and height for the image, in the case filename=false

	// Some auxiliary functions
	virtual bool rawLoad();// load the resource itself, and nothing more
protected:
public:
	// Constructor / destructor
	graphicResourceClass(char* resName, char* fileName, bool allocateInTheFastestMemory, bool useVideoMemory, bool isFileOrResource, char* resourceType, bool isBMPorPCX, bool isTransparent, bool usesTransparentRGB, int transparentR, int transparentG, int transparentB, int xTransparent, int yTransparent, int desiredWidth, int desiredHeight);
	virtual ~graphicResourceClass() {unload();}

	// Some basic interfaces
	virtual bool needsToBeReloaded() {return surface.isLost();}// tells whether it should be reloaded before using
	virtual bool isLoaded() {return surface.isReady();};// tells whether the object is currently loaded or not
	virtual resourceTypeEnum getType() {return FILE_RESOBJECTS_GRAPHIC;}// tells the type of this resource
	ddSpriteClass* getObjectPointer() {return &surface;}// get a pointer to the surface object this resource contains

	// Basic object manipulation
	virtual bool load();// loads the resource
	virtual bool reload();// reloads the resource
	virtual bool unload();// unloads the resource
};// graphicResourceClass

/*
// WAVE RESOURCE CLASS
// This is the class for wave sound objects
class waveResourceClass:public resourceClass{
private:
	waveFileClass waveFile;

	// Information for loading
	char* fileName;// file from where we'll take the wave when loading
	char* fileOgg;// name for the file in ogg version
	char* fileTemp;// name for the file in its temporary version... (ogg decoding)
	char hardOrSoft;// a char telling where sounds will go, options are FILE_AUDIOWRAP_WAVE_DEFER, FILE_AUDIOWRAP_WAVE_HARDWARE and FILE_AUDIOWRAP_WAVE_SOFTWARE

	// Some auxiliary functions
	virtual bool rawLoad();// load the resource itself, and nothing more
protected:
public:
	// Constructors / destructor
	waveResourceClass(char* resName, char* fileName, char hardOrSoft);
	waveResourceClass(char* resName, char* oggFileName, char* fileName, char* fileTemp, char hardOrSoft);
	virtual ~waveResourceClass() {unload();}

	// Some functions to tell what the file names are...
	char* getFileName() {return fileName;}// get the name of the file...
	char* getFileTemp() {return fileTemp;}// get the name of the file...
	char* getOggFileName() {return fileOgg;}// get the name of the file in ogg version...

	// Some basic interfaces
	virtual bool needsToBeReloaded() {return waveFile.isLost();}// tells whether it should be reloaded before using
	virtual bool isLoaded() {return waveFile.isReady();};// tells whether the object is currently loaded or not
	virtual resourceTypeEnum getType() {return FILE_RESOBJECTS_WAVE;}// tells the type of this resource
	waveFileClass* getObjectPointer() {return &waveFile;}// get a pointer to the surface object this resource contains

	// Basic object manipulation
	virtual bool load();// loads the resource
	virtual bool reload();// reloads the resource
	virtual bool unload();// unloads the resource
};// waveResourceClass


// MIDI RESOURCE CLASS
// This is the class for midi music objects
class midiResourceClass:public resourceClass{
private:
	midiMusicClass midiMusic;

	// Information for loading
	char* fileName;// file from where we'll take the midi when loading
protected:
public:
	// Constructor / destructor
	midiResourceClass(char* resName, char* fileName);
	virtual ~midiResourceClass() {unload();}

	// Some basic interfaces
	virtual bool needsToBeReloaded() {return false;}// tells whether it should be reloaded before using
	virtual bool isLoaded() {return midiMusic.isReady();};// tells whether the object is currently loaded or not
	virtual resourceTypeEnum getType() {return FILE_RESOBJECTS_MIDI;}// tells the type of this resource
	midiMusicClass* getObjectPointer() {return &midiMusic;}// get a pointer to the midi music object this resource contains

	// Basic object manipulation
	virtual bool load();// loads the resource
	virtual bool reload() {return isLoaded();}// reloads the resource
	virtual bool unload();// unloads the resource
};// midiResourceClass
*/

#endif// FILE_RESOBJECTS_INCLUDED
