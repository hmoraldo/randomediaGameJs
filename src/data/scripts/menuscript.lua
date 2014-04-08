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

-- MENU RELATED FUNCTIONS

globalEnv = _ENV

-- some data for management
frameNumberFromZero=nil
lastMouseX=nil; lastMouseY=nil; lastMouseClick=nil;
lastKeyPrevious=nil; lastKeyNext=nil; lastKeyClick=nil
switchingToMenu=nil

-- global vars...
-- mouse pointer coordinates
menuMouseX=nil
menuMouseY=nil
currentMenu=nil

-- current music name (for knowing whether to change or not)
currentMusicName=nil


-- beforeLoop **********
-- this is the function that is called by the game before initiating the menus manager, for
-- example, after finishing a game session or just after the intro
function beforeLoop ()
	-- initialize the mouse coordinates
	menuMouseX=0
	menuMouseY=0

	-- do some other useful stuff
	-- (we want the first frame to be named 0)
	frameNumberFromZero=-1

	-- we are not using any music yet
	currentMusicName=nil

	-- we set the current menu to nil
	currentMenu=nil

	-- we are not switching anywhere yet
	switchingToMenu=nil
end -- beforeLoop

-- afterLoop **********
-- this is the function that is called by the game before closing the menus manager,
-- for example, just before starting a game session, at the moment the player is going to
-- go out of the menues
function afterLoop ()
	currentMenu=nil

	logToFile("After loop executed!")
end -- afterLoop

-- executeFrame **********
-- this is the function that is called by the game for managing
-- the execution of the menu screen
function executeFrame (frameNumber, mouseX, mouseY, mouseClick, keyPrevious, keyNext, keyClick)
	local ctrl
	local pointedObject

	-- FIRST THE ROUTINE THINGS...
	-- the following variables tell whether an event is happening or not
	local mouseMoved=false
	local mouseClickDown=false; local mouseClickUp=false
	local keyPreviousDown=false; local keyPreviousUp=false
	local keyNextDown=false; local keyNextUp=false
	local keyClickDown=false; local keyClickUp=false

	-- we increment our internal frame counter
	frameNumberFromZero=frameNumberFromZero+1

	-- set the mouse coordinates on the variables outside of this function
	menuMouseX=mouseX
	menuMouseY=mouseY

	-- if this is our first frame, set the last* values to the current ones
	if frameNumberFromZero==0 then
		lastMouseX=mouseX-2; lastMouseY=mouseY-2; -- we want mouse to have moved only
		lastMouseClick=mouseClick
		lastKeyPrevious=keyPrevious; lastKeyNext=keyNext
		lastKeyClick=keyClick
	end

	-- now check for changes in the different io devices
	if mouseX~=lastMouseX or mouseY~=lastMouseY then mouseMoved=true end
	if (not lastMouseClick) and mouseClick then mouseClickDown=true end
	if lastMouseClick and (not mouseClick) then mouseClickUp=true end
	if (not lastKeyClick) and keyClick then keyClickDown=true end
	if lastKeyClick and (not keyClick) then keyClickUp=true end
	if (not lastKeyPrevious) and lastKeyPrevious~=keyPrevious then keyPreviousDown=true end
	if lastKeyPrevious and lastKeyPrevious~=keyPrevious then keyPreviousUp=true end
	if (not lastKeyNext) and lastKeyNext~=keyNext then keyNextDown=true end
	if lastKeyNext and lastKeyNext~=keyNext then keyNextUp=true end

	-- EXECUTE THE EVENTS THAT ARE NEEDED
	-- go through all the controls, doing what's needed in every case
	pointedObject=nil

	for i=1, #currentMenu.controlsList do
		ctrl=currentMenu.controlsList[i]

		-- we don't let two pointed objects to exist
		if mouseX>=ctrl.x and mouseX<=ctrl.x+ctrl.w
			and mouseY>=ctrl.y and mouseY<=ctrl.y+ctrl.h
			and pointedObject==nil then

			-- this is the button we are over of
			pointedObject=ctrl
		else
			-- the mouse is not over this object...
			if ctrl.wasMouseOver then
				ctrl:onOverOutEvent()
			end--if
			ctrl.wasMouseOver=false
		end-- if
	end-- for

	-- check whether the pointed object is the menu itself or not
	if pointedObject==nil then
		-- the mouse pointer is over the menu itself
		pointedObject=currentMenu
	else
		-- the mouse is not over the menu itself
		if currentMenu.wasMouseOver then
			currentMenu:onOverOutEvent()
		end--if
		currentMenu.wasMouseOver=false
	end-- if

	-- we have already executed the overOut events on all the objects... now execute
	-- the mouse over and the mouse click events on the pointed object, in that exact
	-- order
	if pointedObject~=nil then
		-- the mouse is over this object, we execute the mouse over event on it
		if not pointedObject.wasMouseOver then
			pointedObject:onOverEvent()
		end--if
		pointedObject.wasMouseOver=true

		-- (only one object can receive the clicks events for a given time)
		if mouseClickDown then pointedObject:onClickEvent() end
	end-- if

	-- also, the key events are only received by the menu itself
	if currentMenu~=nil then
		if keyClickDown then currentMenu:onKeyClickEvent() end
		if keyPreviousDown then currentMenu:onKeyPreviousEvent() end
		if keyNextDown then currentMenu:onKeyNextEvent() end
	end-- if

	-- finally, after all that, execute the tick event on all the controls
	for i=1, #currentMenu.controlsList do
		ctrl=currentMenu.controlsList[i]

		-- execute the tick event
		ctrl:onTickEvent()
	end-- for

	-- SET UP FOR THE NEXT FRAME EXECUTION
	-- finally, set the last* values for the next execution of this function
	lastMouseX=mouseX; lastMouseY=mouseY
	lastMouseClick=mouseClick
	lastKeyPrevious=keyPrevious; lastKeyNext=keyNext
	lastKeyClick=keyClick

	-- and switch to another menu if that's what the user is asking for
	if switchingToMenu~=nil then
		switchToMenu(switchingToMenu)
		switchingToMenu=nil
	end
