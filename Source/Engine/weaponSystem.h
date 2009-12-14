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

class WeaponSystem {
	public:
	WeaponSystem(void);
	~WeaponSystem(void);
	void addShipWeapon(Weapon *i);
	void addShipWeapon(string weaponName);
	void removeShipWeapon(int pos);
	void addAmmo(int qty);
	void fireWeapon(float angleToFire, Coordinate worldPosition, int offset);
	void changeWeaponNext(void);
	
	private:
	unsigned int lastWeaponChangeAt; //number of where last weapon change occcured
	unsigned int lastFiredAt; //number of ticks where last fire event occured
	vector<Weapon *> shipWeapons;
	int projectileAmmo;
	unsigned int selectedWeapon;
};

#endif /*WEAPONSYSTEM_H_*/
