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
	return true;
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

bool Technologies::Save( string filename )
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "technologies");
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( list<Technology*>::iterator i = technologies.begin(); i != technologies.end(); ++i ) {
		section = xmlNewNode(NULL, BAD_CAST "technology");
		xmlAddChild(root_node, section);

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST (*i)->GetName().c_str() );
		
		list<Model*> models = (*i)->GetModels();
		for( list<Model*>::iterator it_m = models.begin(); it_m!=models.end(); ++it_m ){
			xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST (*it_m)->GetName().c_str() );
		}

		list<Weapon*> weapons = (*i)->GetWeapons();
		for( list<Weapon*>::iterator it_w = weapons.begin(); it_w!=weapons.end(); ++it_w ){
			xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST (*it_w)->GetName().c_str() );
		}

		list<Engine*> engines = (*i)->GetEngines();
		for( list<Engine*>::iterator it_e = engines.begin(); it_e!=engines.end(); ++it_e ){
			xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST (*it_e)->GetName().c_str() );
		}
	}

	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
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
