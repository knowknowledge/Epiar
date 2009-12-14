/*
 * Filename      : weaponSystem.h
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, December 11, 2009
 * Last Modified : Friday, December 11, 2009
 * Purpose       : Header for Weapon systems for ships
 * Notes         :
 */

#ifndef WEAPONSYSTEM_H_
#define WEAPONSYSTEM_H_

#include "Engine/weapon.h"
#include "Sprites/projectile.h"
#include <map>

class WeaponSystem {
	public:
	WeaponSystem(void);
	~WeaponSystem(void);

	// Outfitting Functions
	void addShipWeapon(Weapon *i);
	void addShipWeapon(string weaponName);
	void removeShipWeapon(int pos);
	void addAmmo(int qty);

	// Action Functions
	Projectile* fireWeapon(float angleToFire, Coordinate worldPosition, int offset);
	string changeWeaponNext(void);

	// Status Functions
	Weapon* currentWeapon();
	int currentAmmo();
	
	private:
	multimap <int,int> ammo; //contains the quantity of each ammo type on the ship
	
	unsigned int lastWeaponChangeAt; //number of where last weapon change occcured
	unsigned int lastFiredAt; //number of ticks where last fire event occured
	vector<Weapon *> shipWeapons;
	unsigned int selectedWeapon;
};

#endif /*WEAPONSYSTEM_H_*/
