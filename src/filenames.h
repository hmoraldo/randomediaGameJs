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
FILENAMES.*:


These files defines the files to be used by the program, by binding
a #define to each one of those.

*/

#ifndef FILE_FILENAMES_INCLUDED
#define FILE_FILENAMES_INCLUDED

#define FILE_FILENAMES_VERSION "24-5-2002-H"// last modification: dd/mm/yy

// Define the names for the files containing wave sound data (_WAV_).
// ...
// And for test purposes (_WAVTST_).
#define FILE_FILENAMES_WAVTST_ESCAPE_WAV "data/sound/intro/intro.wav"// "escape del imperio andre" voice

// Define the names for the files containing wave music data (_WMS_).
// ...
// And for test purposes (_WMSTST_).
#ifdef PROGMODE_SCREENSHOTS_DEMO_YES// is this a screenshots demo or not?
	#define FILE_FILENAMES_WMSTST_BACKGROUND_WAV "data/demosnd/menufx1.wav"// our music for testing
#else
	#define FILE_FILENAMES_WMSTST_BACKGROUND_WAV "data/sound/intro/intro.wav"// our music for testing
#endif

// Define what's the wave file where to load the sound system params.
#define FILE_FILENAMES_WAV_INITWAV FILE_FILENAMES_WMSTST_BACKGROUND_WAV


#endif// FILE_FILENAMES_INCLUDED
