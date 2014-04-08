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

-- MENU OBJECTS CREATION FUNCTIONS IN THE LIGHTABLE STYLE

-- a global variable:
-- selected control (from menu of style lightable)
selectedControlLgt=nil

globalEnv = _ENV

-- createLightableMenu ************
-- a function used for the creation of a menu object, telling the way such menu should
-- be drawn and used on the game
function createLightableMenu(
	name,-- name of the menu
	backSurface,-- surface for the background image
	cursorSurface,-- surface to be used with the cursor
	soundMusic,-- name of the sound to be played as music with this menu
	toolTipText,-- default text for this menu in the tooltip
	toolTipVarName,-- name of the variable that contains the tool tip text for this menu
	onInitMenu,-- code to be executed at the time the user enters to this menu
	onCloseMenu,-- code to be executed at the time the user leaves this menu
	onClickEvent,-- code to be executed every time a click to the menu is detected
	onTickEvent,-- code to be executed once per frame
	controlsList-- the list of controls to be used in this menu
	)

	local m;

	m=createMenu(
		name, -- name
		backSurface, -- backSurface
		cursorSurface, -- cursorSurface
		controlsList, -- controlsList
		function (v) end, -- onInitMenu
		function (v) end, -- onCloseMenu
		function (v) end, -- onClickEvent
		function (v) end, -- onKeyClickEvent
		function (v) end, -- onKeyPreviousEvent
		function (v) end, -- onKeyNextEvent
		function (v) end, -- onOverEvent
		function (v) end, -- onOverOutEvent
		function (v) end -- onTickEvent
		);

	-- load some other data
	m.objectType="menu";
	m.soundMusic=soundMusic;
	m.toolTipText=toolTipText;
	m.toolTipVarName=toolTipVarName;
	m.user_onInitMenu=onInitMenu;
	m.user_onCloseMenu=onCloseMenu;
	m.user_onClickEvent=onClickEvent;
	m.user_onTickEvent=onTickEvent;

	-- load new methods for events...
	m.onCloseMenu=function (v)
		rawset(globalEnv, v.toolTipVarName, nil);
		v:user_onCloseMenu();
	end;
	m.onClickEvent=function (v)
		v:user_onClickEvent();
	end;
	m.onTickEvent=function (v)
		v:user_onTickEvent();
	end;
	m.onKeyClickEvent=function (v)
		if selectedControlLgt~=nil then
			selectedControlLgt:onClickEvent();
		end;
	end;
	m.onKeyPreviousEvent=function (v)
		if selectedControlLgt~=nil then
			v:switchToButton(selectedControlLgt.previousButtonLL);
		end;
	end;
	m.onKeyNextEvent=function (v)
		if selectedControlLgt~=nil then
			v:switchToButton(selectedControlLgt.nextButtonLL);
		end;
	end;

	-- some extra useful methods
	m.switchToButton=function (v, button)
		menuSetMouseXY(button.x+(button.w/2), button.y+(button.h/2));
	end

	-- load a huge init method
	m.onInitMenu=function (v)
		local oneBefore=nil;
		local size;
		local ctrl;

		selectedControlLgt=nil;
		rawset(globalEnv, v.toolTipVarName, v.toolTipText);

		-- make the linked list between the buttons
		-- (using m.firstButton, b.previousButton and b.nextButton)
		if #v.controlsList>0 then
			size=#v.controlsList;

			-- do the link to the first node
			v.firstButtonLL=nil;

			-- look for the latest button object on the screen
			for i=1, size do
				ctrl=v.controlsList[i];
				if ctrl.objectType=="button" then
					oneBefore=ctrl;
					if v.firstButtonLL==nil then v.firstButtonLL=oneBefore; end
				end;
			end;-- for

			if oneBefore~=nil then
				-- write the 'previousButtonLL' link
				for i=1, size do
					ctrl=v.controlsList[i];
					if ctrl.objectType=="button" then
						ctrl.previousButtonLL=oneBefore;
						oneBefore=ctrl;
					end;
				end;-- for

				-- write the 'nextButton' link
				for i=1, size do
					ctrl=v.controlsList[i];
					if ctrl.objectType=="button" then
						ctrl.previousButtonLL.nextButtonLL=ctrl;
					end;
				end;-- for

				-- select the first button, if there is any
				v:switchToButton(v.firstButtonLL);
			end;-- if
		end;-- if

		-- execute the new music if needed
		if currentMusicName~=v.soundMusic then
			stopSoundResource(currentMusicName);
			currentMusicName=v.soundMusic;
			if v.soundMusic~=nil then playSoundResource(v.soundMusic, true); end;
		end;

		v:user_onInitMenu();
	end;-- onInitMenu

	return m;
