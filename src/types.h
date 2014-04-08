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


//
// Common types and stuff.

#ifndef TYPES_FILE_INCLUDED
#define TYPES_FILE_INCLUDED
#define TYPES_FILE_VERSION "26-5-2001-H"// last modification
// compatible to versions from 24-5-2001-H

#include "win-constants.h"

// the following macros (than will never change) says whether
// a return error value means 'OK' or 'Error', that's very
// easy because 'false' for HHM means 'Error'.
// retValue is supposed to be a bool.
#define retIsError(retValue) (!(retValue))
#define retIsOk(retValue) (retValue)

// Variables and stuff for language switching
// The enumeration to be used for language switching
enum languageEnum {
	language_spanish,// hola mundo
	language_english// hello world
};


#endif //TYPES_FILE_INCLUDED
