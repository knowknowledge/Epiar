/*
 * Filename      : engines.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Engine/engines.h"
#include "Utilities/parser.h"

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
