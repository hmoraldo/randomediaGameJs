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

#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "resfonts.h"// for accessing to the font resources


// setUpProgramFonts *************
// This function sets all the fonts up, either initializing or
// deinitializing them.
// It does only return false if the load of data fails, it doesn't do
// so if it appears to be an unnecessary action.
bool setUpProgramFonts(
	bool shouldDeinitialize)// boolean telling whether this function should deinitialize those who need being deinitialized, or initialize those who need being initialized
{

	if (!shouldDeinitialize) {
		// Initialize all the resources necessary for using fonts
		if (!fntDahFont.isReady()) {
			// we assume no one of the resources used for fonts
			// is ready yet... so we initialize them all together
			if (
				!fntDahFont.init(getSurfaceByResourceName(
					"srfDahFont"), 8, 15) ||
				!fntEdiaFont.init(getSurfaceByResourceName(
					"srfEdiaFont"), 6, 6) ||
				!fntEdiaBigFont.init(getSurfaceByResourceName(
					"srfEdiaFont"), 13, 11) ||
				!fntDahFont.setUpCharactersInMatrix(
					" AaBbCcDdEeFfGgHhIiJjKkLlMm Nn—ÒOoPpQqRrSsTtUuVvWwXxYy Zz0123456789°!ø?=|@#\"$%&\\/()+*-_;:.,<>^[]{}¡·…ÈÕÌ”Û⁄˙ƒ‰ÀÎœÔ÷ˆ‹¸'",
					1, 1,// x, y for the matrix start
					27,// width of the matrix in characters
					1, 1) ||// horizontal and vertical space between characters
				!fntEdiaFont.setUpCharactersInMatrix(
					" ABCDEFGHIJKLMN—OPQRSTUVWXYZ,.:?!-0123456789",
					1, 1,// x, y for the matrix start
					28,// width of the matrix in characters
					1, 1) ||// horizontal and vertical space between characters
				!fntEdiaBigFont.setUpCharactersInMatrix(
					" ABCDEFGHIJKLMN—OPQRSTUVWXYZ,.:?!-0123456789wb",
					1, 31,// x, y for the matrix start
					28,// width of the matrix in characters
					1, 1))// horizontal and vertical space between characters
			{
				fntDahFont.end();
				fntEdiaFont.end();
				fntEdiaBigFont.end();
				return false;
			}
		}
	}else{
		// Deinitialize all our font resources
		fntDahFont.end();
		fntEdiaFont.end();
		fntEdiaBigFont.end();
	}


	return true;// all ok...
}// setUpProgramFonts
