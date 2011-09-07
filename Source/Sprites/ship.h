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
#include "Engine/weapons.h"
#include "Sprites/projectile.h"
#include <map>

class Ship : public Sprite {
	public:
		Ship();
		~Ship();
		
		bool SetModel( Model *model );

		Model *GetModel(){ return model; };

		bool SetEngine( Engine *engine );
		Sprite *GetSprite();
		string GetModelName();
		string GetEngineName();
		
		void Update( lua_State *L );
		void Rotate( float direction );
		void Accelerate( void );
		void Damage( short int damage );
		void Repair( short int damage );

		void Draw( void );
		bool ChangeWeapon( void );
		FireStatus FirePrimary( int target = -1 );
		FireStatus FireSecondary( int target = -1 );

		// Outfitting Functions
		void AddToShipWeaponList(Weapon *i);
		void AddToShipWeaponList(string weaponName);
		int AddShipWeapon(Weapon *i);
		int AddShipWeapon(string weaponName);
		void RemoveFromShipWeaponList(int pos);
		void RemoveFromShipWeaponList(Weapon *i);
		void RemoveFromShipWeaponList(string weaponName);
		void RemoveShipWeapon(Weapon *i);
		void RemoveShipWeapon(string weaponName);
		void AddAmmo(AmmoType ammoType, int qty);
		void AddOutfit(Outfit *outfit);
		void AddOutfit(string outfitName);
		void RemoveOutfit(Outfit *outfit);
		void RemoveOutfit(string outfitName);
		int GetWeaponSlotCount();
		string GetWeaponSlotName(int i);
		string GetWeaponSlotStatus(int i);
		void SetWeaponSlotStatus(int i, string);
		short int GetWeaponSlotFG(int i);
		void SetWeaponSlotFG(int i, short int);
		map<string,string> GetWeaponSlotContents();

		// Economic Functions
		void SetCredits(unsigned int _credits);
		map<Commodity*,unsigned int> GetCargo();
		int StoreCommodities(string commodity, unsigned int count);
		int DiscardCommodities(string commodity, unsigned int count);

		// Status functions
		float GetDirectionTowards(Coordinate c);
		float GetDirectionTowards(float angle);
		float GetHullIntegrityPct();
		float GetShieldIntegrityPct();
		//Weapon* GetCurrentWeapon();
		short int GetHullDamage(){ return status.hullDamage; }
		void SetHullDamage(short int hd){ status.hullDamage = hd; }
		short int GetShieldDamage(){ return status.shieldDamage; }
		void SetShieldDamage(short int sd){ status.shieldDamage = sd; }

		//int GetCurrentAmmo();
		int GetAmmo(AmmoType type);
		map<Weapon*,int> GetWeaponsAndAmmo();
		vector<Weapon*>* GetWeapons() { return &shipWeapons; }
		list<Outfit*>* GetOutfits() { return &outfits; }
		void SetOutfits(list<Outfit*>* o) { outfits = *o; }

		Engine* GetEngine( void ) const { return engine; }
		unsigned int GetCredits() { return credits; }
		unsigned int GetCargoSpaceUsed() { return status.cargoSpaceUsed; }
		bool IsDisabled() { return status.isDisabled; }
		int GetTotalCost() {  return shipStats.GetMSRP();  }
		
		virtual string GetName( void ) { return ""; }
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_SHIP );
		}
		//Power Distirubution functions
		float GetShieldBoost() {return shieldBooster;}
		float GetEngineBoost() {return engineBooster;}
		float GetDamageBoost() {return damageBooster;}
		void SetShieldBoost(float shield) { shieldBooster = shield;}		
		void SetEngineBoost(float engine) {engineBooster=engine;}
		void SetDamageBoost(float damage) {damageBooster=damage;}

	protected:
		vector<WeaponSlot> weaponSlots; ///< The weapon slot arrangement - accessed directly by Player for loading/saving
	
	private:
		Model *model;
		Engine *engine;
		Animation *flareAnimation;
		Outfit shipStats;
		//power distribution variables
		float damageBooster, engineBooster, shieldBooster;

		FireStatus Fire( unsigned int group, int target = -1 );
		void ComputeShipStats();

		struct {
			/* Related to ship's condition */
			short int hullDamage; ///< Once the hull takes too much damage, your ship blows apart, hooray! :)
			short int shieldDamage; ///< Your hull doesn`t take damage untill the shield is down
			unsigned int lastWeaponChangeAt; ///< Number of where last weapon change occcured
			unsigned int lastFiredAt[32]; ///< Number of ticks where last fire event occured for a given weapon slot
			unsigned int cargoSpaceUsed; ///< Tons of cargo space that are currently filled
			
			/* Flags */
			bool isAccelerating; ///< Cleared by update, set by accelerate (so it's always updated twice a loop)
			bool isRotatingLeft;  ///< Cleared by update, set by turning left (so it's always updated twice a loop)
			bool isRotatingRight;  ///< Cleared by update, set by turning right (so it's always updated twice a loop)
			bool isDisabled; ///< Set when a ship is disabled (cannot move, may self-repair)
		} status;

		// Weapon Systems
		int ammo[max_ammo]; ///< Contains the quantity of each ammo type on the ship

		vector<Weapon *> shipWeapons; ///< The weapons installed on this ship

		list<Outfit *> outfits; ///< The Outfit installed on this ship

		// Economic Stuff
		unsigned int credits;
		map<Commodity*,unsigned int> commodities;
};

#endif // __H_SHIP__
