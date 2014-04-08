-- Copyright (c) 2003 - 2014 Horacio Hernan Moraldo
--
-- This software is provided 'as-is', without any express or
-- implied warranty. In no event will the authors be held liable
-- for any damages arising from the use of this software.
--
-- Permission is granted to anyone to use this software for any
-- purpose, including commercial applications, and to alter it and
-- redistribute it freely, subject to the following restrictions:
--
-- 1. The origin of this software must not be misrepresented; you
-- must not claim that you wrote the original software. If you use
-- this software in a product, an acknowledgment in the product
-- documentation would be appreciated but is not required.
--
-- 2. Altered source versions must be plainly marked as such, and
-- must not be misrepresented as being the original software.
--
-- 3. This notice may not be removed or altered from any source
-- distribution.
--

-- LANGUAGE STUFF
-- general variables
--currentLanguage="english"

-- multilanguage text objects and methods

-- langText ******
-- A constructor for multilanguage text objects
function langText(spanish, english)
	return {spanish=spanish, english=english}
end;

-- getText *****
-- A function designed to get the text of the current language, from
-- a given text object
function getText(textObj)
	if type(textObj)=="string" then
		return textObj;
	end;

	if currentLanguage=="spanish" then
		-- show it in spanish if possible
		if textObj.spanish~=nil then
			return textObj.spanish;
		else
			return textObj.english;
		end;
	else
		-- show it in english if possible
		if textObj.english~=nil then
			return textObj.english;
		else
			return textObj.spanish;
		end;
	end;
end;

-- selectText *****
-- A function designed to select between texts for every language available, the one that
-- goes for the current language
function selectText(spanish, english)
	return getText(langText(spanish, english));
end;
