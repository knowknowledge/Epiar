/*
 * Filename      : weaponSystem.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, December 11, 2009
 * Last Modified : Friday, December 11, 2009
 * Purpose       : Weapon Systems for ships, manages weapons
 * Notes         :
 */

#include "Engine/weaponSystem.h"
#include "Engine/weapons.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"

WeaponSystem::WeaponSystem() {
	lastFiredAt = 0;
	lastWeaponChangeAt = 0;
	/*Debug: Add weapons to this ship list.*/
	addShipWeapon(string("Laser"));
	addAmmo(string("Laser"), 5);
	selectedWeapon = 0;
}

WeaponSystem::~WeaponSystem(){
}

void WeaponSystem::addShipWeapon(Weapon *i){
	shipWeapons.push_back(i);
}

void WeaponSystem::addShipWeapon(string weaponName){
	Weapons *weapons = Weapons::Instance();
	addShipWeapon(weapons->GetWeapon(weaponName));	
}

void WeaponSystem::removeShipWeapon(int pos){
	shipWeapons.erase(shipWeapons.begin()+pos);
}

void WeaponSystem::addAmmo(string weaponName, int qty){
	Weapons *weapons = Weapons::Instance();
	Weapon* currentWeapon = weapons->GetWeapon(weaponName);
	
	if (ammo.find(currentWeapon->getAmmoType()) == ammo.end() ) {
		ammo.insert ( pair<int,int>(currentWeapon->getAmmoType(),qty) );
	} else {
		ammo.find(currentWeapon->getAmmoType())->second += qty;
	}
	
}

void WeaponSystem::fireWeapon(float angleToFire, Coordinate worldPosition, int offset) {
	if( selectedWeapon<0 || selectedWeapon > shipWeapons.size() ) return;
	Weapon* currentWeapon = shipWeapons.at(selectedWeapon);
	
	if ( currentWeapon->getFireDelay() < (int)(Timer::GetTicks() - lastFiredAt)  &&
	 !shipWeapons.empty() && ammo.find(currentWeapon->getAmmoType())->second > 0) {
		//Calculate the offset needed by the ship to fire infront of the ship
		Trig *trig = Trig::Instance();
		float angle = static_cast<float>(trig->DegToRad( angleToFire ));		
		worldPosition += Coordinate(trig->GetCos( angle ) * offset, -trig->GetSin( angle ) * offset);
		//Fire the weapon
		currentWeapon->fireWeapon(angleToFire, worldPosition);
		//track number of ticks the last fired occured
		lastFiredAt = Timer::GetTicks();
		//reduce ammo
		ammo.find(currentWeapon->getAmmoType())->second -=  currentWeapon->getAmmoConsumption();
		
	}
}

void WeaponSystem::changeWeaponNext() {
	if (250 < Timer::GetTicks() - lastWeaponChangeAt  && selectedWeapon < shipWeapons.size()-1) {
		selectedWeapon++;
		return;
	} 
	selectedWeapon = 0;
}
