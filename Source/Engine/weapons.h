/*
 * Filename      : weapons.h
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : 
 * Notes         :
 */

#ifndef __H_WEAPONS__
#define __H_WEAPONS__

//#include "Sprites/sprite.h"
#include "Engine/weapon.h"
#include "Utilities/components.h"

class Weapons : public Components {
public:
	static Weapons *Instance();

	Weapon *GetWeapon( string& weaponName ) { return (Weapon*) this->Get(weaponName); }
	Component* newComponent() { return new Weapon(); }
protected:
	Weapons(){};
	Weapons( const Weapons & );
	Weapons& operator= (const Weapons&);
private:
	static Weapons *pInstance;
};

#endif