end;




-- createLightableButton ************
-- a function used for the creation of a lightable button, telling the way such control
-- should be drawn and used on the game
function createLightableButton(
	name,-- name of the button
	x, y,-- coordinates of the button on the screen
	surfaceNormal,-- name of the surface to use in the normal state
	xS, yS, wS, hS,-- source coordinates for the source surface
	textDesel,-- text to use when the button is not selected
	fontNameDesel,-- font to use for the text when not selected
	xFDesel, yFDesel, wFDesel, hFDesel, h_alignDesel, v_alignDesel,-- more font data not sel
	xSel, ySel,-- coordinates of the button on the screen when selected
	surfaceSelected,-- name of the surface to use in the selected state
	xSSel, ySSel, wSSel, hSSel,-- source coordinates for the source surface, when selected
	textSel,-- text to use when the button is selected
	fontNameSel,-- font to use for the text when selected
	xFSel, yFSel, wFSel, hFSel, h_alignSel, v_alignSel,-- other font data when selected
	soundSelection,-- sound to be executed when the button gets selected
	toolTipText,-- text to put in the tooltip when selected
	toolTipVarName,-- name of the variable that contains the tool tip text for this button
	onInitMenu,-- code to be executed at the time the user enters to this menu
	onCloseMenu,-- code to be executed at the time the user leaves this menu
	onSelEvent,-- code to be executed when the button gets selected
	onDeselEvent,-- code to be executed when the button gets deselected
	onClickEvent,-- code to be executed when a click on the button is received
	onTickEvent-- code to be executed once per frame
	)

	local b;

	b=createMenuControl(
		name, -- name
		surfaceNormal, -- surface
		xS, yS, -- xSrc, ySrc,
		x, y, wS, hS, -- x, y, w, h
		textDesel, -- text
		fontNameDesel, -- fontName
		xFDesel, yFDesel, wFDesel, hFDesel, -- xF, yF, wF, hF
		h_alignDesel, v_alignDesel, -- h_align, v_align,
		function (v) end, -- onInitMenu
		function (v) end, -- onCloseMenu
		function (v) end, -- onClickEvent
		function (v) end, -- onOverEvent
		function (v) end, -- onOverOutEvent
		function (v) end -- onTickEvent
		);

	-- load some other data
	b.objectType="button";
	b.xDesel=x; b.yDesel=y; b.surfaceNormal=surfaceNormal;
	b.xSDesel=xS; b.ySDesel=yS; b.wSDesel=wS; b.hSDesel=hS;
	b.textDesel=textDesel; b.fontNameDesel=fontNameDesel;
	if xFDesel==nil then
		b.xFDesel=x; b.yFDesel=y; b.wFDesel=wS; b.hFDesel=hS;
	else
		b.xFDesel=xFDesel; b.yFDesel=yFDesel; b.wFDesel=wFDesel; b.hFDesel=hFDesel;
	end
	b.h_alignDesel=h_alignDesel; b.v_alignDesel=v_alignDesel;
	if xSel==nil then
		b.xSel=b.x; b.ySel=b.y;
	else
		b.xSel=xSel; b.ySel=ySel;
	end
	b.surfaceSelected=surfaceSelected;
	if xSSel==nil then
		b.xSSel=xS; b.ySSel=yS; b.wSSel=wSSel; b.hSSel=hS;
	else
		b.xSSel=xSSel; b.ySSel=ySSel; b.wSSel=wSSel; b.hSSel=hSSel;
	end
	if textSel==nil then
		b.textSel=b.textDesel;
	else
		b.textSel=textSel;
	end;
	b.fontNameSel=fontNameSel;
	if xFSel==nil then
		b.xFSel=b.xFDesel; b.yFSel=b.yFDesel; b.wFSel=b.wFDesel; b.hFSel=b.hFDesel;
	else
		b.xFSel=xFSel; b.yFSel=yFSel; b.wFSel=wFSel; b.hFSel=hFSel;
	end
	if h_alignSel==nil then
		b.h_alignSel=h_alignDesel; b.v_alignSel=v_alignDesel;
	else
		b.h_alignSel=h_alignSel; b.v_alignSel=v_alignSel;
	end
	b.soundSelection=soundSelection;
	b.toolTipText=toolTipText;
	b.toolTipVarName=toolTipVarName;
	b.user_onInitMenu=onInitMenu;
	b.user_onCloseMenu=onCloseMenu;
	b.user_onSelEvent=onSelEvent;
	b.user_onDeselEvent=onDeselEvent;
	b.user_onClickEvent=onClickEvent;
	b.user_onTickEvent=onTickEvent;

	-- load new general methods
	function b.selectMe(v)
		v.surface=v.surfaceSelected;
		v.x=v.xSel; v.y=v.ySel; v.w=v.wSSel; v.h=v.hSSel;
		v.xSrc=v.xSSel; v.ySrc=v.ySSel;
		v.text=v.textSel; v.fontName=v.fontNameSel;
		v.xF=v.xFSel; v.yF=v.yFSel; v.wF=v.wFSel; v.hF=v.hFSel;
		v.h_align=v.h_alignSel; v.v_align=v.v_alignSel;

		if selectedControlLgt~=v then
			rawset(globalEnv, v.toolTipVarName, v.toolTipText);
			selectedControlLgt=v;
			v:user_onSelEvent();
		end
	end
	function b.deselectMe(v)
		v.surface=v.surfaceNormal;
		v.x=v.xDesel; v.y=v.yDesel; v.w=v.wSDesel; v.h=v.hSDesel;
		v.xSrc=v.xSDesel; v.ySrc=v.ySDesel;
		v.text=v.textDesel; v.fontName=v.fontNameDesel;
		v.xF=v.xFDesel; v.yF=v.yFDesel; v.wF=v.wFDesel; v.hF=v.hFDesel;
		v.h_align=v.h_alignDesel; v.v_align=v.v_alignDesel;

		if selectedControlLgt==v then
			selectedControlLgt=nil;

			v:user_onDeselEvent();
		end
	end

	-- load new methods for events...
	b.onInitMenu=function (v)
		v:deselectMe();
		v:user_onInitMenu();
	end;
	b.onCloseMenu=function (v)
		v:user_onCloseMenu();
	end;
	b.onClickEvent=function (v)
		v:user_onClickEvent();

		-- execute the sel sound if needed
		if v.soundSelection~=nil then
			stopAndPlaySoundResource(v.soundSelection);
		end;
	end;
	b.onOverEvent=function (v)
		if selectedControlLgt~=nil and selectedControlLgt~=v then
			selectedControlLgt:deselectMe();
		end;
		v:selectMe();

		-- execute the sel sound if needed
		if v.soundSelection~=nil then
			stopAndPlaySoundResource(v.soundSelection);
		end;
	end;
	b.onOverOutEvent=function (v)
	end;
	b.onTickEvent=function (v)
		if v==selectedControlLgt then
			rawset(globalEnv, v.toolTipVarName, v.toolTipText);
		end;

		v:user_onTickEvent();
	end;

	return b;
