/**\file			engines.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/engines.h"
#include "Utilities/parser.h"

/**\class Engines
 * \brief Handles ship engines. */

Engines *Engines::pInstance = 0; // initialize pointer

Engines *Engines::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Engines; // create the sold instance
	}
	return( pInstance );
}

bool Engines::Load( string filename ) {
	Parser<Engine> parser;
	
	engines = parser.Parse( filename, "engines", "engine" );

	for( list<Engine *>::iterator i = engines.begin(); i != engines.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

Engine *Engines::LookUp( string engineName ) {
	for( list<Engine *>::iterator i = engines.begin(); i != engines.end(); ++i ) {
		if( (*i)->IsNamed( engineName ) )
			return (*i);
	}
	
	return( NULL );
}
