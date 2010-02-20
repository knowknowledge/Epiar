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

/**\class Engines
 * \brief Handles ship engines. */

Engines *Engines::pInstance = 0; // initialize pointer

/**\brief Initializes the instance or gets a pointer to it.
 * \return Pointer to an Engine object
 */
Engines *Engines::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Engines; // create the sold instance
	}
	return( pInstance );
}

/**\brief Loads the engine from an XML file.
 * \return true always
 */
bool Engines::Load( string filename ) {
	Parser<Engine> parser;
	
	engines = parser.Parse( filename, "engines", "engine" );

	for( list<Engine *>::iterator i = engines.begin(); i != engines.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

bool Engines::Save( string filename )
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */
	char buff[256];

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "engines");
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( list<Engine*>::iterator i = engines.begin(); i != engines.end(); ++i ) {
		section = xmlNewNode(NULL, BAD_CAST "engine");
		xmlAddChild(root_node, section);

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST (*i)->GetName().c_str() );

        snprintf(buff, sizeof(buff), "%1.1f", (*i)->GetForceOutput() );
		xmlNewChild(section, NULL, BAD_CAST "forceOutput", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetMSRP() );
		xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
		xmlNewChild(section, NULL, BAD_CAST "foldDrive", BAD_CAST ((*i)->GetFoldDrive()?"1":"0") );
		xmlNewChild(section, NULL, BAD_CAST "flareAnimation", BAD_CAST (*i)->GetFlareAnimation().c_str() );
		xmlNewChild(section, NULL, BAD_CAST "thrustSound", BAD_CAST (*i)->thrustsound->GetPath().c_str() );
	}

	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
	return true;
}


/**\brief Lookup engine name.
 * \return Pointer to an Engine object.
 */
Engine *Engines::LookUp( string engineName ) {
	for( list<Engine *>::iterator i = engines.begin(); i != engines.end(); ++i ) {
		if( (*i)->IsNamed( engineName ) )
			return (*i);
	}
	
	return( NULL );
}
