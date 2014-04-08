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

-- externally set variables
-- desiredFramesPerSecond, the same value as in the c++ program
-- highScoresList, a variable updated with updateHighScoresList, actually a simple table
-- programSoundEnabled, telling whether the sound is enabled or not by the user
-- programSoundActive, telling whether sound is available or not by the system

-- the list of frame collections...

animsFrameList={"frplJAvatar", "frenWhiteSheep", "frenBlackSheep", "frenHumanHead", "frenSheepCarrier", "frenHeadCarrier", "frblPlayerFlame", "frwgWhiteSheeps", "frwgBlackSheeps", "frwgHumanHeads", "frwgSheepsOut"}

globalEnv = _ENV

-- SOME USEFUL FUNCTIONS

-- getFrameIndexInList ********
-- given a frame name, and a frames list, returns the index of such frame in that list
-- (starting from 0) or null if none
function getFrameIndexInList(framename, frameList)
	for i=1, #frameList do
		if frameList[i].framename==framename then
			return i-1
		end
	end

	-- not found
	return NULL
end


-- getFrameIndex ***********
-- given a frame name, this function returns the index of such frame in its respective
-- frames list
function getFrameIndex (framename)
	local t
	local frameList

	for i=1, #animsFrameList do
		frameList=rawget(globalEnv, animsFrameList[i])
		t=getFrameIndexInList(framename, frameList)
		if type(t)~="nil" then
			return t
		end
	end

	-- not found
	logToFile("The follwoing framename wasn't found:", framename)
	return NULL
end


-- correctFrames ******
-- a function called by the system at start, for making the following change:
-- we look for frames telling a frame name at .aSVT, and change it for the respective frame 
-- number
function correctFrames()
	for i=1, #animsFrameList do
		local frameList

		frameList=rawget(globalEnv, animsFrameList[i])
		for j=1, #frameList do
			-- if we have a string, we change it for the index number it should be
			if type(frameList[j].aNF)=="string" then
				frameList[j].aNF=getFrameIndex(frameList[j].aNF)
			end
			-- if we have a null value, we change it for the data that means 'no next'
			if type(frameList[j].aNF)=="nil" then
				frameList[j].aNF=0
				frameList[j].aSVT=0
			end
		end
	end
end

-- errorManager ******
-- a function designed to take the error messages from failed function calls
function errorManager(message)
	logToFile("pcall error: ", message)
	return 0
end

-- startScript *******
-- a function called by the game at start, for initialization
function startScript()
	correctFrames()
	calculateVirtualWalls()
end

-- iif *************
-- a special if, the typical C++ (? : ) operator
function iif (expression, valueTrue, valueFalse)
	if expression then
		return valueTrue
	else
		return valueFalse
	end
end
