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

class Lua {
	public:
		static bool Load( string filename );
		static bool Run( string line );
		static vector<string> GetOutput();

		static lua_State *luaVM; // public for debugging purposes
	private:
		static vector<string> buffer;

		static bool Init();
		static bool Close();
		
		static bool luaInitialized;
};

#endif // __H_LUA__
