/**\file			weapons.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief
 * \details
 */

#include "includes.h"
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

/**\brief Gets or returns Weapons instance
 * \return Pointer to a Weapons object
 */
Weapons *Weapons::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Weapons; // create the sold instance
	}
	return( pInstance );
}

/**\brief Empty destructor
 */
Weapons::~Weapons(void)
{
}

/**\brief Loads XML Weapons file and parses it
 * \param filename String containing file name
 * \return true always
 */
bool Weapons::Load( string filename ) {
	Parser<Weapon> parser;

	weapons = parser.Parse( filename, "weapons", "weapon" );

	//for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		//(*i)->_dbg_PrintInfo();
	//}
	return true;
}

/**\brief Returns Weapon based on its name
 * \param weaponName string of the weapon name
 */
Weapon * Weapons::GetWeapon( string& weaponName )
{
	list<Weapon *>::iterator i = std::find_if( weapons.begin(), weapons.end(), std::bind2nd( weapon_name_equals(), weaponName ));
	if( i != weapons.end() ) {
		return( *i );
	}
	
	return( NULL );
}

/**\brief Gets a list of the weapon names.
 * \return std::list of strings containing weapon names
 */
list<string>* Weapons::GetWeaponNames()
{
	list<string> *names = new list<string>();
	for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		names->push_back( (*i)->GetName() );
	}
	return names;
}