end;



-- createLightableContentSwitcher ************
-- a function used for the creation of the content switcher controls for the lightable menu
-- style, telling the way such control should be drawn and used on the game
function createLightableContentSwitcher(
	name,-- name of the control
	drawFunctionVarName,-- name of the var always pointing to the right drawing function
	x, y, w, h,-- coordinates of the control
	onInitMenu,-- code to be executed at the time the user enters to this menu
	onCloseMenu,-- code to be executed at the time the user leaves this menu
	onClickEvent,-- code to be executed when a click on the control is received
	onTickEvent-- code to be executed once per frame
	)

	local s;

	s=createMenuControl(
		name, -- name
		nil, -- surface
		0, 0, -- xSrc, ySrc,
		x, y, w, h, -- x, y, w, h
		nil, -- text
		nil, -- fontName
		0, 0, 0, 0, -- xF, yF, wF, hF
		0, 0, -- h_align, v_align,
		nil, -- onInitMenu
		nil, -- onCloseMenu
		onClickEvent, -- onClickEvent
		function (v) end, -- onOverEvent
		function (v) end, -- onOverOutEvent
		nil -- onTickEvent
		);

	-- set some other data
	s.objectType="switcher";
	s.drawFunctionVarName=drawFunctionVarName;
	s.user_onTickEvent=onTickEvent;
	s.user_onInitMenu=onInitMenu;
	s.user_onCloseMenu=onCloseMenu;

	-- set some useful methods
	s.onTickEvent=function (v)
		v.drawFunction=rawget(globalEnv, v.drawFunctionVarName);--2010
		v:user_onTickEvent();
	end;
	s.onInitMenu=function (v)
		v.drawFunction=nil;
		v:user_onInitMenu();
	end;
	s.onCloseMenu=function (v)
		v.drawFunction=nil;
		v:user_onCloseMenu();
	end;

	return s;
