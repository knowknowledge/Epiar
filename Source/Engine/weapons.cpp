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