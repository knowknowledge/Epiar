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
		static void RegisterEditor(lua_State *L);

		static void StoreSimulation(lua_State *L, Simulation *sim);
		static Simulation* GetSimulation(lua_State *L);

		static int Console_echo(lua_State *L);
		static int Pause(lua_State *L);

		// Options Interfaces
		static int Getoption(lua_State *L);
		static int Setoption(lua_State *L);

		// Input Interfaces
		static int RegisterKey(lua_State *L);
		static int UnRegisterKey(lua_State *L);

		// Simulation Interfaces
		static int Unpause(lua_State *L);
		static int Ispaused(lua_State *L);
		static int GetCamera(lua_State *L);
		static int MoveCamera(lua_State *L);
		static int FocusCamera(lua_State *L);
		static int ShakeCamera(lua_State *L);
		static int GetPlayerNames(lua_State *L);
		static int SetLastPlanet(lua_State *L);
		static int LoadPlayer(lua_State *L);
		static int SavePlayer(lua_State *L);
		static int NewPlayer(lua_State *L);

		static int NewGatePair(lua_State *L);

		// Sprite Fetchers
		static int GetPlayer(lua_State *L);
		static int GetSpriteByID(lua_State *L);
		static int GetSprites(lua_State *L, int type);
		static int GetNearestSprite(lua_State *L, int type=DRAW_ORDER_ALL);
		static int GetNearestShip(lua_State *L);
		static int GetNearestPlanet(lua_State *L);
		static int GetShips(lua_State *L);
		static int GetPlanets(lua_State *L);
		static int GetGates(lua_State *L);

		// Game Components
		static int GetCommodityNames(lua_State *L);
		static int GetAllianceNames(lua_State *L);
		static int GetEngineNames(lua_State *L);
		static int GetModelNames(lua_State *L);
		static int GetWeaponNames(lua_State *L);
		static int GetOutfitNames(lua_State *L);
		static int GetTechnologyNames(lua_State *L);
		static int GetPlanetNames(lua_State *L);
		static int GetGateNames(lua_State *L);

		// Information about Components
		static int GetMSRP(lua_State *L);
		static int GetSimulationInfo(lua_State *L);
		static int GetCommodityInfo(lua_State *L);
		static int GetAllianceInfo(lua_State *L);
		static int GetModelInfo(lua_State *L);
		static int GetPlanetInfo(lua_State *L);
		static int GetGateInfo(lua_State *L);
		static int GetWeaponInfo(lua_State *L);
		static int GetEngineInfo(lua_State *L);
		static int GetOutfitInfo(lua_State *L);
		static int GetTechnologyInfo(lua_State *L);

		// Editor Commands
		static int SetInfo(lua_State *L);
		static int SetDefaultPlayer(lua_State *L);
		static int GetDefaultPlayer(lua_State *L);
		static int SaveComponents(lua_State *L);
		static int ListImages(lua_State *L);
		static int ListAnimations(lua_State *L);
		static int ListSounds(lua_State *L);
		static int SetDescription(lua_State *L);

		static void PushSprite(lua_State *L,Sprite* sprite);
		static void PushComponents(lua_State *L, list<Component*> *components);
	private:
};

#endif // __H_SIMULATION_LUA__
