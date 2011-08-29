/**\file			planets.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Monday, November 16 2009
 * \brief
 * \details
 */

#ifndef __h_planets__
#define __h_planets__

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
		Planet( string _name,
				float _x,
				float _y,
				Image* _image,
				Alliance* _alliance,
				bool _landable,
				int _traffic,
				int _militiaSize,
				int _sphereOfInfluence,
				Image* _surface,
				string _summary,
				list<Technology*> _technologies
		);
		
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
		string GetSummary() const {return summary;}
		int GetInfluence() const {return sphereOfInfluence;}
		Image* GetSurfaceImage() const {return surface;}
		list<Technology*> GetTechnologies() const { return technologies;}

		bool GetForbidden() {return forbidden;}
		void SetForbidden(bool f) {forbidden = f;}
		void SetInfluence(int influence) {sphereOfInfluence = influence;}

	private:
		Alliance* alliance;
		bool landable;
		bool forbidden;
		short unsigned int traffic;
		short unsigned int militiaSize;
		int sphereOfInfluence;
		Image* surface;
		string summary;
		list<Technology*> technologies;

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


#endif // __h_planets__
