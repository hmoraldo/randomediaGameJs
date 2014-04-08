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
RESINFOMAN.*:


These files contain the functions used for the management of the
information that tells the program the way the program resources
should be used (for example, scripts explaining the sequence of
frames needed for an animation, or even scripts telling the way
an enemy should move).

*/

#ifndef FILE_RESINFOMAN_INCLUDED
#define FILE_RESINFOMAN_INCLUDED

#define FILE_RESINFOMAN_VERSION "30-7-2003-H"// last modification: dd/mm/yy

// Functions of resource info management
bool initializeResInfo();
bool closeResourcesInfo();

#endif// FILE_RESINFOMAN_INCLUDED
