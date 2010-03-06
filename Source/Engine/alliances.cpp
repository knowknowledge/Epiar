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

bool Alliance::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "aggressiveness" ) {
		aggressiveness = static_cast<float>(atof( value.c_str() ) / 10.);
	} else PPA_MATCHES( "attackSize" ) {
		attackSize = (short int)atof( value.c_str() );
	} else PPA_MATCHES( "currency" ) {
		currency = value;
	} else PPA_MATCHES( "illegalCargo" ) {
		illegalCargos.push_back( value );
	}
	
	return true;
}

xmlNodePtr Alliance::ToXMLNode(string componentName){
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%1.1f", this->GetAggressiveness()*10 );
	xmlNewChild(section, NULL, BAD_CAST "aggressiveness", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetAttackSize() );
	xmlNewChild(section, NULL, BAD_CAST "attackSize", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "currency", BAD_CAST this->GetCurrency().c_str() );

	list<string> illegals = this->GetIlligalCargos();
	for( list<string>::iterator it = illegals.begin(); it!=illegals.end(); ++it ){
		xmlNewChild(section, NULL, BAD_CAST "illegalCargo", BAD_CAST (*it).c_str() );
	}
	return section;
}

Alliances *Alliances::pInstance = 0; // initialize pointer

/**\brief Initializes a new instance or gets the current instance.
 * \return Pointer to an Alliance object
 */
Alliances *Alliances::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Alliances; // create the sold instance
		pInstance->rootName = "alliances";
		pInstance->componentName = "alliance";
	}
	return( pInstance );
}

