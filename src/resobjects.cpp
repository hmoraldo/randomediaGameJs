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

#include "resobjects.h"// for accessing to the resource objects (graphics, sounds, etc)


// GRAPHIC RESOURCE CLASS METHODS

// The parametrized constructor
graphicResourceClass::graphicResourceClass(
	char* resName,// resource name
	char* fileName,// file name, null for empty resource
	bool allocateInTheFastestMemory,// let the object decide whether to go to video or system memory, according to where it should blt faster? it supposes isTransparent=true if it's going to be blt transparently always
	bool useVideoMemory,// should the surface be stored in video memory, whenever possible?
	bool isFileOrResource,// true if it's a file, and not a resource
	char* resourceType,// if the graphic is stored in a pcx resource, it could need to know the resource type
	bool isBMPorPCX,// true if it's a bmp file, and not a pcx file
	bool isTransparent,// does it use a transparent background?
	bool usesTransparentRGB,// does it use the three color components for the transparent color, or just the color coordinates? true for a dword
	int transparentR, int transparentG, int transparentB,// components of the transparent color
	int xTransparent, int yTransparent,// coordinates of the transparent color
	int desiredWidth, int desiredHeight)// size for the image in the case it's supposed to be loaded empty (filename=NULL)
{
	// Set all up...
	resourceName=resName;
	graphicResourceClass::fileName=fileName;
	graphicResourceClass::allocateInTheFastestMemory=allocateInTheFastestMemory;
	graphicResourceClass::useVideoMemory=useVideoMemory;
	graphicResourceClass::isFileOrResource=isFileOrResource;
	graphicResourceClass::resourceType=resourceType;
	graphicResourceClass::isBMPorPCX=isBMPorPCX;
	graphicResourceClass::isTransparent=isTransparent;
	graphicResourceClass::usesTransparentRGB=usesTransparentRGB;
	graphicResourceClass::transparentR=transparentR;
	graphicResourceClass::transparentG=transparentG;
	graphicResourceClass::transparentB=transparentB;
	graphicResourceClass::xTransparent=xTransparent;
	graphicResourceClass::yTransparent=yTransparent;
	graphicResourceClass::desiredWidth=desiredWidth;
	graphicResourceClass::desiredHeight=desiredHeight;
}// constructor for graphicResourceClass

// rawLoad: our function for loading the resource this object contains.
// Only loads the file.
bool graphicResourceClass::rawLoad()
{
	// First check the case files are not needed at all
	if (fileName==NULL) {
		// In such case, there is nothing to load at all
		return true;// all ok
	}
	if (!surface.createFromGeneralImage((BYTE*)fileName, useVideoMemory))
		return false;

	return true;// all ok!
}// rawLoad

// Load: our function for loading and setting the resource this object
// contains up.
bool graphicResourceClass::load()
{
	// not even try to load the resource if it's already up and running
	if (isLoaded()) return true;

	// If it's needed, create the surface (it's only needed if it doesn't
	// require being loaded)
	if (fileName==NULL) {
		// create the surface...
		if (!surface.create(desiredWidth, desiredHeight, useVideoMemory))
			return false;
	}

	// Load the surface itself, and check there aren't errors.
	if (!rawLoad()) return false;

	return true;
}// load

// Reload: our function for setting the resource this object contains up,
// when it's already loaded, but needs to be reloaded.
bool graphicResourceClass::reload()
{
	// if the surface wasn't loaded before, return an error code
	if (!isLoaded()) return false;

	// don't ever bother reloading the surface, if it's not been lost
	// at all
	if (!needsToBeReloaded()) return true;

	// If the surface can't be restored, it's an error, but not an
	// important one, as it only indicates that's not the time for
	// restoring the surface yet, try again later. So we return our
	// "ok" error code, true, meaning it's all ok for us.
	if (!surface.restore()) return true;

	// Load the surface itself, and check there aren't errors.
	if (!rawLoad()) return false;

	return true;// all ok!
}// reload

// Unload: our function for unloading the resource this object contains.
bool graphicResourceClass::unload()
{
	// Don't ever bother unloading the resource if it's not loaded yet
	if (!isLoaded()) return true;

	return surface.free();
}// unload



