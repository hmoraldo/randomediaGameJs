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
VIRTUALWALLS.*:

These files contain all the functions related to the management of
the virtual walls, the objects that limit the movement of the units.

*/

/*

Virtual walls:
Virtual walls are objects that define the limits of the normal
movement for units. There is a left, and a right wall. These are curves
that have defined, for every y coordinate, an x value meaning the
horizontal limit for that line. Virtual walls are supposed to be used
for defining the points where the drawings in the background show the
beginning of an obstacle that should continue even outside the screen.

*/

#ifndef FILE_VIRTUALWALLS_INCLUDED
#define FILE_VIRTUALWALLS_INCLUDED

#define FILE_VIRTUALWALLS_VERSION "11-12-2002-H"// last modification: dd/mm/yy

// Some useful functions
bool initializeVirtualWalls();
bool deinitializeVirtualWalls();
int getLeftWall(int y);
int getRightWall(int y);
int getGroundWall(int x);


#endif// FILE_VIRTUALWALLS_INCLUDED
