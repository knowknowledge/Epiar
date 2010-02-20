/**\file			weapons.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/weapons.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"

struct weapon_name_equals
	: public std::binary_function<Weapon*, string, bool>
{
	bool operator()( Weapon* weapon, const string& name ) const
	{
		return weapon->GetName() == name;
	}
};

Weapons *Weapons::pInstance = 0; // initialize pointer

/**\brief Gets or returns Weapons instance
 * \return Pointer to a Weapons object
 */
Weapons *Weapons::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Weapons; // create the sold instance
	}
	return( pInstance );
}

/**\brief Empty destructor
 */
Weapons::~Weapons(void)
{
}

/**\brief Loads XML Weapons file and parses it
 * \param filename String containing file name
 * \return true always
 */
bool Weapons::Load( string filename ) {
	Parser<Weapon> parser;

	weapons = parser.Parse( filename, "weapons", "weapon" );

	//for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		//(*i)->_dbg_PrintInfo();
	//}
	return true;
}

bool Weapons::Save( string filename )
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */
    char buff[256];

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "weapons");
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( list<Weapon*>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		section = xmlNewNode(NULL, BAD_CAST "weapon");
		xmlAddChild(root_node, section);

		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST (*i)->GetName().c_str() );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetType() );
		xmlNewChild(section, NULL, BAD_CAST "weaponType", BAD_CAST buff );

		xmlNewChild(section, NULL, BAD_CAST "imageName", BAD_CAST (*i)->GetImage()->GetPath().c_str() );
		xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST (*i)->GetPicture()->GetPath().c_str() );

        snprintf(buff, sizeof(buff), "%d", (*i)->GetPayload() );
		xmlNewChild(section, NULL, BAD_CAST "payload", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetVelocity() );
		xmlNewChild(section, NULL, BAD_CAST "velocity", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetAcceleration() );
		xmlNewChild(section, NULL, BAD_CAST "acceleration", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetAmmoType() );
		xmlNewChild(section, NULL, BAD_CAST "ammoType", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetAmmoConsumption() );
		xmlNewChild(section, NULL, BAD_CAST "ammoConsumption", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetFireDelay() );
		xmlNewChild(section, NULL, BAD_CAST "fireDelay", BAD_CAST buff );
        snprintf(buff, sizeof(buff), "%d", (*i)->GetLifetime() );
		xmlNewChild(section, NULL, BAD_CAST "lifetime", BAD_CAST buff );
		xmlNewChild(section, NULL, BAD_CAST "sound", BAD_CAST (*i)->sound->GetPath().c_str() );
	}

	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
	return true;
}


/**\brief Returns Weapon based on its name
 * \param weaponName string of the weapon name
 */
Weapon * Weapons::GetWeapon( string& weaponName )
{
	list<Weapon *>::iterator i = std::find_if( weapons.begin(), weapons.end(), std::bind2nd( weapon_name_equals(), weaponName ));
	if( i != weapons.end() ) {
		return( *i );
	}
	
	return( NULL );
}

/**\brief Gets a list of the weapon names.
 * \return std::list of strings containing weapon names
 */
list<string>* Weapons::GetWeaponNames()
{
	list<string> *names = new list<string>();
	for( list<Weapon *>::iterator i = weapons.begin(); i != weapons.end(); ++i ) {
		names->push_back( (*i)->GetName() );
	}
	return names;
}
