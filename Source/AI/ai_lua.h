/*
 * Filename      : ai_lua.h
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Thursday, October 29, 2009
 * Purpose       : Lua Bridge for AI objects
 * Notes         :
 */

 
#ifndef __H_AI_LUA_
#define __H_AI_LUA_


class AI_Lua{
	public:
		// Functions to communicate with Lua
		static void RegisterAI(lua_State *luaVM);
		static int newShip(lua_State *luaVM);

		static int ShipAccelerate(lua_State* luaVM);
		static int ShipRotate(lua_State* luaVM);
		static int ShipGetAngle(lua_State* luaVM);
		static int ShipGetPosition(lua_State* luaVM);
		static int ShipGetMomentumAngle(lua_State* luaVM);
		static int ShipGetMomentumSpeed(lua_State* luaVM);
		static int ShipGetDirectionTowards(lua_State* luaVM); // Accepts either Angles or Coordinates
	private:
};


#endif /* __H_AI_LUA_ */
