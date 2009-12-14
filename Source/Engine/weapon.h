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

#define WEAPON_PROJECTILE  0
#define WEAPON_ENERGY  1

class Weapon {
	public:
		Weapon(void);
		~Weapon(void);

		bool parserCB( string sectionName, string subName, string value );
		void _dbg_PrintInfo( void );
		string GetName(void) {return name;}

		void fireWeapon(float angleToFire, Coordinate worldPosition);
		int getFireDelay(void) {return fireDelay;}
		int getAmmoType() {return ammoType;}
	private:

		string name; //weapons name
		int weaponType; //(energy, explosive, laser, etc)
		Image *image;
		int payload; //intesity of explosion
	    int velocity; //speed of travel
		int acceleration; //speed of acceleration
		int ammoType; //type of ammo used, unique id
		int ammoConsumption; //ammount of ammo to consume per shot
		int fireDelay; //delay between being able to fire agian in ticks
	    int lifetime; //ticks until weapon is destroyed
		//tracking factor
		float direction;
		bool isAccelerating; 
};

#endif
