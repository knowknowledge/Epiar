/**\file			ship.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Friday, November 14, 2009
 * \brief
 * \details
 */


#ifndef __H_SHIP__
#define __H_SHIP__

#include "Engine/models.h"
#include "Sprites/sprite.h"
#include "Engine/commodities.h"
#include "Engine/weapon.h"
#include "Sprites/projectile.h"
#include <map>

class Ship : public Sprite {
	public:
		Ship();
		~Ship();
		
		bool SetModel( Model *model );
		bool SetEngine( Engine *engine );
		Sprite *GetSprite();
		string GetModelName();
		string GetEngineName();
		
		void Update( void );
		void Rotate( float direction );
		void Accelerate( void );
		void Damage( short int damage );
		void Repair( short int damage );

		void Draw( void );
		FireStatus Fire( int target = -1 );
		bool ChangeWeapon( void );

		// Outfitting Functions
		void AddShipWeapon(Weapon *i);
		void AddShipWeapon(string weaponName);
		void RemoveShipWeapon(int pos);
		void AddAmmo(AmmoType ammoType, int qty);
		void AddOutfit(Outfit *outfit);
		void AddOutfit(string outfitName);

		// Economic Functions
		void SetCredits(unsigned int _credits);
		map<Commodity*,unsigned int> GetCargo();
		int StoreCommodities(string commodity, unsigned int count);
		int DiscardCommodities(string commodity, unsigned int count);

		// Status functions
		float GetDirectionTowards(Coordinate c);
		float GetDirectionTowards(float angle);
		float GetHullIntegrityPct();
		Weapon* GetCurrentWeapon();
		int GetCurrentAmmo();
		int GetAmmo(AmmoType type);
		map<Weapon*,int> GetWeaponsAndAmmo();
		list<Outfit*>* GetOutfits() { return &outfits; }
		Engine* GetEngine( void ) const { return engine; }
		unsigned int GetCredits() { return credits; }
		unsigned int GetCargoSpaceUsed() { return status.cargoSpaceUsed; }
		bool IsDisabled() { return status.isDisabled; }
		
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_SHIP );
		}
		
	private:
		Model *model;
		Engine *engine;
		Animation *flareAnimation;
		Outfit shipStats;

		void ComputeShipStats();

		struct {
			/* Related to ship's condition */
			short int hullDamage; ///< Once the hull takes too much damage, your ship blows apart, hooray! :)
			unsigned int lastWeaponChangeAt; ///< Number of where last weapon change occcured
			unsigned int lastFiredAt; ///< Number of ticks where last fire event occured
			unsigned int selectedWeapon; ///< Which weapon is currently selected
			unsigned int cargoSpaceUsed; ///< Tons of cargo space that are currently filled
			
			/* Flags */
			bool isAccelerating; ///< Cleared by update, set by accelerate (so it's always updated twice a loop)
			bool isDisabled; ///< Set when a ship is disabled (cannot move, may self-repair)
		} status;

		// Weapon Systems
		int ammo[max_ammo]; ///< Contains the quantity of each ammo type on the ship
		vector<Weapon *> shipWeapons; ///< 

		list<Outfit *> outfits;

		// Economic Stuff
		unsigned int credits;
		map<Commodity*,unsigned int> commodities;
};

#endif // __H_SHIP__
