/**\file			technologies.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, February 13, 2010
 * \date			Modified: Saturday, February 13, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/technologies.h"
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Utilities/parser.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

bool Technology::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "model" ) {
		Model* model = Models::Instance()->GetModel( value );
		if(model==NULL) {
			Log::Error( "Could Not find the technology '%s'.", value.c_str() );
		} else {
			models.push_back( model );
		}
	} else PPA_MATCHES( "engine" ) {
		Engine* engine = Engines::Instance()->LookUp( value );
		if(engine==NULL) {
			Log::Error( "Could Not find the technology '%s'.", value.c_str() );
		} else {
			engines.push_back( engine );
		}
	} else PPA_MATCHES( "weapon" ) {
		Weapon* weapon = Weapons::Instance()->GetWeapon( value );
		if(weapon==NULL) {
			Log::Error( "Could Not find the technology '%s'.", value.c_str() );
		} else {
			weapons.push_back( weapon );
		}
	}
}

void Technology::_dbg_PrintInfo( void ) {
}

Technologies *Technologies::pInstance = 0;

Technologies *Technologies::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Technologies; // create the sold instance
	}
	return( pInstance );
}

bool Technologies::Load( string& filename ) {
	Parser<Technology> parser;

	technologies = parser.Parse( filename, "technologies", "technology" );

	for( list<Technology*>::iterator i = technologies.begin(); i != technologies.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

Technology* Technologies::GetTechnology( string& techname ) {
	list<Technology*>::iterator iter;
	for( iter=technologies.begin(); iter!=technologies.end(); ++iter ) {
		if( (*iter)->GetName() == techname ){
			return (*iter);
		}
	}
	return NULL;
}
