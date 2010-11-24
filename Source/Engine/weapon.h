/*
 * Filename      : weapon.h
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : 
 * Notes         :
 */
#ifndef __H_WEAPON__
#define __H_WEAPON__

#include "Sprites/sprite.h"
#include "Audio/sound.h"
#include "Utilities/components.h"
#include "Engine/outfit.h"

#define WEAPON_PROJECTILE  0
#define WEAPON_ENERGY  1

enum FireStatus {FireSuccess, FireNoWeapons, FireNotReady, FireNoAmmo, FireEmptyGroup, FireUnknown};

enum AmmoType {
	energy_ammo,
	bullet_ammo,
	missile_ammo,
	torpedo_ammo,
	max_ammo // This AmmoType is used only for iterating accross the Types
};

class Weapon : public Outfit {
	public:
		Weapon(void);
		Weapon& operator=(const Weapon& other);
		Weapon( string _name, Image* _image, Image* _pic, int _weaponType, int _payload, int _velocity, int _acceleration, AmmoType _ammoType, int _ammoConsumption, int _fireDelay, int _lifetime, Sound* _sound, float _tracking, int _msrp);
		~Weapon(void);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		static string AmmoTypeToName(AmmoType type);
		static AmmoType AmmoNameToType(string typeName );

		Image *GetImage(void) {return image;}
		Sound *GetSound(void) {return sound;}
		int GetType(void) {return weaponType;}
		int GetPayload(void) {return payload;}
		int GetVelocity(void) {return velocity;}
		int GetAcceleration(void) {return acceleration;}
		AmmoType GetAmmoType(void) {return ammoType;}
		int GetAmmoConsumption(void) { return ammoConsumption;}
		int GetFireDelay(void) {return fireDelay;}
		int GetLifetime(void) {return lifetime;}
		float GetTracking(void) {return tracking;}

	private:
		Image *image;
		Sound *sound; //Sound the weapon makes
		int weaponType; //(energy, explosive, laser, etc)
		int payload; //intesity of explosion
		int velocity; //speed of travel
		int acceleration; //speed of acceleration
		AmmoType ammoType; //type of ammo used, unique id
		int ammoConsumption; //ammount of ammo to consume per shot
		int fireDelay; //delay between being able to fire agian in ticks
		int lifetime; //ticks until weapon is destroyed
		float tracking;
};

#endif
