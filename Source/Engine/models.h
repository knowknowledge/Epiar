/**\file			models.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_models__
#define __h_models__

#include "Engine/engines.h"
#include "Graphics/image.h"
#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/components.h"


struct ws {
	string name;            ///< name of the slot
	string mode;            ///< coord mode: "auto" or "manual"
	double x,y;             ///< only matters if mode is manual
	double angle;           ///< angle the weapon will be mounted
	double motionAngle;     ///< should be either 0 (meaning no turrets allowed) or a number > 0 and <= 360
	string content;         ///< name of the weapon it contains (or "" for empty)
	short int firingGroup;  ///< which firing group this slot belongs to
};
typedef struct ws ws_t;

// Abstraction of a single ship model
class Model : public Outfit {
	public:
		Model();
		Model& operator= (const Model&);

		Model( string _name, Image* _image, Engine* _defaultEngine, float _mass, short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _hullStrength, int _shieldStrength, int _msrp, int _cargoSpace, vector<ws_t>& _weaponSlots);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		void _dbg_PrintInfo( void );
		
		Image *GetImage( void ) { return image; }

		Engine* GetDefaultEngine() { return defaultEngine; }
		int GetThrustOffset( void ) { return thrustOffset; }

		vector<ws_t> GetWeaponSlots(){ return this->weaponSlots; }
		int GetWeaponSlotCount();
		bool ConfigureWeaponSlots( xmlDocPtr, xmlNodePtr );
		bool ConfigureWeaponSlots( vector<ws_t>& slots );
		bool ConfigureWeaponSlots();

	private:
		Image *image; ///< The Image used when drawing these ships in space.
		Engine* defaultEngine; ///< The default Engine for this model
		short int thrustOffset; ///< The number of pixels engine flare animation offset
		vector<ws_t> weaponSlots; ///< Slots for Weapons
		// Debug
		void WSDebug(vector<ws_t>&);
		void WSDebug(ws_t);
};

// Class that holds list of all models; manages them
class Models : public Components {
	public:
		static Models *Instance();
		Model* GetModel(string name) { return (Model*) this->Get(name); }
		Component* newComponent() { return new Model(); }

	protected:
		Models() {};
		Models( const Models & );
		Models& operator= (const Models&);

	private:
		static Models *pInstance;
};

#endif // __h_models__
