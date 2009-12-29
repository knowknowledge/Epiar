/*
 * Filename      : weapons.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : 
 * Notes         :
 */
#include "Engine/weapons.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"

struct weapon_name_equals
	: public std::binary_function<Weapon*, string, bool>
{
	bool operator()( Weapon* weapon, const string& name ) const
	{
		return weapon->GetName() == name;
	}
};

Weapons *Weapons::pInstance = 0; // initialize pointer

Weapons *Weapons::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Weapons; // create the sold instance
	}
	return( pInstance );
}

Weapons::~Weapons(void)
{
}


bool Weapons::Load( string filename ) {
	Parser<Weapon> parser;

	weapons = parser.Parse( filename, "weapons", "weapon" );

	//for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		//(*i)->_dbg_PrintInfo();
	//}
	return true;
}

Weapon * Weapons::GetWeapon( string& weaponName )
{
	list<Weapon *>::iterator i = std::find_if( weapons.begin(), weapons.end(), std::bind2nd( weapon_name_equals(), weaponName ));
	if( i != weapons.end() ) {
		return( *i );
	}
	
	return( NULL );
}

list<string>* Weapons::GetWeaponNames()
{
	list<string> *names = new list<string>();
	for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		names->push_back( (*i)->GetName() );
	}
	return names;
}
