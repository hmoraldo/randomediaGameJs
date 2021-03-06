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

#include "resinfoman.h"// for managing the information related with the use of resources, ie: animation scripts
#include "globals.h"// for using the program objects
#include "gameglobals.h"// for using the game objects
#include "massDeallocator.h"// for accessing to the massDeallocator object, useful for massive deallocations
#include "programinfo.h"// for general info about the program itself
#include "scriptman.h"// used for accessing to the script manager object
#include <string.h>

// header files for using lua
extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
}// extern

// RES INFO MEMORY MANAGEMENT objects
static massDeallocatorClass resMemory;

// FUNCTIONS OF RESOURCE INFO MANAGEMENT

// initializeResInfo **********
// This function initializes all the data structures related to
// bringing information about the use of resources (for example, the
// animations and frames data).
bool initializeResInfo()
{
 	lua_State* lstate;

	logger.logLine("Initializing resources info", true, false);
 
	// get the lua state object, to be used below
	lstate=scriptMan.getLuaObject();

	// set up the memory mass deallocator we'll use here
	if (!resMemory.init()) {
		logger.logLine("Error initializing mass deallocator");
		return false;
	}

 	// now we have successfully executed our scripts... let's use
	// the data in those to define the composition of our frame lists
	lua_getglobal(lstate, "animsFrameList");
	lua_pushnil(lstate);
	while (lua_next(lstate, -2)!=0) {
		// for every specific frame list...
		// we get the name of the frame list to traverse
		char* curr=(char*)(lua_tostring(lstate, -1));
		frameDataStruct* frData;
		int size;
		int i;
		#ifdef PROGMODE_DEVELOPER_YES
		logger.logLine("Loading animation: %s",
			true, false, true, curr);
		#endif
		// ...
		lua_getglobal(lstate, curr);
		// let's see the size of the table
		size=lua_rawlen(lstate, -1);
		frData=new frameDataStruct[size+1];
		resMemory.addMemAllocation(frData, MEMTYPE_FRAMEDATA, false);
		// we add the empty node at the end of the table (position size
		// in a size+1 list)
		memset(&(frData[size]), 0, sizeof(frData[0]));
		// let's traverse that table...
		lua_pushnil(lstate);
		i=0;
		while (lua_next(lstate, -2)!=0) {
			char* framename;// name of the frame itself 
			char* resname; // name of the resource to use 
			char* eventname; // name of the event...
			char* groupname; // name of the frames group
			int x, y, w, h, // x, y, w, h, as usual 
				cx, cy, cw, ch,// cX, cY, cW, cH, as usual, but in absolute coordinates! 
				hx, hy, // handlerX, handlerY, as usual, but in absolute coordinates 
				sx, sy, // spawnBulletsX, spawnBulletsY, as usual, but in absolute coordinates 
				ax, ay, // adherenceX, adherenceY, as usual, but in absolute coordinates 
				aNF; // autoNextFrame, just as usual 
			double aSVT; // autoShowVirtualTime, just as autoShowVirtualFrames but with time measured in 
	
			// and for each node, let's take what's at there
			int* value;
			// framename
			lua_pushstring(lstate, "framename");
			lua_gettable(lstate, -2);
			if (lua_isnil(lstate, -1)) {
				framename=NULL;
			}else{
				const char* str=lua_tostring(lstate, -1);
				framename=new char[strlen(str)+1];
				strcpy(framename, str);
				resMemory.addMemAllocation(framename, MEMTYPE_CHAR, false);
			}
			lua_pop(lstate, 1);
			// resname
			lua_pushstring(lstate, "resname");
			lua_gettable(lstate, -2);
			if (lua_isnil(lstate, -1)) {
				resname=NULL;
			}else{
				const char* str=lua_tostring(lstate, -1);
				resname=new char[strlen(str)+1];
				strcpy(resname, str);
				resMemory.addMemAllocation(resname, MEMTYPE_CHAR, false);
			}
			lua_pop(lstate, 1);
			// eventname
			lua_pushstring(lstate, "eventName");
			lua_gettable(lstate, -2);
			if (lua_isnil(lstate, -1)) {
				eventname=NULL;
			}else{
				const char* str=lua_tostring(lstate, -1);
				eventname=new char[strlen(str)+1];
				strcpy(eventname, str);
				resMemory.addMemAllocation(eventname, MEMTYPE_CHAR, false);
			}
			lua_pop(lstate, 1);
			// groupname
			lua_pushstring(lstate, "groupName");
			lua_gettable(lstate, -2);
			if (lua_isnil(lstate, -1)) {
				groupname=NULL;
			}else{
				const char* str=lua_tostring(lstate, -1);
				groupname=new char[strlen(str)+1];
				strcpy(groupname, str);
				resMemory.addMemAllocation(groupname, MEMTYPE_CHAR, false);
			}
			lua_pop(lstate, 1);
			// x
			value=&x;
			lua_pushstring(lstate, "x");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// y
			value=&y;
			lua_pushstring(lstate, "y");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// w
			value=&w;
			lua_pushstring(lstate, "w");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// h
			value=&h;
			lua_pushstring(lstate, "h");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// cx
			value=&cx;
			lua_pushstring(lstate, "cx");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// cy
			value=&cy;
			lua_pushstring(lstate, "cy");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// cw
			value=&cw;
			lua_pushstring(lstate, "cw");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// ch
			value=&ch;
			lua_pushstring(lstate, "ch");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// hx
			value=&hx;
			lua_pushstring(lstate, "hx");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// hy
			value=&hy;
			lua_pushstring(lstate, "hy");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// sx
			value=&sx;
			lua_pushstring(lstate, "sx");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// sy
			value=&sy;
			lua_pushstring(lstate, "sy");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// ax
			value=&ax;
			lua_pushstring(lstate, "ax");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// ay
			value=&ay;
			lua_pushstring(lstate, "ay");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// aNF
			value=&aNF;
			lua_pushstring(lstate, "aNF");
			lua_gettable(lstate, -2);
			*value=int(lua_tonumber(lstate, -1));
			lua_pop(lstate, 1);
			// aSVT
			lua_pushstring(lstate, "aSVT");
			lua_gettable(lstate, -2);
			aSVT=lua_tonumber(lstate, -1);
			lua_pop(lstate, 1);
	
			// let's add the node to our list
			frameDataStruct frTemp=createFrame(
				framename,
				resname,
				x, y, w, h,
				cx, cy, cw, ch,
				hx, hy,
				sx, sy,
				ax, ay,
				aNF,
				aSVT,
				eventname,
				groupname);
			frData[i]=frTemp;
	
			// let's remove the value
			lua_pop(lstate, 1);
			i++;
		}

		// now we remove the 'curr' table we were traversing itself
		lua_pop(lstate, 1);

		frameListDataStruct* listNode=getFrameListNodeByName(curr);
		if (listNode==NULL) {
			// error
			logger.logLine("No list node for %s", true, false, true,
				curr);
			return false;
		}else{
			// all ok, continue working
			listNode->frameList=frData;
			listNode->size=size;
		}

		// let's remove the value got from the traversed key
		lua_pop(lstate, 1);
	}

	// remove the animsFrameList table
	lua_pop(lstate, 1);

	return true;// all ok
}// initializeResInfo 



// closeResourcesInfo **********
// This function closes all the data structures that were set up
// with initializeResInfo().
bool closeResourcesInfo()
{
	if (resMemory.isReady()) {
		// deallocate all
		if (!resMemory.massDeallocate()) {
			logger.logLine("Could not mass deallocate resMemory");
			return false;
		}

		// close some remaining objects
		resMemory.close();
	}

	return true;// all ok
}// closeResourcesInfo
