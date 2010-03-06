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

#include "Sprites/spritemanager.h"
#include "Input/input.h"

class Lua {
	public:
		static bool Load( const string& filename );
		static bool Run( string line );
		static bool Call(const char *func, const char *sig="", ...);

		static vector<string> GetOutput();


		static void RegisterFunctions();

		static int console_echo(lua_State *L);
		static int pause(lua_State *L);
		static int getoption(lua_State *L);
		static int setoption(lua_State *L);

		static int unpause(lua_State *L);
		static int ispaused(lua_State *L);
		static int getPlayer(lua_State *L);
		static int focusCamera(lua_State *L);
		static int shakeCamera(lua_State *L);
		static int getSpriteByID(lua_State *L);
		static int getSprites(lua_State *L, int type);
		static int getNearestSprite(lua_State *L, int type=DRAW_ORDER_ALL);
		static int getNearestShip(lua_State *L);
		static int getNearestPlanet(lua_State *L);
		static int getShips(lua_State *L);

		static int getAllianceNames(lua_State *L);
		static int getEngineNames(lua_State *L);
		static int getModelNames(lua_State *L);
		static int getPlanets(lua_State *L);
		static int getWeaponNames(lua_State *L);
		static int getTechnologyNames(lua_State *L);

		static int getModelInfo(lua_State *L);
		static int getPlanetInfo(lua_State *L);
		static int getWeaponInfo(lua_State *L);
		static int getEngineInfo(lua_State *L);
		static int getTechnologyInfo(lua_State *L);
		static int setModelInfo(lua_State *L);
		static int setPlanetInfo(lua_State *L);
		static int setWeaponInfo(lua_State *L);
		static int setEngineInfo(lua_State *L);
		static int setTechnologyInfo(lua_State *L);

		static void pushSprite(lua_State *L,Sprite* sprite);
		static void pushNames(lua_State *L, list<string> *names);
		static void setField(const char* index, int value);
		static void setField(const char* index, float value);
		static void setField(const char* index, const char* value);
		static int getIntField(int index, const char* name);
		static float getNumField(int index, const char* name);
		static string getStringField(int index, const char* name);
		static void stackDump(lua_State *L);

	private:
		static vector<string> buffer;

		static bool Init();
		static bool Close();

		// Internal variables
		static lua_State *L;
		static bool luaInitialized;
};

#endif // __H_LUA__