end;



-- createLightableToolTip ************
-- a function used for the creation of the tool tip controls for the lightable menu
-- style, telling the way such control should be drawn and used on the game
function createLightableToolTip(
	name,-- name of the tooltip bar object
	surface,-- surface to be used for the background of this object
	x, y, w, h,-- coordinates of this object on the screen
	xS, yS,-- source coordinates for the background image of this object, if any
	fontName,-- font to use for the text when selected
	xF, yF, wF, hF, h_align, v_align,-- other font data for this object
	toolTipVarName,-- name of the tooltip variable to be used for getting the texts
	onInitMenu,-- code to be executed at the time the user enters to this menu
	onCloseMenu,-- code to be executed at the time the user leaves this menu
	onClickEvent,-- code to be executed when a click on the control is received
	onTickEvent-- code to be executed once per frame
	)

	local t;

	t=createMenuControl(
		name, -- name
		surface, -- surface
		xS, yS, -- xSrc, ySrc,
		x, y, w, h, -- x, y, w, h
		nil, -- text
		fontName, -- fontName
		xF, yF, wF, hF, -- xF, yF, wF, hF
		h_align, v_align, -- h_align, v_align,
		onInitMenu, -- onInitMenu
		onCloseMenu, -- onCloseMenu
		onClickEvent, -- onClickEvent
		function (v) end, -- onOverEvent
		function (v) end, -- onOverOutEvent
		nil -- onTickEvent
		);

	-- set some other data
	t.user_onTickEvent=onTickEvent;
	t.toolTipVarName=toolTipVarName;
	t.onTickEvent=function (v)
		v.text=rawget(globalEnv, v.toolTipVarName);
		v:user_onTickEvent();
	end;

	t.objectType="tooltip";

	return t;
end;




--[[

Order of the events on the controls:

Init

OverOut
Over - Sel - DeSel
Click - Keys, etc
Tick

Close

]]--
