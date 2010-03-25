/**\file			planets.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Monday, November 16 2009
 * \brief
 * \details
 */


#ifndef __h_planets__
#define __h_planets__

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#define EPIAR_PLANET "Planet"

#include "includes.h"
#include "Sprites/sprite.h"
#include "Utilities/coordinate.h"
#include "Utilities/components.h"
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Engine/technologies.h"

// Abstraction of a single planet
class Planet : public Sprite, public Component {
	public:
		Planet();
		Planet& operator=(const Planet& other);
		Planet( string _name, float _x, float _y, Image* _image, string _alliance, bool _landable, int _traffic, int _militiaSize, int _sphereOfInfluence, list<Technology*> _technologies);
		bool parserCB( string sectionName, string subName, string value );
		
		virtual int GetDrawOrder( void ) { return( DRAW_ORDER_PLANET ); }
		
		void _dbg_PrintInfo( void );

		xmlNodePtr ToXMLNode(string componentName);
		
		~Planet();

		string GetAlliance() {return alliance;}
		short int GetTraffic() {return traffic;}
		short int GetMilitiaSize() {return militiaSize;}
		bool GetLandable() {return landable;}
		int GetInfluence() {return sphereOfInfluence;}
		list<Technology*> GetTechnologies() { return technologies;}
		list<Model*> GetModels();
		list<Engine*> GetEngines();
		list<Weapon*> GetWeapons();
		
	private:
		string alliance;
		bool landable;
		short int traffic;
		short int militiaSize;
		int sphereOfInfluence;
		list<Technology*> technologies;
};

// Class that holds list of all planets; manages them
class Planets : public Components {
	public:
		static Planets *Instance();
		Planet *GetPlanet( string& PlanetName ) { return (Planet*) this->Get(PlanetName); }
		Component* newComponent() { return new Planet(); }
		
	protected:
		Planets() {};
		Planets( const Planets & );
		Planets& operator= (const Planets&);

	private:
		static Planets *pInstance;
};

class Planets_Lua {
	public:
		static void RegisterPlanets(lua_State *L);
		//static Planet **pushPlanet(lua_State *L);
		static Planet *checkPlanet(lua_State *L, int index);
		static int Get(lua_State* L);

		static int GetName(lua_State* L);
		static int GetType(lua_State* L);
		static int GetID(lua_State* L);
		static int GetPosition(lua_State* L);
		static int GetAlliance(lua_State* L);
		static int GetTraffic(lua_State* L);
		static int GetMilitiaSize(lua_State* L);
		static int GetInfluence(lua_State* L);
		static int GetLandable(lua_State* L);
		static int GetModels(lua_State* L);
		static int GetEngines(lua_State* L);
		static int GetWeapons(lua_State* L);
	private:
};

#endif // __h_planets__
