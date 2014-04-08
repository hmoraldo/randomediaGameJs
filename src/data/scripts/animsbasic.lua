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

-- FUNCTIONS USED FOR FRAMING

-- createImage ************
-- a function used for the creation of an image object, telling the way such image should
-- be drawn and used on the game
function createImage(
	resname, -- name of the resource to use 
	x, y, w, h, -- x, y, w, h, as usual 
	cx, cy, cw, ch, -- cX, cY, cW, cH, as usual, but in absolute coordinates! 
	hx, hy, -- handlerX, handlerY, as usual, but in absolute coordinates 
	sx, sy, -- spawnBulletsX, spawnBulletsY, as usual, but in absolute coordinates 
	ax, ay -- adherenceX, adherenceY, as usual, but in absolute coordinates 
	)

	return {
	resname=resname,
	x=x, y=y, w=w, h=h,
	cx=cx, cy=cy, cw=cw, ch=ch,
	hx=hx, hy=hy,
	sx=sx, sy=sy,
	ax=ax, ay=ay,
	}

	end

-- createFrame **************
-- a function used for the creation of an animation frame, complete but using the data
-- of a previously defined image
function createFrame(
	framename, -- name of the frame itself 
	image,-- an image for the frame
	aNF, -- autoNextFrameName, the name of the next frame to show after this one
	aSVT, -- autoShowVirtualTime, the time in seconds to show this frame
	eventName,-- name of the event to execute before going to the next frame, null for none 
	groupName-- name of the group of frames this frame belongs to
	)

	return {
	framename=framename,
	resname=image.resname,
	x=image.x, y=image.y, w=image.w, h=image.h,
	cx=image.cx, cy=image.cy, cw=image.cw, ch=image.ch,
	hx=image.hx, hy=image.hy,
	sx=image.sx, sy=image.sy,
	ax=image.ax, ay=image.ay,
	aNF=aNF,
	aSVT=aSVT,
	eventName=eventName,
	groupName=groupName
	}

	end


-- createFullFrame **********
-- a function used for the creation of a complete frame of animation
-- (shouldn't be used much)
function createFullFrame(
	framename, -- name of the frame itself 
	resname, -- name of the resource to use 
	x, y, w, h, -- x, y, w, h, as usual 
	cx, cy, cw, ch, -- cX, cY, cW, cH, as usual, but in absolute coordinates! 
	hx, hy, -- handlerX, handlerY, as usual, but in absolute coordinates 
	sx, sy, -- spawnBulletsX, spawnBulletsY, as usual, but in absolute coordinates 
	ax, ay, -- adherenceX, adherenceY, as usual, but in absolute coordinates 
	aNF, -- autoNextFrame, just as usual 
	aSVT, -- autoShowVirtualTime, just as autoShowVirtualFrames but with time measured in seconds 
	eventName,-- name of the event to execute before going to the next frame, null for none 
	groupName-- name of the group of frames this frame belongs to
	)

	return {
	framename=framename,
	resname=resname,
	x=x, y=y, w=w, h=h,
	cx=cx, cy=cy, cw=cw, ch=ch,
	hx=hx, hy=hy,
	sx=sx, sy=sy,
	ax=ax, ay=ay,
	aNF=aNF,
	aSVT=aSVT,
	eventName=eventName,
	groupName=groupName
	}

	end

