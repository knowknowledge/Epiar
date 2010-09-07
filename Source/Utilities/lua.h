/**\file			lua.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 5, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Provides abilities to load, store, and run Lua scripts
 * \details
 * To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#ifndef __H_LUA__
#define __H_LUA__

#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lualib.h>
#	include <lauxlib.h>
#ifdef __cplusplus
}
#endif

class Lua {
	public:
		static bool Init();
		static bool Close();

		static bool Load( const string& filename );
		static int Run( string line, bool allowReturns=false );
		static bool Call(const char *func, const char *sig="", ...);

		static lua_State* CurrentState() { return L;}

		static void RegisterFunctions();

		static void RegisterGlobal(string name, int value);
		static void RegisterGlobal(string name, float value);
		static void RegisterGlobal(string name, string value);

		// Get/Set Table Fields
		static void setField(const char* index, int value);
		static void setField(const char* index, float value);
		static void setField(const char* index, const char* value);
		static int getIntField(int index, const char* name);
		static float getNumField(int index, const char* name);
		static string getStringField(int index, const char* name);
		// Not a native type but pretty vital at times.
		static void pushStringList(lua_State *L, list<string> *names);
		static list<string> getStringListField(int index);
		static list<string> getStringListField(int index, const char* name);

		static void stackDump(lua_State *L);

	private:
		static int ErrorCatch(lua_State *L);

		// Internal variables
		static lua_State *L;
		static bool luaInitialized;
};

#endif // __H_LUA__
