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
#include "Utilities/components.h"

Technology::Technology(){}

Technology& Technology::operator= (const Technology& other)
{
	name = other.name;
	models = other.models;
	engines = other.engines;
	weapons = other.weapons;
	return *this;
}

Technology::Technology( string _name, list<Model*> _models, list<Engine*>_engines, list<Weapon*>_weapons) :
	models (_models ),
	engines(_engines),
	weapons(_weapons)
{
	SetName(_name);
}

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
		Engine* engine = Engines::Instance()->GetEngine( value );
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

xmlNodePtr Technology::ToXMLNode(string componentName) {
		xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
		
		list<Model*> models = this->GetModels();
		for( list<Model*>::iterator it_m = models.begin(); it_m!=models.end(); ++it_m ){
			xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST (*it_m)->GetName().c_str() );
		}

		list<Weapon*> weapons = this->GetWeapons();
		for( list<Weapon*>::iterator it_w = weapons.begin(); it_w!=weapons.end(); ++it_w ){
			xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST (*it_w)->GetName().c_str() );
		}

		list<Engine*> engines = this->GetEngines();
		for( list<Engine*>::iterator it_e = engines.begin(); it_e!=engines.end(); ++it_e ){
			xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST (*it_e)->GetName().c_str() );
		}

	return section;
}

Technologies *Technologies::pInstance = 0;

Technologies *Technologies::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Technologies; // create the sold instance
		pInstance->rootName = "technologies";
		pInstance->componentName = "technology";
	}
	return( pInstance );
}

