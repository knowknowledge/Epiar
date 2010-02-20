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

/**\brief Initializes a new instance or gets the current instance.
 * \return Pointer to an Alliance object
 */
Alliances *Alliances::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Alliances; // create the sold instance
	}
	return( pInstance );
}

/**\brief Parses an Alliance XML file
 * \return true always
 * \sa Parser
 */
bool Alliances::Load( string& filename )
{
	Parser<Alliance> parser;
	
	alliances = parser.Parse( filename, "alliances", "alliance" );

	return true;
}

bool Alliances::Save( string filename )
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */
	char buff[256];

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "alliances");
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( list<Alliance*>::iterator i = alliances.begin(); i != alliances.end(); ++i ) {
		section = xmlNewNode(NULL, BAD_CAST "alliance");
		xmlAddChild(root_node, section);

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST (*i)->GetName().c_str() );

        snprintf(buff, sizeof(buff), "%1.1f", (*i)->GetAggressiveness()*10 );
		xmlNewChild(section, NULL, BAD_CAST "aggressiveness", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetAttackSize() );
		xmlNewChild(section, NULL, BAD_CAST "attackSize", BAD_CAST buff );
		xmlNewChild(section, NULL, BAD_CAST "currency", BAD_CAST (*i)->GetCurrency().c_str() );

		list<string> illegals = (*i)->GetIlligalCargos();
		for( list<string>::iterator it = illegals.begin(); it!=illegals.end(); ++it ){
			xmlNewChild(section, NULL, BAD_CAST "illegalCargo", BAD_CAST (*it).c_str() );
		}
	}

	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
	return true;
}

