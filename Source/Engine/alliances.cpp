/**\file			alliances.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

/**\class Alliances
 * \brief Government/Faction. */

#include "includes.h"
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
