/*
 * Filename      : ai_lua.h
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Monday, November 16 2009
 * Purpose       : Lua Bridge for AI objects
 * Notes         :
 */

 
#ifndef __H_AI_LUA_
#define __H_AI_LUA_

#include "AI/ai.h"
#include "Utilities/lua.h"

#define EPIAR_SHIP "Ship"

class AI_Lua{
	public:
		// Functions to communicate with Lua
		static void RegisterAI(lua_State *luaVM);
        static AI **pushShip(lua_State *luaVM);
        static AI **checkShip(lua_State *luaVM, int index);
		static int newShip(lua_State *luaVM);

		// Actions
		static int ShipAccelerate(lua_State* luaVM);
		static int ShipRotate(lua_State* luaVM);
		static int ShipRadarColor(lua_State* luaVM);

		// Current Ship State
		static int ShipGetAngle(lua_State* luaVM);
		static int ShipGetPosition(lua_State* luaVM);
		static int ShipGetMomentumAngle(lua_State* luaVM);
		static int ShipGetMomentumSpeed(lua_State* luaVM);
		static int ShipGetDirectionTowards(lua_State* luaVM); // Accepts either Angles or Coordinates

		// Ship Properties
		static int ShipGetModelName(lua_State* luaVM);
		static int ShipGetHull(lua_State* luaVM);
	private:
};


#endif /* __H_AI_LUA_ */
