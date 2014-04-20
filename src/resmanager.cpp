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

#include "resmanager.h"// for using the functions which manage the resources and operate their loading and unloading
#include "win-constants.h"// for defining those common types, like DWORD, etc
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "programzones.h"// for accessing to the code which manages the program zones and the switching between them

// Functions used by the Resources Manager

// setUpResources ************
// This function sets all the resources of a given zone up, while
// deallocating the resources that are not going to be used there.
bool setUpResources(
	programZonesEnum programZone)// program zone to look at

{
	resObjectDataStruct* ptrObj;
	resourceClass* resource;

	// we use this boolean for some specific cases where a resource
	// should be excepted from the load
	bool thisResourceExcepted;

	// DEINITIALIZER
	// Deinitialize all first, only keep in memory those resources
	// which are to be used in both programZones, and that are marked
	// with reallocateAlways==false.
	// Also, only unload those that say neverDeallocate==true, unless
	// it's going to the pgzon_emptyZone program zone.
	// for every node in the objects list...
	for (ptrObj=resObjectsList;
	ptrObj<resObjectsList+resObjectsListSize; ptrObj++) {

		resource=ptrObj->resObj;
		// check it can be deallocated now... (details are checked below)
		if (resource->isLoaded() &&
			(programZone==pgzon_emptyZone || !ptrObj->neverDeallocate))
		{

			if (ptrObj->reallocateAlways) {
				// it has to be reallocated always, unload it
				if (!resource->unload()) {
					// failed, log and exit
					logger.logLine(
						"Failed unloading %s for program zone %i",
						true, true, true,
						resource->getResourceName(), programZone);
					return false;
				}
			}else{
				// unload it only if it won't be used on the given
				// program zone
				if (!isResObjectInProgramZone(ptrObj, programZone)) {
					if (!resource->unload()) {
						// failed, log and exit
						logger.logLine(
							"Failed unloading %s for program zone %i",
							true, true, true,
							resource->getResourceName(), programZone);
						return false;
					}
				}
			}
		}
	}// for ptrObj


	// INITIALIZER
	// Now initialize all those resources which aren't still loaded,
	// and that belong to the given program zone.
	for (ptrObj=resObjectsList;
	ptrObj<resObjectsList+resObjectsListSize; ptrObj++) {

		resource=ptrObj->resObj;
		if (!resource->isLoaded()) {
			// EXCEPTION CHECK SPECIFIC BEGIN
			// check for exceptions
			thisResourceExcepted=false;

			// first exception: don't load an audio file if the
			// program sound isn't active
			if ((resource->getType()==FILE_RESOBJECTS_WAVE ||
				resource->getType()==FILE_RESOBJECTS_MIDI)
				&& !programSoundActive) {

				thisResourceExcepted=true;
			}
			// EXCEPTION CHECK SPECIFIC END

			// load it only if it's used in the given program zone
			// and it's not an exception to what should be loaded
			if (isResObjectInProgramZone(ptrObj, programZone) &&
				!thisResourceExcepted) {

				if (!resource->load()) {
					// failed the loading, check whether it's a
					// necessary resource or not
					if (ptrObj->isNecessary) {
						// failed, log and exit
						logger.logLine(
							"Failed loading %s for program zone %i",
							true, true, true,
							resource->getResourceName(), programZone);
						return false;
					}else{
						// never mind, log the fault and continue
						logger.logLine(
							"Failed loading %s for program zone %i, system will continue execution (unnecessary resource)",
							true, true, true,
							resource->getResourceName(), programZone);
					}
				}
			}
		}
	}// for ptrObj

	return true;// all ok
}// setUpResources

// reloadAllLostResources ****
// This function reloads all the resources which are currently loaded,
// but which have been lost (that is, they need to be reloaded). Useful
// for being executed after a switch between programs, for example,
// when graphic surfaces and other resources are lost.
bool reloadAllLostResources()
{
	resObjectDataStruct* ptrObj;
	resourceClass* resource;

	// for every node in the objects list, check if it's loaded and
	// lost, and in such a case, reload it
	for (ptrObj=resObjectsList;
	ptrObj<resObjectsList+resObjectsListSize; ptrObj++) {

		resource=ptrObj->resObj;
		if (resource->isLoaded() && resource->needsToBeReloaded()) {
			if (!resource->reload()) {
				logger.logLine("Failed reloading %s",
					true, true, true,
					resource->getResourceName());
				return false;
			}
		}
	}// for ptrObj

	return true;// all ok
}// reloadAllLostResources
