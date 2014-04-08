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

-- BASIC MENU OBJECTS CREATION FUNCTIONS

-- createMenu ************
-- a function used for the creation of a menu object, telling the way such menu should
-- be drawn and used on the game
function createMenu(
	name,-- name of the menu
	backSurface,-- name of the back surface
	cursorSurface,-- name of the surface to be used for the cursor
	controlsList,-- list of the controls of this menu
	onInitMenu,-- code to be executed when entering to this menu
	onCloseMenu,-- code to be executed when leaving this menu
	onClickEvent,-- code to be executed when a click is done over the menu and nothing else
	onKeyClickEvent,-- code to be executed when the enter or equiv button is pressed
	onKeyPreviousEvent,-- code to be executed when the previous button is pressed
	onKeyNextEvent,-- code to be executed when the next button is pressed
	onOverEvent,-- code to be executed when having the mouse go over this button
	onOverOutEvent,-- code to execute when the mouse goes out the button after being over it
	onTickEvent-- code to be executed once per virtual frame
	)

	return {
	name=name,
	backSurface=backSurface,
	cursorSurface=cursorSurface,
	controlsList=controlsList,

	-- events
	onInitMenu_user=onInitMenu,-- yes, onInitMenu isn't the real one
	onCloseMenu=onCloseMenu,
	onClickEvent=onClickEvent,
	onKeyClickEvent=onKeyClickEvent,
	onKeyPreviousEvent=onKeyPreviousEvent,
	onKeyNextEvent=onKeyNextEvent,
	onOverEvent=onOverEvent,
	onOverOutEvent=onOverOutEvent,
	onTickEvent=onTickEvent,

	-- internal data and methods
	onInitMenu=function (ctrl) ctrl.wasMouseOver=false; ctrl:onInitMenu_user() end,
	wasMouseOver=false -- was the mouse over this control
	}

	end


-- createMenuControl ************
-- a function used for the creation of a menu control object, telling the way such control
-- should be drawn and used on the game
function createMenuControl(
	name,-- name of this control
	surface,-- surface to be used when drawing this control
	xSrc, ySrc,-- source coordinates of the image
	x, y, w, h,-- coordinates of the control on the screen
	text,-- text to be drawn over the control
	fontName,-- font to be used for the control text
	xF, yF, wF, hF, h_align, v_align,-- coordinates of this control's text
	onInitMenu,-- code to be executed when entering to the menu this button is on
	onCloseMenu,-- code to be executed when leaving the menu this button is on
	onClickEvent,-- code to be executed when clicking on this button
	onOverEvent,-- code to be executed when having the mouse go over this button
	onOverOutEvent,-- code to execute when the mouse goes out the button after being over it
	onTickEvent-- code to be executed once per virtual frame
	)

	return {
	name=name,
	surface=surface,
	xSrc=xSrc, ySrc=ySrc,
	x=x, y=y, w=w, h=h,
	text=text, fontName=fontName,
	xF=xF, yF=yF, wF=wF, hF=hF,
	h_align=h_align, v_align=v_align,

	-- events
	onInitMenu_user=onInitMenu,-- yes, onInitMenu isn't the real one
	onCloseMenu=onCloseMenu,
	onClickEvent=onClickEvent, onOverEvent=onOverEvent, onOverOutEvent=onOverOutEvent,
	onTickEvent=onTickEvent,

	-- internal data and methods
	onInitMenu=function (ctrl) ctrl.wasMouseOver=false; ctrl:onInitMenu_user() end,
	wasMouseOver=false, -- was the mouse over this control
	drawFunction=nil -- the draw function, usually 'function (v) end', nil for no one
	}

	end

