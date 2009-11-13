/*
 * Filename      : alliances.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Engine/alliances.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"

Alliances *Alliances::pInstance = 0; // initialize pointer

Alliances *Alliances::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Alliances; // create the sold instance
	}
	return( pInstance );
}

bool Alliances::Load( string& filename )
{
	Parser<Alliance> parser;
	
	alliances = parser.Parse( filename, "alliances", "alliance" );

	return true;
}
