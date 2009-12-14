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
	/*Debug: Add all weapons to this ships weapons list, later we will only basic and when they are purchasedhi*/
	Weapons* weapons = Weapons::Instance();
	for( list<Weapon *>::iterator i = weapons->weapons.begin(); i != weapons->weapons.end(); ++i ) {
		addShipWeapon(*i);
	}
	selectedWeapon = 0;
	
}
WeaponSystem::~WeaponSystem(){

}
void WeaponSystem::addShipWeapon(Weapon *i){
	shipWeapons.push_back(i);
	//Debug: add 100 rounds of ammo for every weapon added
	ammo.insert ( pair<int,int>(i->getAmmoType(),20) );
}

void WeaponSystem::removeShipWeapon(int pos){
	shipWeapons.erase(shipWeapons.begin()+pos);
	
}
//TODO: better ammo system
void WeaponSystem::addAmmo(int qty){
	//projectileAmmo += qty;
}

void WeaponSystem::fireWeapon(float angleToFire, Coordinate worldPosition, int offset) {
	Weapon* currentWeapon = shipWeapons.at(selectedWeapon);
	
	if ( currentWeapon->getFireDelay() < (int)(Timer::GetTicks() - lastFiredAt)  && !shipWeapons.empty() && ammo.find(currentWeapon->getAmmoType())->second > 0) {
		//Calculate the offset needed by the ship to fire infront of the ship
		Trig *trig = Trig::Instance();
		float angle = static_cast<float>(trig->DegToRad( angleToFire ));		
		worldPosition += Coordinate(trig->GetCos( angle ) * offset, -trig->GetSin( angle ) * offset);
		//Fire the weapon
		currentWeapon->fireWeapon(angleToFire, worldPosition);
		//track number of ticks the last fired occured
		lastFiredAt = Timer::GetTicks();
		//reduce ammo
		ammo.find(currentWeapon->getAmmoType())->second = ammo.find(currentWeapon->getAmmoType())->second - 1;
		
	}
}

void WeaponSystem::changeWeaponNext() {
	if (200 < Timer::GetTicks() - lastWeaponChangeAt  && selectedWeapon < shipWeapons.size()-1) {
		selectedWeapon++;
		return;
	} 
	selectedWeapon = 0;
}