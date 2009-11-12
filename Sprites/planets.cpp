/*
 * Filename      : planets.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Sprites/planets.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"

Planets *Planets::pInstance = 0; // initialize pointer

Planets *Planets::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Planets; // create the sold instance
	}
	return( pInstance );
}

bool Planets::Load( string filename ) {
	Parser<cPlanet> parser;
	
	planets = parser.Parse( filename, "planets", "planet" );

	for( list<cPlanet *>::iterator i = planets.begin(); i != planets.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

// Adds all planets in the manager (this, Planets) to the spritelist
void Planets::RegisterAll( SpriteManager *sprites ) {
	if( !sprites ) {
		Log::Warning( "Invalid spritelist passed to planets manager." );
		
		return;
	}

	for( list<cPlanet *>::iterator i = planets.begin(); i != planets.end(); ++i ) {
		sprites->Add( (*i) );
	}	
}
