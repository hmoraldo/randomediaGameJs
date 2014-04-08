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

#include "postinstall.h"// for using the functions related to post-installation tasks
/*#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "winutil.h"// for accessing to some useful functions related to low level windows specific code
#include "vorbis2wav.h"// for converting ogg vorbis files to wav
*/

// executePostInstallTasks *************
// This funcion is supposed to be called the first time the program is
// executed after installation, when it's time to decompress the ogg
// files into wav, for example.
bool executePostInstallTasks(bool showMessages)
{
/*	int nodeIdx;// node in the resources list
	waveResourceClass* waveObj;// a wave object...
	char waitText[200];

	// Let's do the Ogg conversion to Wav
	// for every node in the objects list...
	for (nodeIdx=0; nodeIdx<resObjectsListSize; nodeIdx++) {
		// check it's a wave file
		if (resObjectsList[nodeIdx].resObj->getType()
			==FILE_RESOBJECTS_WAVE) {

			// it's a wave file, let's see what to do now
			waveObj=(waveResourceClass*)resObjectsList[nodeIdx].resObj;

			// check it needs to be converted
			if (waveObj->getOggFileName()!=NULL &&
				!WinUtilCode::fileExists(waveObj->getFileName())) {

				// Log it
				logger.logLine("Going to decode %s using Vorbis.",
					true, true, true,
					waveObj->getOggFileName());

				if (showMessages) {
					// Show a message on screen
					wsprintf(waitText,
						"-- RANDOMEDIA CONCEPT --\n"
						"Please wait while the installation is completed.\n"
						"Decompressing %s.",
						waveObj->getOggFileName());
					DD.fillAllDword(0);
					DD.writeText(waitText,
						255, 255, 255,
						FILE_DDWRAP_ALIGN_CENTER,
						0, 120, 0, 0);
					DD.flip();
				}

				// Now it's time to do the real conversion, and let's
				// use 16 bits...
				if (!convertOggToWavFile(
					waveObj->getOggFileName(),
					waveObj->getFileTemp(),
					16)) return false;
				DeleteFile(waveObj->getFileName());
				MoveFile(waveObj->getFileTemp(), waveObj->getFileName());
			}
		}
	}// for nodeIdx
*/
	return true;
}// executePostInstallTasks

