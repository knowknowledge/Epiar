/**\file			weapons.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/weapon.h"
#include "Utilities/trig.h"
#include "Engine/models.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

/**\brief Empty constructor
 */
Weapon::Weapon(void)
{
}
/**\brief Empty destructor
 */
Weapon::~Weapon(void)
{
}

/**\brief Parses weapon information
 */
bool Weapon::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "weaponType" ) {
		weaponType = atoi( value.c_str() );
	} else PPA_MATCHES( "imageName" ) {
		image = Image::Get(value);
	} else PPA_MATCHES( "picName" ) {
		pic = Image::Get(value);
		// This can be accessed by either the path or the modelName
		Image::Store(name, pic);
	} else PPA_MATCHES( "payload" ) {
		if (atoi( value.c_str()) != 0)
			payload = atoi( value.c_str() );
	} else PPA_MATCHES( "velocity" ) {
		if (atoi( value.c_str()) != 0)
			velocity = atoi( value.c_str() );
	} else PPA_MATCHES( "acceleration" ) {
		if (atoi( value.c_str()) != 0)
			acceleration = atoi( value.c_str() );
	} else PPA_MATCHES( "ammoType" ) {
		if (atoi( value.c_str()) != 0)
			ammoType = atoi( value.c_str() );
	} else PPA_MATCHES( "ammoConsumption" ) {
		ammoConsumption = atoi( value.c_str() );
	} else PPA_MATCHES( "fireDelay" ) {
		if (atoi( value.c_str()) != 0)
			fireDelay = atoi( value.c_str() );
	} else PPA_MATCHES( "lifetime" ) {
		if (atoi( value.c_str()) != 0)
			lifetime = atoi( value.c_str() );
	} else PPA_MATCHES( "sound" ) {
			string pathPrefix = "Resources/Audio/Weapons/";
			if( value.find(pathPrefix)==0 ) {
				this->sound = Sound::Get( value );
			} else {
				this->sound = Sound::Get( value.insert(0,pathPrefix) );
			}
	}
	//	SetRadarColor(Color::Get(255, 0, 0));
	return true;
}


void Weapon::_dbg_PrintInfo( void ) {
	cout << "Weapon: " << name << endl;
}
