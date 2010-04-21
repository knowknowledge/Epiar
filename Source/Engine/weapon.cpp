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

/**\class Weapon
 *
 * \see Weapons
 */

/**\brief Empty constructor
 */
Weapon::Weapon(void) :
	sound(NULL),
	image(NULL),
	pic(NULL),
	weaponType(0),
	payload(0),
	velocity(0),
	acceleration(0),
	ammoType(energy_ammo),
	ammoConsumption(0),
	fireDelay(0),
	lifetime(0),
	tracking(0.0f),
	msrp(0)
{
	SetName("dead");
}

/**\brief Assignment operator (Copy fields)
 */
Weapon& Weapon::operator=(const Weapon& other) {
	name = other.name;
	image = other.image;
	pic = other.pic;
	weaponType = other.weaponType;
	payload = other.payload;
	velocity = other.velocity;
	acceleration = other.acceleration;
	ammoType = other.ammoType;
	ammoConsumption = other.ammoConsumption;
	fireDelay = other.fireDelay;
	lifetime = other.lifetime;
	tracking = other.tracking;
	msrp = other.msrp;
	sound = other.sound;
	return *this;
}

/**\brief Creates a new Weapon object given parameters.
 * \param _name Name of the weapon
 * \param _image Picture of the weapon as fired
 * \param _pic Picture of the weapon in shop
 * \param _weaponType Type of weapon
 * \param _payload Amount of damage
 * \param _velocity Traveling velocity
 * \param _acceleration Acceleration of the weapon
 * \param _ammoType Type of ammo required
 * \param _ammoConsumption Amount of ammo consumed per shot
 * \param _fireDelay Delay after firing
 * \param _lifetime Life the ammo
 * \param _sound Sound the weapon makes
 * \param _tracking Turning rate
 * \param _msrp Price of the weapon
 */
Weapon::Weapon( string _name, Image* _image, Image* _pic,
		int _weaponType, int _payload, int _velocity, int _acceleration,
		AmmoType _ammoType, int _ammoConsumption, int _fireDelay,  int _lifetime, Sound* _sound, float _tracking, int _msrp) :
	sound(_sound),
	image(_image),
	pic(_pic),
	weaponType(_weaponType),
	payload(_payload),
	velocity(_velocity),
	acceleration(_acceleration),
	ammoType(_ammoType),
	ammoConsumption(_ammoConsumption),
	fireDelay(_fireDelay),
	lifetime(_lifetime),
	tracking(_tracking),
	msrp(_msrp)
{
	SetName(_name);
	//((Component*)this)->SetName(_name);
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
		ammoType = AmmoNameToType(value);
		if(ammoType>=max_ammo) {
			// This is an Error!
		}
	} else PPA_MATCHES( "ammoConsumption" ) {
		ammoConsumption = atoi( value.c_str() );
	} else PPA_MATCHES( "fireDelay" ) {
		if (atoi( value.c_str()) != 0)
			fireDelay = atoi( value.c_str() );
	} else PPA_MATCHES( "lifetime" ) {
		if (atoi( value.c_str()) != 0)
			lifetime = atoi( value.c_str() );
	} else PPA_MATCHES( "tracking" ) {
		float _tracking = atof( value.c_str());
		if (_tracking > 1.0f ) _tracking = 1.0f;
		if (_tracking < 0.0001f ) _tracking = 0.0f;
		tracking = _tracking;
	} else PPA_MATCHES( "msrp" ) {
		if (atoi( value.c_str()) != 0)
			msrp = atoi( value.c_str() );
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

/** \brief Converts the Weapon object to an XML node.
 */
xmlNodePtr Weapon::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetType() );
	xmlNewChild(section, NULL, BAD_CAST "weaponType", BAD_CAST buff );

	xmlNewChild(section, NULL, BAD_CAST "imageName", BAD_CAST this->GetImage()->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST this->GetPicture()->GetPath().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetPayload() );
	xmlNewChild(section, NULL, BAD_CAST "payload", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetVelocity() );
	xmlNewChild(section, NULL, BAD_CAST "velocity", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetAcceleration() );
	xmlNewChild(section, NULL, BAD_CAST "acceleration", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "ammoType", BAD_CAST AmmoTypeToName(this->GetAmmoType()).c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetAmmoConsumption() );
	xmlNewChild(section, NULL, BAD_CAST "ammoConsumption", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetFireDelay() );
	xmlNewChild(section, NULL, BAD_CAST "fireDelay", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetLifetime() );
	xmlNewChild(section, NULL, BAD_CAST "lifetime", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%.4f", this->GetTracking() );
	xmlNewChild(section, NULL, BAD_CAST "tracking", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "sound", BAD_CAST this->sound->GetPath().c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );

	return section;
}

/**\brief Prints debugging information (not implemented)
 */
void Weapon::_dbg_PrintInfo( void ) {
	cout << "Weapon: " << name << endl;
}

/**\fn Weapon::GetImage( )
 *  \brief Returns the image of the fired weapon
 * \fn Weapon::GetPicture( )
 *  \brief Returns the image of the weapon as seen in ship
 * \fn Weapon::GetType( )
 *  \brief Retrieves the type of Weapon
 * \fn Weapon::GetPayload( )
 *  \brief Retrieves the payload
 * \fn Weapon::GetVelocity( )
 *  \brief Retrieves the velocity
 * \fn Weapon::GetAcceleration( )
 *  \brief Retrieves the acceleration
 * \fn Weapon::GetAmmoType( )
 *  \brief Retrieves the ammo type
 * \fn Weapon::GetAmmoConsumption( )
 *  \brief Retrieves the ammo consumption
 * \fn Weapon::GetFireDelay( )
 *  \brief Retrieves the firing delay
 * \fn Weapon::GetMSRP( )
 *  \brief Retrieves the price
 * \fn Weapon::GetLifetime( )
 *  \brief Retrieves the ammo life time
 * \var Weapon::sound
 *  \brief Pointer to the Sound object
 */

const string AmmoNames[] = {
	"Energy",
	"Bullet",
	"Missile",
	"Torpedo",
};

string Weapon::AmmoTypeToName(AmmoType type) {
	return AmmoNames[(int)type];
}

AmmoType Weapon::AmmoNameToType(string typeName ) {
	int i;
	for(i=0;i<max_ammo;i++){
		if(AmmoNames[i]==typeName)
			return (AmmoType)i;
	}
	// Didn't find anything
	cerr<<"There is no Ammo Type '"<<typeName<<"'"<<endl;
	return max_ammo;
}


