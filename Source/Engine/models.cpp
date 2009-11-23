/**\file			models.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/models.h"
#include "Utilities/parser.h"

/**\class Models
 * \brief Handles ship models. */

struct model_name_equals
	: public std::binary_function<Model*, string, bool>
{
	bool operator()( Model* model, const string& name ) const
	{
		return model->GetName() == name;		
	}
};

Models *Models::pInstance = 0; // initialize pointer

Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the sold instance
	}
	return( pInstance );
}

bool Models::Load( string& filename )
{
	Parser<Model> parser;
	
	models = parser.Parse( filename, "models", "model" );

	for( list<Model *>::iterator i = models.begin(); i != models.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

Model * Models::GetModel( string& modelName )
{
	list<Model *>::iterator i = std::find_if( models.begin(), models.end(), std::bind2nd( model_name_equals(), modelName ));
	if( i != models.end() ) {
		return( *i );
	}
	
	return( NULL );
}

list<string>* Models::GetModelNames()
{
	list<string> *names = new list<string>();
	for( list<Model *>::iterator i = models.begin(); i != models.end(); ++i ) {
		names->push_back( (*i)->GetName() );
	}
	return names;
}
