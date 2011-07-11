/**\file			commodities.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Wednesday, April 21, 2010
 * \date			Modified:
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/components.h"
#include "Engine/commodities.h"

/**\class Commodity
 * \brief Fungible items that can be traded between Planets by the ton.
 * \note Fungible: a commodity that is freely interchangeable with another in satisfying an obligation.
 */

/**\brief Initializes Commodity
 */
Commodity::Commodity()
	:msrp(0)
{
	SetName("");
}

/**\brief Initializes Commodity using another Commodity
 */
Commodity& Commodity::operator=(const Commodity& other){
	name = other.name;
	msrp = other.msrp;
	return *this;
}

/**\brief Initializes Commodity from scratch
 */
Commodity::Commodity( string _name, int _msrp )
	:msrp(_msrp)
{
	SetName(_name);
}

/**\brief Parser to parse the XML file.
 */
bool Commodity::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		msrp = atoi(NodeToString(doc,attr).c_str());
	} else return false;

	return true;
}

/**\brief Converts the Alliance object to an XML node.
 */
xmlNodePtr Commodity::ToXMLNode(string componentName){
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );

	return section;
}

/**\class Commodities
 * \brief Collection of Commodity objects.
 */
Commodities *Commodities::pInstance = 0; // initialize pointer

/**\brief Initializes a new instance or gets the current instance.
 * \return Pointer to an Commodities object
 */
Commodities *Commodities::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Commodities; // create the solid instance
		pInstance->rootName = "commodities";
		pInstance->componentName = "commodity";
	}
	return( pInstance );
}
