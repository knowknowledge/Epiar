/**\file			engines.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/engines.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"

xmlNodePtr Engine::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%1.1f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "forceOutput", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "foldDrive", BAD_CAST (this->GetFoldDrive()?"1":"0") );
	xmlNewChild(section, NULL, BAD_CAST "flareAnimation", BAD_CAST this->GetFlareAnimation().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "thrustSound", BAD_CAST this->thrustsound->GetPath().c_str() );

	return section;
}

/**\class Engines
 * \brief Handles ship engines. */

Engines *Engines::pInstance = 0; // initialize pointer

/**\brief Initializes the instance or gets a pointer to it.
 * \return Pointer to an Engine object
 */
Engines *Engines::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Engines; // create the sold instance
		pInstance->rootName = "engines";
		pInstance->componentName = "engine";
	}
	return( pInstance );
}

