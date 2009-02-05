/*
 * Filename      : lua.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#ifndef __H_LUA__
#define __H_LUA__

#include "includes.h"
#include "Lua/src/lua.h"
#include "Lua/src/lualib.h"
#include "Lua/src/lauxlib.h"

// A LuaScript instance is required for each script. The class uses static variables, however, to share
// one lua_State, meaning only one Lua script can run at once, so this is almost surely not thread-safe.
class LuaScript {
	public:
		LuaScript();
		LuaScript( string filename );
		~LuaScript();
		bool Load( string filename );
		bool Run();

		static lua_State *luaVM; // public for debugging purposes
	private:
		bool InitLua();
		bool CloseLua();
		
		static bool luaInitialized;
		//static lua_State *luaVM;
		static int numScriptsLoaded; // when destructor decrements this to zero, we de-init the Lua VM
};

#endif // __H_LUA__
