/*
 * Filename      : models.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Engine/models.h"
#include "Utilities/parser.h"

Models *Models::pInstance = 0; // initialize pointer

Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the sold instance
	}
	return( pInstance );
}

bool Models::Load( string filename ) {
	Parser<Model> parser;
	
	models = parser.Parse( filename, "models", "model" );

	list<Model *>::iterator i;

	for( i = models.begin(); i != models.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

Model *Models::GetModel( string modelName ) {
	list<Model *>::iterator i;

	for( i = models.begin(); i != models.end(); ++i ) {
		if( (*i)->GetName() == modelName )
			return( (*i) );
	}
	
	return( NULL );
}
