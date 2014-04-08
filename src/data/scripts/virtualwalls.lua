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
-- VIRTUAL WALLS

leftVirtualWall={}
rightVirtualWall={}
groundVirtualWall={}

-- fillVectorWithRectLine *********
-- a function which fills the values of a vector as if they were the coordinates of the
-- points in a rect
function fillVectorWithRectLine (vector, x1, y1, x2, y2)
	local step, y

	if (x2-x1)==0 then
		return
	end

	step=(y2-y1)/(x2-x1)
	y=y1

	for x=x1, x2 do
		vector[x]=y
		y=y+step
	end
end


-- calculateVirtualWalls **********
-- a function designed to fill the wall vectors with the appropriate values
function calculateVirtualWalls()
	-- calculate the left virtual wall
	fillVectorWithRectLine (leftVirtualWall, 0, 0, 257, 0)
	fillVectorWithRectLine (leftVirtualWall, 257, 0, 371, 162)
	fillVectorWithRectLine (leftVirtualWall, 371, 162, 468, 201)
	fillVectorWithRectLine (leftVirtualWall, 468, 201, 527, 215)
	fillVectorWithRectLine (leftVirtualWall, 527, 215, leftWallSize, 215)

	-- calculate the right virtual wall
	fillVectorWithRectLine (rightVirtualWall, 0, 664, 62, 646)
	fillVectorWithRectLine (rightVirtualWall, 62, 646, 159, 655)
	fillVectorWithRectLine (rightVirtualWall, 159, 655, 220, 634)
	fillVectorWithRectLine (rightVirtualWall, 220, 634, 280, 622)
	fillVectorWithRectLine (rightVirtualWall, 280, 622, 307, 658)
	fillVectorWithRectLine (rightVirtualWall, 307, 658, 353, 665)
	fillVectorWithRectLine (rightVirtualWall, 353, 665, 378, 688)
	fillVectorWithRectLine (rightVirtualWall, 378, 688, 440, 667)
	fillVectorWithRectLine (rightVirtualWall, 440, 667, 473, 629)
	fillVectorWithRectLine (rightVirtualWall, 473, 629, 539, 620)
	fillVectorWithRectLine (rightVirtualWall, 539, 620, rightWallSize, 620)

	-- calculate the ground virtual wall
	fillVectorWithRectLine (groundVirtualWall, 0, 506, 80, 505)
	fillVectorWithRectLine (groundVirtualWall, 80, 505, 145, 526)
	fillVectorWithRectLine (groundVirtualWall, 145, 526, 174, 542)
	fillVectorWithRectLine (groundVirtualWall, 174, 542, 311, 559)
	fillVectorWithRectLine (groundVirtualWall, 311, 559, 518, 536)
	fillVectorWithRectLine (groundVirtualWall, 518, 536, 635, 546)
	fillVectorWithRectLine (groundVirtualWall, 635, 546, groundWallSize, 546)
end
