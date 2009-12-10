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

class Weapon {//: public Sprite {
	public:
		Weapon(void);
		~Weapon(void);

		bool parserCB( string sectionName, string subName, string value );
		virtual int GetDrawOrder( void );
		void _dbg_PrintInfo( void );
		void fireWeapon(float angleToFire, Coordinate worldPosition);
		int getFireDelay(void) {return fireDelay;}
		void setAmmo (int value) {ammo = value;} 
	private:

		string name; //weapons name
		int weaponType; //(energy, explosive, laser, etc)
		string modelName;
		int payload; //intesity of explosion
	    int velocity; //speed of travel
		int acceleration; //speed of acceleration
		int ammoType; //type of ammo used
		int ammoConsumption; //ammount of ammo to consume per shot
		int ammo;
		int fireDelay; //delay between being able to fire agian in ticks
	    int lifetime; //ticks until weapon is destroyed
		//tracking factor
	   	Model *model;
		Animation *flareAnimation;
		float direction;
		bool isAccelerating; 
};

#endif
