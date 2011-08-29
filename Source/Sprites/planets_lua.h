/**\file			planets_lua.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, June 4 2011
 * \date			Modified: Saturday, June 4 2011
 * \brief
 * \details
 */

#ifndef __h_lua_planets__
#define __h_lua_planets__

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "includes.h"
#include "Sprites/planets.h"

#define EPIAR_PLANET "Planet"

class Planets_Lua {
	public:
		static void RegisterPlanets(lua_State *L);
		//static Planet **pushPlanet(lua_State *L);
		static Planet *checkPlanet(lua_State *L, int index);
		static int Get(lua_State* L);
		static int NewPlanet(lua_State* L);

		static int GetName(lua_State* L);
		static int GetType(lua_State* L);
		static int GetID(lua_State* L);
		static int GetPosition(lua_State* L);
		static int GetSize(lua_State* L);
		static int GetSummary(lua_State* L);
		static int GetSurfaceImage(lua_State* L);
		static int GetAlliance(lua_State* L);
		static int GetTraffic(lua_State* L);
		static int GetMilitiaSize(lua_State* L);
		static int GetInfluence(lua_State* L);
		static int GetLandable(lua_State* L);
		static int GetModels(lua_State* L);
		static int GetEngines(lua_State* L);
		static int GetWeapons(lua_State* L);
		static int GetOutfits(lua_State* L);
		static int GetForbidden(lua_State* L);
		static int SetForbidden(lua_State* L);

		// Editor Features
		static int SetPosition(lua_State* L);
		static int SetInfluence(lua_State* L);
	private:
};

#endif // __h_lua_planets__

