/**\file			outfit.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, April 29, 2010
 * \date			Modified: Thursday, April 29, 2010
 * \brief
 * \details
 */

#ifndef __H_OUTFIT__
#define __H_OUTFIT__

#include "Engine/outfit.h"
#include "Graphics/image.h"
#include "Utilities/components.h"

class Outfit : public Component {
	public:
		Outfit();

		Outfit(
				int _msrp,
				Image* _picture,
				string _description,
				float _rotPerSecond,
				float _maxSpeed,
				float _forceOutput,
				float _mass,
				int _cargoSpace,
				int _surfaceArea,
				int _hullStrength,
				int _shieldStrength
				);

		Outfit& operator= (const Outfit& other);
		Outfit operator+ (const Outfit& other);
		Outfit& operator+= (const Outfit& other);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		int GetMSRP() { return msrp; }
		void SetMSRP( int _msrp ) { msrp = _msrp; }

		Image* GetPicture() { return picture; }
		void SetPicture( Image* _picture ) { picture = _picture; }

		string GetDescription() { return description; }
		void SetDescription( string _description ) { description = _description; }

		float GetRotationsPerSecond() { return rotPerSecond; }
		void SetRotationsPerSecond( float _rotPerSecond ) { rotPerSecond = _rotPerSecond; }

		float GetMaxSpeed() { return maxSpeed; }
		void SetMaxSpeed( float _maxSpeed ) { maxSpeed = _maxSpeed; }

		float GetForceOutput() { return forceOutput; }
		void SetForceOutput( float _forceOutput ) { forceOutput = _forceOutput; }

		float GetMass() { return mass; }
		void SetMass( float _mass ) { mass = _mass; }

		int GetCargoSpace() { return cargoSpace; }
		void SetCargoSpace( int _cargoSpace ) { cargoSpace = _cargoSpace; }

		int GetSurfaceArea() { return surfaceArea; }
		void SetSurfaceArea( int _surfaceArea ) { surfaceArea = _surfaceArea; }

		int GetHullStrength() { return hullStrength; }
		void SetHullStrength( int _hullStrength ) { hullStrength = _hullStrength; }

		int GetShieldStrength() { return shieldStrength; }
		void SetShieldStrength( int _shieldStrength ) { shieldStrength = _shieldStrength; }

	protected:
		int msrp; ///< The cost in credits.
		Image* picture; ///< The image used in the store.
		string description; ///< The description of the item.

		// Navigation Stats
		float rotPerSecond; ///< The degrees of rotatation per second.
		float maxSpeed; ///< The maximum momentum magnitude.
		float forceOutput; ///< The force of the engine.

		// Capacity Stats
		float mass; ///< The mass that this consumes.
		int cargoSpace; ///< The amount of cargo hold area that this consumes.
		int surfaceArea; ///< The amount of mountable surface area that this consumes.

		// Defensive Stats
		int hullStrength; ///< The amount of damage the hull can absorb.
		int shieldStrength; ///< The amount of damage the shields can absorb.

		// Offensive Stats
	private:
};

// Class that holds list of all models; manages them
class Outfits : public Components {
	public:
		static Outfits *Instance();
		Outfit* GetOutfit(string name) { return (Outfit*) this->Get(name); }
		Component* newComponent() { return new Outfit(); }

	protected:
		Outfits() {};
		Outfits( const Outfits & );
		Outfits& operator= (const Outfits&);

	private:
		static Outfits *pInstance;
};

#endif // define __H_OUTFIT__
