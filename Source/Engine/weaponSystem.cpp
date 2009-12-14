/*
 * Filename      : weaponSystem.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, December 11, 2009
 * Last Modified : Friday, December 11, 2009
 * Purpose       : Weapon Systems for ships, manages weapons
 * Notes         :
 */

#include "Sprites/spritemanager.h"
#include "Engine/weaponSystem.h"
#include "Engine/weapons.h"
#include "Sprites/projectile.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"

WeaponSystem::WeaponSystem() {
	lastFiredAt = 0;
	lastWeaponChangeAt = 0;
	/*Debug: Add all weapons to this ship list.*/
	addShipWeapon(string("Missile"));
	addShipWeapon(string("Slow Missile"));
	addShipWeapon(string("Laser"));
	addShipWeapon(string("Strong Laser"));
	addShipWeapon(string("Minigun"));
	addAmmo(string("Missile"), 20);
	selectedWeapon = 0;
}

WeaponSystem::~WeaponSystem(){
}

void WeaponSystem::addShipWeapon(Weapon *i){
	shipWeapons.push_back(i);
	//Debug: add 100 rounds of ammo for every weapon added
	ammo.insert ( pair<int,int>(i->GetAmmoType(),5) );
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
	
	if (ammo.find(currentWeapon->GetAmmoType()) == ammo.end() ) {
		ammo.insert ( pair<int,int>(currentWeapon->GetAmmoType(),qty) );
	} else {
		ammo.find(currentWeapon->GetAmmoType())->second += qty;
	}
	
}

Projectile* WeaponSystem::fireWeapon(float angleToFire, Coordinate worldPosition, int offset) {
	if( selectedWeapon<0 || selectedWeapon > shipWeapons.size() )
		return (Projectile*)NULL;

	Weapon* currentWeapon = shipWeapons.at(selectedWeapon);
	
	if ( currentWeapon->GetFireDelay() < (int)(Timer::GetTicks() - lastFiredAt)  &&
	 !shipWeapons.empty() && ammo.find(currentWeapon->GetAmmoType())->second > 0) {
		//Calculate the offset needed by the ship to fire infront of the ship
		Trig *trig = Trig::Instance();
		float angle = static_cast<float>(trig->DegToRad( angleToFire ));		
		worldPosition += Coordinate(trig->GetCos( angle ) * offset, -trig->GetSin( angle ) * offset);

		//Fire the weapon
		SpriteManager *sprites = SpriteManager::Instance();
		Projectile *projectile = new Projectile(angleToFire, worldPosition, currentWeapon);
		sprites->Add( (Sprite*)projectile );

		//track number of ticks the last fired occured
		lastFiredAt = Timer::GetTicks();
		//reduce ammo
		ammo.find(currentWeapon->GetAmmoType())->second -=  currentWeapon->GetAmmoConsumption();

		return projectile;
	} else {
		return (Projectile*)NULL;
		
	}
}

string WeaponSystem::changeWeaponNext() {
	if (250 < Timer::GetTicks() - lastWeaponChangeAt) {
		selectedWeapon = (selectedWeapon+1)%shipWeapons.size();
	} 
	return shipWeapons.at(selectedWeapon)->GetName();
}

Weapon* WeaponSystem::currentWeapon() {
	return shipWeapons.at(selectedWeapon);
}

int WeaponSystem::currentAmmo() {
	Weapon* currentWeapon = shipWeapons.at(selectedWeapon);
	return ammo.find(currentWeapon->GetAmmoType())->second;
}

