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

		Model *GetModel(){ return model; };

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
		void AddShipWeaponAndInstall(Weapon *i);
		void AddShipWeaponAndInstall(string weaponName);
		void RemoveShipWeapon(int pos);
		void RemoveShipWeapon(Weapon *i);
		void RemoveShipWeapon(string weaponName);
		void DeinstallShipWeaponAndRemove(Weapon *i);
		void DeinstallShipWeaponAndRemove(string weaponName);
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

		// Situational awareness / AI functions
		void SetAttacker(int attacker) {
			attackedBy = attacker;
			if(attacker == 60) friendly = false;
		}
		int GetAttacker() { return attackedBy; }
		void SetFriendly(int f) { friendly = (f == 1); }
		int GetFriendly() { return (friendly ? 1 : 0 ); }

	protected:
		vector<struct Outfit::ws> weaponSlots; ///< The weapon slot arrangement - accessed directly by Player for loading/saving
	
	private:
		Model *model;
		Engine *engine;
		Animation *flareAnimation;
		Outfit shipStats;
		//power distribution variables
		float damageBooster, engineBooster, shieldBooster;
	
		void ComputeShipStats();

		struct {
			/* Related to ship's condition */
			short int hullDamage; ///< Once the hull takes too much damage, your ship blows apart, hooray! :)
			short int shieldDamage; ///< Your hull doesn`t take damage untill the shield is down
			unsigned int lastWeaponChangeAt; ///< Number of where last weapon change occcured
			//unsigned int lastFiredAt; ///< Number of ticks where last fire event occured
			unsigned int lastFiredAt[35]; ///< Number of ticks where last fire event occured
			unsigned int selectedWeapon; ///< Which weapon is currently selected
			string selectedWeaponName; ///< Which weapon is currently selected
			unsigned int cargoSpaceUsed; ///< Tons of cargo space that are currently filled
			
			/* Flags */
			bool isAccelerating; ///< Cleared by update, set by accelerate (so it's always updated twice a loop)
			bool isDisabled; ///< Set when a ship is disabled (cannot move, may self-repair)
		} status;

		int attackedBy; ///< Sprite id for owner of last projectile to hit this ship
		bool friendly; ///< Is this ship friendly to the player?

		// Weapon Systems
		int ammo[max_ammo]; ///< Contains the quantity of each ammo type on the ship

		vector<Weapon *> shipWeapons; ///< The weapons installed on this ship

		list<Outfit *> outfits;

		// Economic Stuff
		unsigned int credits;
		map<Commodity*,unsigned int> commodities;
};

#endif // __H_SHIP__
