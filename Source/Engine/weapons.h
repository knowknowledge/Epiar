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

class Weapons {
public:
	
	~Weapons(void);
	static Weapons *Instance();
	bool Load( string filename );
	bool Save( string filename );

	Weapon *GetWeapon( string& weaponName );
	list<string>* GetWeaponNames();
protected:
	Weapons(){};
private:
	string filename;
	list<Weapon *> weapons;
	
	static Weapons *pInstance;
};

#endif
