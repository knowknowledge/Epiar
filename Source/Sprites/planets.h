/**\file			planets.h
 * \author			Chris Thielen (chris@epiar.net)
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
#include "Engine/alliances.h"

// Abstraction of a single planet
class Planet : public Sprite, public Component {
	public:
		Planet();
		Planet& operator=(const Planet& other);
		Planet( string _name, float _x, float _y, Image* _image, Alliance* _alliance, bool _landable, int _traffic, int _militiaSize, int _sphereOfInfluence, list<Technology*> _technologies);
		
		void Update( lua_State *L );
		void GenerateTraffic( lua_State *L );

		virtual int GetDrawOrder( void ) { return( DRAW_ORDER_PLANET ); }
		
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
		~Planet();

		list<Model*> GetModels();
		list<Engine*> GetEngines();
		list<Weapon*> GetWeapons();
		list<Outfit*> GetOutfits();
		Alliance* GetAlliance() const {return alliance;}
		short unsigned int GetTraffic() const {return traffic;}
		short unsigned int GetMilitiaSize() const {return militiaSize;}
		bool GetLandable() const {return landable;}
		int GetInfluence() const {return sphereOfInfluence;}
		list<Technology*> GetTechnologies() const { return technologies;}

		bool GetForbidden() {return forbidden;}
		void SetForbidden(bool f) {forbidden = f;}

	private:
		Alliance* alliance;
		bool landable;
		bool forbidden;
		short unsigned int traffic;
		short unsigned int militiaSize;
		int sphereOfInfluence;
		list<Technology*> technologies;
		Image* surface;
		string summary;

		Uint32 lastTrafficTime;
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
		static int NewPlanet(lua_State* L);

		static int GetName(lua_State* L);
		static int GetType(lua_State* L);
		static int GetID(lua_State* L);
		static int GetPosition(lua_State* L);
		static int GetSize(lua_State* L);
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
	private:
};

#endif // __h_planets__
