/**\file			weapons.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/weapons.h"
#include "Utilities/trig.h"
#include "Engine/models.h"
#include "Engine/outfit.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

/** \class Weapon
 *  \brief A Ship Outfit used for attacking other Ships.
 *  \see Weapons
 */

/**\brief Empty constructor
 */
Weapon::Weapon(void) :
	image(NULL),
	sound(NULL),
	weaponType(0),
	payload(0),
	velocity(0),
	acceleration(0),
	ammoType(energy_ammo),
	ammoConsumption(0),
	fireDelay(0),
	lifetime(0),
	tracking(0.0f)
{
	SetName("dead");
}

/**\brief Assignment operator (Copy fields)
 */
Weapon& Weapon::operator=(const Weapon& other) {
	Outfit(*this) = Outfit(other);

	name = other.name;
	image = other.image;
	weaponType = other.weaponType;
	payload = other.payload;
	velocity = other.velocity;
	acceleration = other.acceleration;
	ammoType = other.ammoType;
	ammoConsumption = other.ammoConsumption;
	fireDelay = other.fireDelay;
	lifetime = other.lifetime;
	tracking = other.tracking;
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
	image(_image),
	sound(_sound),
	weaponType(_weaponType),
	payload(_payload),
	velocity(_velocity),
	acceleration(_acceleration),
	ammoType(_ammoType),
	ammoConsumption(_ammoConsumption),
	fireDelay(_fireDelay),
	lifetime(_lifetime),
	tracking(_tracking)
{
	SetName(_name);
	SetMSRP( _msrp );
	SetPicture( _pic );
	
	// Generic Outfit stuff
	//((Component*)this)->SetName(_name);
}


/**\brief Empty destructor
 */
Weapon::~Weapon(void)
{
}

/**\brief Parses weapon information
 */
bool Weapon::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"weaponType")) ){
		weaponType = (short int)NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node weaponType while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"imageName")) ){
		image = Image::Get( NodeToString(doc,attr) );
	} else {
		LogMsg(ERR,"Could not find child node imageName while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"picName")) ){
		Image* pic = Image::Get( NodeToString(doc,attr) );
		// This image can be accessed by either the path or the Weapon Name
		Image::Store(name, pic);
		SetPicture(pic);
	} else {
		LogMsg(ERR,"Could not find child node picName while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"payload")) ){
		payload = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node payload while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"velocity")) ){
		velocity = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node velocity while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"acceleration")) ){
		acceleration = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node acceleration while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"ammoType")) ){
		value = NodeToString(doc,attr);
		ammoType = AmmoNameToType(value);
		if(ammoType>=max_ammo) {
			LogMsg(ERR,"ammoType is >= max_ammo in Weapons XML parsing");
			return false;
		}
	} else {
		LogMsg(ERR,"Could not find child node ammoType while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"ammoConsumption")) ){
		ammoConsumption = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node ammoConsumption while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"fireDelay")) ){
		fireDelay = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node fireDelay while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"lifetime")) ){
		lifetime = NodeToInt(doc,attr);
	} else {
		LogMsg(ERR,"Could not find child node lifetime while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"tracking")) ){
		float _tracking = NodeToFloat(doc,attr);
		if (_tracking > 1.0f ) _tracking = 1.0f;
		if (_tracking < 0.0001f ) _tracking = 0.0f;
		tracking = _tracking;
	} else {
		LogMsg(ERR,"Could not find child node tracking while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( (short int)atoi( value.c_str() ));
	} else {
		LogMsg(ERR,"Could not find child node msrp while searching component");
		return false;
	}

	if( (attr = FirstChildNamed(node,"sound")) ){
		string pathPrefix = "Resources/Audio/Weapons/";
		value = NodeToString(doc,attr);
		if( value.find(pathPrefix)==0 ) {
			this->sound = Sound::Get( value );
		} else {
			this->sound = Sound::Get( value.insert(0,pathPrefix) );
		}
		if( this->sound==NULL) {
			// Do not return false here - they may be disabling audio on purpose or audio may not be supported on their system
			LogMsg(NOTICE,"Could not load sound file while searching component");
		}
	} else {
		LogMsg(ERR,"Could not find child node sound while searching component");
		return false;
	}

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

/**\fn Weapon::GetImage( )
 *  \brief Returns the image of the fired weapon
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

/**\class Weapons
 * \brief Collection of Weapon objects
 *
 * \see Weapon
 */
Weapons *Weapons::pInstance = 0; // initialize pointer

/**\brief Gets or returns Weapons instance
 * \return Pointer to a Weapons object
 */
Weapons *Weapons::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Weapons; // create the sold instance
		pInstance->rootName = "weapons";
		pInstance->componentName = "weapon";
	}
	return( pInstance );
}

/**\fn Weapons::GetWeapon(string& weaponName)
 *  \brief Retrieves a Weapon by name
 * \fn Weapons::newComponent( )
 *  \brief Creates a new Weapon object
 * \fn Weapons::Weapons( const Weapons & )
 *  \brief Empty copy constructor
 * \fn Weapons::operator = (const Weapons&)
 *  \brief Empty assignment operator
 */

