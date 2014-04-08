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


#ifndef FILE_WINCONSTANTS_INCLUDED
#define FILE_WINCONSTANTS_INCLUDED

#define FILE_WINCONSTANTS_VERSION "23-5-2002-H"// last modification: dd/mm/yy

#include <stdio.h>// defines NULL
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef void* LPVOID;

typedef struct { 
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;


#endif// FILE_WINCONSTANTS_INCLUDED
