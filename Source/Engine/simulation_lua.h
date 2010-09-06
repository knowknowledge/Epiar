/**\file			simulation_lua.h
 * \author			Matt Zweig
 * \date			Created: Friday, September 3, 2010
 * \date			Modified: Friday, September 3, 2010
 * \brief			Simulation Managment from Lua
 * \details
 */


#ifndef __H_SIMULATION_LUA__
#define __H_SIMULATION_LUA__

#include "Utilities/lua.h"
#include "Sprites/sprite.h"
#include "Engine/simulation.h"

class Simulation_Lua{
	public:
		static void RegisterSimulation(lua_State *L);

		static void StoreSimulation(lua_State *L, Simulation *sim);
		static Simulation* GetSimulation(lua_State *L);

		static int console_echo(lua_State *L);
		static int pause(lua_State *L);

		// Options Interfaces
		static int getoption(lua_State *L);
		static int setoption(lua_State *L);

		// Input Interfaces
		static int RegisterKey(lua_State *L);
		static int UnRegisterKey(lua_State *L);

		// Simulation Interfaces
		static int unpause(lua_State *L);
		static int ispaused(lua_State *L);
		static int getCamera(lua_State *L);
		static int moveCamera(lua_State *L);
		static int focusCamera(lua_State *L);
		static int shakeCamera(lua_State *L);
		static int getPlayerNames(lua_State *L);
		static int setLastPlanet(lua_State *L);
		static int loadPlayer(lua_State *L);
		static int savePlayer(lua_State *L);
		static int newPlayer(lua_State *L);

		static int NewGatePair(lua_State *L);

		// Sprite Fetchers
		static int getPlayer(lua_State *L);
		static int getSpriteByID(lua_State *L);
		static int getSprites(lua_State *L, int type);
		static int getNearestSprite(lua_State *L, int type=DRAW_ORDER_ALL);
		static int getNearestShip(lua_State *L);
		static int getNearestPlanet(lua_State *L);
		static int getShips(lua_State *L);


		// Game Components
		static int getCommodityNames(lua_State *L);
		static int getAllianceNames(lua_State *L);
		static int getEngineNames(lua_State *L);
		static int getModelNames(lua_State *L);
		static int getPlanets(lua_State *L);
		static int getWeaponNames(lua_State *L);
		static int getOutfitNames(lua_State *L);
		static int getTechnologyNames(lua_State *L);
		static int getPlanetNames(lua_State *L);

		// Information about Components
		static int getMSRP(lua_State *L);

		// Editor Commands
		static int getCommodityInfo(lua_State *L);
		static int getAllianceInfo(lua_State *L);
		static int getModelInfo(lua_State *L);
		static int getPlanetInfo(lua_State *L);
		static int getWeaponInfo(lua_State *L);
		static int getEngineInfo(lua_State *L);
		static int getOutfitInfo(lua_State *L);
		static int getTechnologyInfo(lua_State *L);
		static int setInfo(lua_State *L);
		static int saveComponents(lua_State *L);
		static int listImages(lua_State *L);

		static void pushSprite(lua_State *L,Sprite* sprite);
		static void pushComponents(lua_State *L, list<Component*> *components);
	private:
		
};

#endif // __H_SIMULATION_LUA__
