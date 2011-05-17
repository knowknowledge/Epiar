/**\file			alliances.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/alliances.h"
#include "Utilities/log.h"

/**\class Alliance
 * \brief Government/Faction.
 */

/**\brief Initializes alliance class using default values.
 * \details
 * - Attack Size = 0
 * - Aggressiveness = 0
 * - Current = "Credits"
 */
Alliance::Alliance() : attackSize(0),aggressiveness(0.0),currency("Credits")
{
	SetName("dead");
	color = WHITE;
}

/**\brief Assignment constructor, copies field values.
 */
Alliance& Alliance::operator= (const Alliance& other){
	name = other.name;
    attackSize = other.attackSize;
    aggressiveness = other.aggressiveness;
    currency = other.currency;
	color = other.color;
    return *this;
}

/**\brief Initializes alliance class using given values.
 * \param _name Name of the alliance.
 * \param _attackSize Size of the fleet
 * \param _aggressiveness Aggressiveness
 * \param _currency The kind of currency used by this Alliance.
 * \param _color The flag color for this Alliance.
 */
Alliance::Alliance( string _name, short int _attackSize, float _aggressiveness, string _currency, Color _color) :
    attackSize(_attackSize),
    aggressiveness(_aggressiveness),
    currency(_currency),
	color(_color)
{
    SetName(_name);
}

/**\brief Parser to parse the XML file.
 */
bool Alliance::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"aggressiveness")) ){
		value = NodeToString(doc,attr);
		aggressiveness = static_cast<float>(atof( value.c_str() ) / 10.);
	} else return false;

	if( (attr = FirstChildNamed(node,"attackSize")) ) {
		value = NodeToString(doc,attr);
		attackSize = (short int)atof( value.c_str() );
	} else return false;

	if( (attr = FirstChildNamed(node,"currency")) ) {
		value = NodeToString(doc,attr);
		currency = value;
	} else return false;

	if( (attr = FirstChildNamed(node,"color")) ) {
		value = NodeToString(doc,attr);
		color = Color(value);
	} else return false;
	
	return true;
}

/**\brief Converts the Alliance object to an XML node.
 */
xmlNodePtr Alliance::ToXMLNode(string componentName){
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%1.1f", this->GetAggressiveness()*10 );
	xmlNewChild(section, NULL, BAD_CAST "aggressiveness", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetAttackSize() );
	xmlNewChild(section, NULL, BAD_CAST "attackSize", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "currency", BAD_CAST this->GetCurrency().c_str() );
	snprintf(buff, sizeof(buff), "0x%02X%02X%02X", int(0xFF*color.r), int(0xFF*color.g), int(0xFF*color.b) );
	xmlNewChild(section, NULL, BAD_CAST "color", BAD_CAST buff );

	return section;
}
/**\fn Alliance::GetAttackSize()
 * \brief Returns the size of the fleet.
 */

/**\fn Alliance::GetAggressiveness()
 * \brief Returns the aggressiveness attribute.
 */

/**\fn Alliance::GetCurrency()
 * \brief Returns the type of currency used.
 */

/**\fn Alliance::GetColor()
 * \brief Returns the banner color of this Alliance.
 */


/**\class Alliances
 * \brief Collection of Alliance objects.
 */
Alliances *Alliances::pInstance = 0; // initialize pointer

/**\brief Initializes a new instance or gets the current instance.
 * \return Pointer to an Alliances object
 */
Alliances *Alliances::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Alliances; // create the solid instance
		pInstance->rootName = "alliances";
		pInstance->componentName = "alliance";
		// The Independent Alliance is a reasonable used by players.
		pInstance->Add( new Alliance( "Independent", 0, 0, "Credits", GREY ) );
	}
	return( pInstance );
}

/**\fn Alliances::GetAlliance( string name )
 * \brief Returns a pointer to the named Alliance.
 */

/**\fn Alliances::newComponent()
 * \brief Creates a new Alliance object.
 */

