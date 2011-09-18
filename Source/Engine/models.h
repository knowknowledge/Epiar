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
#include "Engine/weapons.h"
#include "Graphics/image.h"
#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/components.h"


typedef struct WeaponSlot {
	string name;            ///< name of the slot
	double x,y;             ///< The relative X and Y offsets of this weapon.
	double angle;           ///< angle the weapon will be mounted
	double motionAngle;     ///< should be either 0 (meaning no turrets allowed) or a number > 0 and <= 360
	Weapon* content;         ///< name of the weapon it contains (or "" for empty)
	short int firingGroup;  ///< which firing group this slot belongs to
};

// Abstraction of a single ship model
class Model : public Outfit {
	public:
		Model();
		Model& operator= (const Model&);

		Model( string _name,
				Image* _image,
				string _description,
				Engine* _defaultEngine,
				float _mass,
				short int _thrustOffset,
				float _rotPerSecond,
				float _maxSpeed,
				int _hullStrength,
				int _shieldStrength,
				int _msrp,
				int _cargoSpace,
				vector<WeaponSlot>& _weaponSlots);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
		Image *GetImage( void ) { return image; }

		Engine* GetDefaultEngine() { return defaultEngine; }
		int GetThrustOffset( void ) { return thrustOffset; }

		vector<WeaponSlot> GetWeaponSlots(){ return this->weaponSlots; }
		int GetWeaponSlotCount();
		bool ConfigureWeaponSlots( xmlDocPtr, xmlNodePtr );
		bool ConfigureWeaponSlots( vector<WeaponSlot>& slots );

	private:
		Image *image; ///< The Image used when drawing these ships in space.
		Engine* defaultEngine; ///< The default Engine for this model
		short int thrustOffset; ///< The number of pixels engine flare animation offset
		vector<WeaponSlot> weaponSlots; ///< Slots for Weapons
		// Debug
		void WSDebug(vector<WeaponSlot>&);
		void WSDebug(WeaponSlot);
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
