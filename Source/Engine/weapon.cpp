/*
 * Filename      : weapons.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : 
 * Notes         :
 */
#include "Engine/weapon.h"
#include "Sprites/projectile.h"
#include "Utilities/trig.h"
#include "Engine/models.h"
#include "Sprites/spritemanager.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

Weapon::Weapon(void)
{
}
Weapon::~Weapon(void)
{
}

bool Weapon::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "weaponType" ) {
		if (atoi( value.c_str()) != 0)
			weaponType = atoi( value.c_str() );
	} else PPA_MATCHES( "imageName" ) {
		image = new Image(value);
	} else PPA_MATCHES( "payload" ) {
		if (atoi( value.c_str()) != 0)
			payload = atoi( value.c_str() );
	} else PPA_MATCHES( "velocity" ) {
		if (atoi( value.c_str()) != 0)
			velocity = atoi( value.c_str() );
	} else PPA_MATCHES( "acceleration" ) {
		if (atoi( value.c_str()) != 0)
			acceleration = atoi( value.c_str() );
	} else PPA_MATCHES( "ammoType" ) {
		if (atoi( value.c_str()) != 0)
			ammoType = atoi( value.c_str() );
	} else PPA_MATCHES( "ammoConsumption" ) {
		if (atoi( value.c_str()) != 0)
			ammoConsumption = atoi( value.c_str() );
	} else PPA_MATCHES( "fireDelay" ) {
		if (atoi( value.c_str()) != 0)
			fireDelay = atoi( value.c_str() );
	} else PPA_MATCHES( "lifetime" ) {
		if (atoi( value.c_str()) != 0)
			lifetime = atoi( value.c_str() );
	}
	//	SetRadarColor(Color::Get(255, 0, 0));
	return true;
}


void Weapon::_dbg_PrintInfo( void ) {
	cout << "Weapon: " << name << endl;
}

void Weapon::fireWeapon(float angleToFire, Coordinate worldPosition) {
	//todo check weapon type and generate correct class based on type
	//if (weaponType == WEAPON_PROJECTILE) {
	if (ammo > 0) {
		SpriteManager *sprites = SpriteManager::Instance();
		Projectile *projectile = new Projectile(angleToFire,worldPosition, image, lifetime, velocity);
		sprites->Add( (Sprite*)projectile );
		ammo--;
		cout << "ammo: " << ammo << endl;
	}
	//}

}
