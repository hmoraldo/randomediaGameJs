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

#include "soundman.h"// for using the functions related to sound management
#include "gameglobals.h"// for using the game objects
#include "globals.h"// for using the program objects


// stopAllSoundResources **********
// This function stops all the sounds in the resources list that are
// currently in execution
bool stopAllSoundResources()
{
#ifdef SOUND_ENABLED
	int nodeIdx;// node in the resources list
	waveResourceClass* waveObj;// a wave object...
	midiResourceClass* midiObj;// a midi object...

	// for every node in the objects list...
	for (nodeIdx=0; nodeIdx<resObjectsListSize; nodeIdx++) {
		// check it's a wave file
		if (resObjectsList[nodeIdx].resObj->getType()
			==FILE_RESOBJECTS_WAVE) {

			// it's a wave file...
			waveObj=(waveResourceClass*)resObjectsList[nodeIdx].resObj;

			// stop it if already loaded...
			if (waveObj->isLoaded())
				waveObj->getObjectPointer()->stop();
		} else if (resObjectsList[nodeIdx].resObj->getType()
			==FILE_RESOBJECTS_MIDI) {

			// it's a midi file...
			midiObj=(midiResourceClass*)resObjectsList[nodeIdx].resObj;

			// stop it if already loaded...
			if (midiObj->isLoaded())
				midiObj->getObjectPointer()->stop();
		}
	}// for nodeIdx
#endif
	return true;// all ok
}// stopAllSoundResources

// playSoundResource ***********
// This function looks for a sound resource with the given name, and
// plays it. Only returns false for errors.
// Midi resources aren't allowed to be played infinitely, so setting
// infinite to true for a midi resource returns an error.
bool playSoundResource(char* resourceName, bool infinite)
{
#ifdef SOUND_ENABLED
	resourceClass* genRes;
	waveResourceClass* waveRes;
	midiResourceClass* midiRes;

	genRes=getResObjectByName(resourceName);
	if (genRes==NULL) return false;

	// don't play the sound if we shouldn't do it now
	if (!programSoundEnabled) return true;

	switch (genRes->getType()) {
	case FILE_RESOBJECTS_WAVE:
		// it's a wave resource, play it
		waveRes=(waveResourceClass*)genRes;
		waveRes->getObjectPointer()->play(infinite);
		break;
	case FILE_RESOBJECTS_MIDI:
		// it's a midi resource
		if (infinite) return false;// not allowed in midis
		midiRes=(midiResourceClass*)genRes;
		midiRes->getObjectPointer()->play();
		break;
	default:
		return false;// object type not allowed
		break;
	}
#endif

	return true;// all ok
}// playSoundResource

// stopSoundResource ***********
// This function looks for a sound resource with the given name, and
// stops it. Only returns false for errors.
bool stopSoundResource(char* resourceName)
{
#ifdef SOUND_ENABLED
	resourceClass* genRes;
	waveResourceClass* waveRes;
	midiResourceClass* midiRes;

	genRes=getResObjectByName(resourceName);
	if (genRes==NULL) return false;

	switch (genRes->getType()) {
	case FILE_RESOBJECTS_WAVE:
		// it's a wave resource, play it
		waveRes=(waveResourceClass*)genRes;
		waveRes->getObjectPointer()->stop();
		break;
	case FILE_RESOBJECTS_MIDI:
		// it's a midi resource
		midiRes=(midiResourceClass*)genRes;
		midiRes->getObjectPointer()->stop();
		break;
	default:
		return false;// object type not allowed
		break;
	}
#endif

	return true;// all ok
}// stopSoundResource

// isPlayingSoundResource ***********
// This function looks for a sound resource with the given name, and
// returns a boolean telling whether it's currently playing or not.
// errorCode is used for knowing whether the function has had errors
// during its execution or not (if it's null, it's ignored).
bool isPlayingSoundResource(char* resourceName, bool* errorCode)
{
#ifdef SOUND_ENABLED
	resourceClass* genRes;
	waveResourceClass* waveRes;
	midiResourceClass* midiRes;
	bool tmpErrCod=false;

	genRes=getResObjectByName(resourceName);
	if (genRes==NULL) {
		// failed
		tmpErrCod=false;
		if (errorCode!=NULL) (*errorCode)=tmpErrCod;
		return false;
	}

	switch (genRes->getType()) {
	case FILE_RESOBJECTS_WAVE:
		// it's a wave resource, play it
		waveRes=(waveResourceClass*)genRes;
		return waveRes->getObjectPointer()->isPlaying(errorCode);
	case FILE_RESOBJECTS_MIDI:
		// it's a midi resource
		midiRes=(midiResourceClass*)genRes;
		return midiRes->getObjectPointer()->isPlaying(errorCode);
	default:
		// failed
		tmpErrCod=false;
		if (errorCode!=NULL) (*errorCode)=tmpErrCod;
		return false;// object type not allowed
	}
#endif
	return true;
}// isPlayingSoundResource

// stopAndPlaySoundResource **********
// This function first stops, and then plays (so for playing a sound
// from the beginning, even if it's still being played at the moment),
// a given sound resource.
bool stopAndPlaySoundResource(char* resourceName)
{
#ifdef SOUND_ENABLED
	return (stopSoundResource(resourceName) &&
		playSoundResource(resourceName));
#endif
	return true;
}// stopAndPlaySoundResource
