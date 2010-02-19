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
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Engine/technologies.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single planet
class cPlanet : public Sprite {
	public:
		bool parserCB( string sectionName, string subName, string value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "alliance" ) {
				alliance = value;
			} else PPA_MATCHES( "x" ) {
				Coordinate pos = GetWorldPosition();
				pos.SetX( (double)atof( value.c_str() ) );
				SetWorldPosition( pos );
			} else PPA_MATCHES( "y" ) {
				Coordinate pos = GetWorldPosition();
				pos.SetY( (double)atof( value.c_str() ) );
				SetWorldPosition( pos );
			} else PPA_MATCHES( "landable" ) {
				landable = (atoi( value.c_str() ) !=0);
			} else PPA_MATCHES( "traffic" ) {
				traffic = (short int)atoi( value.c_str() );
			} else PPA_MATCHES( "image" ) {
				Image *image = new Image( value );
				SetImage( image );
			} else PPA_MATCHES( "sphereOfInfluence" ) {
				sphereOfInfluence = atoi( value.c_str() );
			} else PPA_MATCHES( "technology" ) {
				Technology *tech = Technologies::Instance()->GetTechnology( value );
				technologies.push_back(tech);
				technologies.unique();
			}
			SetRadarColor(Color::Get(48, 160, 255));
			return true;
		}
		
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLANET );
		}
		
		void _dbg_PrintInfo( void ) {
			//cout << "Planet: " << name << " at (" << GetWorldPosition() << ") under alliance " << alliance << " with landable option set to " << landable << " and average traffic count of " << traffic << " ships" << endl;
		}
		
		~cPlanet() {
			Image *image = GetImage();
			if( image )
				delete image; // planets delete their own images. not all Sprites do
		}

		string GetName() {return name;}
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
		string name;
		string alliance;
		bool landable;
		short int traffic;
		short int militiaSize;
		int sphereOfInfluence;
		list<Sprite *> militia;
		list<Technology*> technologies;
};

// Class that holds list of all planets; manages them
class Planets {
	public:
		static Planets *Instance();
		
		bool Load( string filename );
		bool Save( string filename );
		
	protected:
		Planets() {};
		Planets( const Planets & );
		Planets& operator= (const Planets&);

	private:
		static Planets *pInstance;
		list<cPlanet *> planets;
};

class Planets_Lua {
	public:
		static void RegisterPlanets(lua_State *L);
		static cPlanet **pushPlanet(lua_State *L);
		static cPlanet **checkPlanet(lua_State *L, int index);

		static int GetName(lua_State* L);
		static int GetType(lua_State* L);
		static int GetID(lua_State* L);
		static int GetPosition(lua_State* L);
		static int GetAlliance(lua_State* L);
		static int GetTraffic(lua_State* L);
		static int GetMilitiaSize(lua_State* L);
		static int GetLandable(lua_State* L);
		static int GetModels(lua_State* L);
		static int GetEngines(lua_State* L);
		static int GetWeapons(lua_State* L);
	private:
};

#endif // __h_planets__
