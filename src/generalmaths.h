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
GENERALMATHS.*:


These files set a few general mathematic functions and stuff. Nothing
very complex.

Use the following documented line:
#include "generalmaths.h"// for mathematical functions
for including.
*/

#ifndef FILE_GENERALMATHS_INCLUDED
#define FILE_GENERALMATHS_INCLUDED

#define FILE_GENERALMATHS_VERSION "13-4-2003-H"// last modification: dd/mm/yy


// MIN: smallest between two numbers
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif// MIN

// MAX: biggest between two numbers
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif// MAX

// SQUARE: square of a number
#ifndef SQUARE
#define SQUARE(x) ((x)*(x))
#endif// SQUARE

// ABS: absolute value
#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif// ABS

// SGN: sign, 0 for 0, 1 for positive numbers, and -1 for negative numbers
// It might seem strange, but it actually does that :)
#ifndef SGN
#define SGN(x) ((x)==0?0:((x)>0?1:-1))
#endif// SGN

// SGNPOS: sign, 1 for positive numbers and 0, and -1 for negative numbers
// That is, SGN with 1 by default (in case x is 0)
#ifndef SGNPOS
#define SGNPOS(x) ((x)>=0?1:-1)
#endif// SGNPOS

// SGNNEG: sign, 1 for positive numbers, and -1 for negative numbers and 0
// That is, SGN with -1 by default (in case x is 0)
#ifndef SGNNEG
#define SGNNEG(x) ((x)>0?1:-1)
#endif// SGNNEG

// POINTINRECT: tells if a given point (x,y) is inside the rectangle
// with corners (x1,y1) and (x2,y2).
#ifndef POINTINRECT
#define POINTINRECT(x, y, x1, y1, x2, y2) \
	(((x)>=(x1)) && ((x)<=(x2)) && ((y)>=(y1)) && ((y)<=(y2))) 
#endif// POINTINRECT

// VLINEANDHLINE: tells if a vertical line which goes from
// (ax,ay1) to (ax,ay2) collides with an horizontal line which goes
// from (bx1, by) to (bx2, by).
#ifndef VLINEANDHLINE
#define VLINEANDHLINE(ax, ay1, ay2, bx1, bx2, by) \
	(((ax)>=(bx1)) && ((ax)<=(bx2)) && ((by)>=(ay1)) && ((by)<=(ay2))) 
#endif// VLINEANDHLINE

// INTERSECTINGRECTANGLES: tells if two rectangles, with corners
// (pX, pY), (pX2, pY2) and (eX, eY), (eX2, eY2), intersect theirselves.
#ifndef INTERSECTINGRECTANGLES
#define INTERSECTINGRECTANGLES(pX, pY, pX2, pY2, eX, eY, eX2, eY2) \
	( (pX2)>=(eX) && (eX2)>=(pX) && (pY2)>=(eY) && (eY2)>=(pY))
#endif// INTERSECTINGRECTANGLES

// INTERSECTING_NOTNULL_RECTANGLES: tells if two rectangles, with corners
// (pX, pY), (pX2, pY2) and (eX, eY), (eX2, eY2), intersect theirselves.
// It's the same than INTERSECTINGRECTANGLES, but this one considers
// that null rectangles (those with width and height both equal to 0)
// can't intersect any other rectangles at all.
#ifndef INTERSECTING_NOTNULL_RECTANGLES
#define INTERSECTING_NOTNULL_RECTANGLES(pX, pY, pX2, pY2, eX, eY, eX2, eY2) \
	((pX2)-(pX)+1!=0 && (pY2)-(pY)+1!=0 && \
	(eX2)-(eX)+1!=0 && (eY2)-(eY)+1!=0 && \
	INTERSECTINGRECTANGLES(pX, pY, pX2, pY2, eX, eY, eX2, eY2))
#endif// INTERSECTING_NOTNULL_RECTANGLES

// RECTINRECTANGLE: tells if one rectangle, with corners
// (pX, pY), (pX2, pY2), is inside another one, with corners
// (eX, eY), (eX2, eY2).
#ifndef RECTINRECTANGLE
#define RECTINRECTANGLE(pX, pY, pX2, pY2, eX, eY, eX2, eY2) \
	( (pX)>=(eX) && (pX2)<=(eX2) && (pY)>=(eY) && (pY2)<=(eY2))
#endif// RECTINRECTANGLE

#endif// FILE_GENERALMATHS_INCLUDED