end -- executeFrame 

-- drawFrame **********
-- this is the function that is called by the game for managing
-- the drawing of the menu screen
function drawFrame ()
	local ctrl

	-- draw the menu background...
	if currentMenu.backSurface~=nil then
		menuBltImage(currentMenu.backSurface, 0, 0, false);
	end

	-- draw all the buttons that are visible
	for i=1, #currentMenu.controlsList do
		ctrl=currentMenu.controlsList[i]

		-- draw this control
		if ctrl.drawFunction~=nil then
			ctrl:drawFunction();
		end
		if ctrl.surface~=nil then
			menuBltImage(
				ctrl.surface,
				ctrl.x, ctrl.y,
				ctrl.xSrc, ctrl.ySrc,
				ctrl.w, ctrl.h,
				true);
		end
		if ctrl.fontName~=nil and ctrl.text~=nil then
			menuDrawText(
				ctrl.fontName, getText(ctrl.text),
				ctrl.xF, ctrl.yF, ctrl.wF, ctrl.hF,
				ctrl.h_align, ctrl.v_align);
		end
	end

	-- show the cursor
	if currentMenu.cursorSurface~=nil then
		menuBltImage(currentMenu.cursorSurface, menuMouseX, menuMouseY, true);
	end
end -- drawFrame



-- EXTERNAL C - FUNCTIONS
-- logToFile(...)
-- menuBltImage(surfaceName, x, y, transparent)
-- menuBltImage(surfaceName, xD, yD, xS, yS, wS, hS, transparent)
-- menuDrawText(fontName, text, x, y, w, h, horiz align, vertical align)
-- switchToMenu(menuName)
-- menuSetMouseXY(x, y)
-- setCurrentLanguage(lang)
-- updateHighScoresList()
-- playSoundResource(resName, infinite)
-- stopAndPlaySoundResource(resName)
-- stopSoundResource(resName)
-- setSoundEnabled(bEnabled)
-- 



-- EXPORTED FUNCTIONS, TO BE USED FROM C ONLY

-- this is the function that's called every time the C function switchToMenu is called
function cpp_switchToMenu(menuName)
	local ctrl

	logToFile("Switching to '" .. tostring(menuName) .. "'")

	-- execute the close event on all the objects of the old menu, and on the menu itself
	if currentMenu~=nil then
		currentMenu:onCloseMenu()
		for i=1, #currentMenu.controlsList do
			ctrl=currentMenu.controlsList[i]
			ctrl:onCloseMenu()
		end
	end

	-- select the named menu
	currentMenu=rawget(globalEnv, menuName)

	-- execute the init event on all the objects of this menu, and on the menu itself
	if currentMenu~=nil then
		currentMenu.wasMouseOver=false;
		currentMenu:onInitMenu()
		for i=1, #currentMenu.controlsList do
			ctrl=currentMenu.controlsList[i]
			ctrl.wasMouseOver=false;
			ctrl:onInitMenu()
		end
	end
end
