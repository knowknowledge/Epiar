/**\file			outfit.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, April 29, 2010
 * \date			Modified: Thursday, April 29, 2010
 * \brief
 * \details
 */

#include "Engine/outfit.h"
#include "Graphics/image.h"
#include "Utilities/components.h"

/** Default Constructor
 */
Outfit::Outfit()
	:msrp(0)
	,picture(NULL)
	,rotPerSecond(0)
	,maxSpeed(0)
	,forceOutput(0)
	,mass(0)
	,cargoSpace(0)
	,surfaceArea(0)
	,hullStrength(0)
	,shieldStrength(0)
{
}

/** Full Argument Constructor
 */
Outfit::Outfit(
	            int _msrp,
	            Image* _picture,
	            float _rotPerSecond,
	            float _maxSpeed,
	            float _forceOutput,
	            float _mass,
	            int _cargoSpace,
	            int _surfaceArea,
	            int _hullStrength,
	            int _shieldStrength
	            )
	:msrp(_msrp)
	,picture(_picture)
	,rotPerSecond(_rotPerSecond)
	,maxSpeed(_maxSpeed)
	,forceOutput(_forceOutput)
	,mass(_mass)
	,cargoSpace(_cargoSpace)
	,surfaceArea(_surfaceArea)
	,hullStrength(_hullStrength)
	,shieldStrength(_shieldStrength)
{
}

/** Default Copy Constructor
 */
Outfit& Outfit::operator= (const Outfit& other)
{
	msrp = other.msrp;
	picture = other.picture;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
	forceOutput = other.forceOutput;
	mass = other.mass;
	cargoSpace = other.cargoSpace;
	surfaceArea = other.surfaceArea;
	hullStrength = other.hullStrength;
	shieldStrength = other.shieldStrength;
	return *this;
}

/**	Add different Outfits together to combine their effectiveness
 */
Outfit Outfit::operator+ (const Outfit& other)
{
	Outfit total;
	total.msrp = msrp + other.msrp;
	total.picture = picture; // Keep the first picture
	total.rotPerSecond = rotPerSecond + other.rotPerSecond;
	total.maxSpeed = maxSpeed + other.maxSpeed;
	total.forceOutput = forceOutput + other.forceOutput;
	total.mass = mass + other.mass;
	total.cargoSpace = cargoSpace + other.cargoSpace;
	total.surfaceArea = surfaceArea + other.surfaceArea;
	total.hullStrength = hullStrength + other.hullStrength;
	total.shieldStrength = shieldStrength + other.shieldStrength;
	return total;
}

/**	Add different Outfits together to combine their effectiveness
 */
Outfit& Outfit::operator+= (const Outfit& other)
{
	msrp += other.msrp;
	// Don't change the picture
	rotPerSecond += other.rotPerSecond;
	maxSpeed += other.maxSpeed;
	forceOutput += other.forceOutput;
	mass += other.mass;
	cargoSpace += other.cargoSpace;
	surfaceArea += other.surfaceArea;
	hullStrength += other.hullStrength;
	shieldStrength += other.shieldStrength;
	return *this;
}

/**\brief Parses weapon information
 */
bool Outfit::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "picName" ) {
		Image* pic = Image::Get(value);
		// This can be accessed by either the path or the modelName
		Image::Store(name, pic);
		SetPicture(pic);
	} else PPA_MATCHES( "msrp" ) {
		if (atoi( value.c_str()) != 0)
			SetMSRP( atoi( value.c_str() ));
	} else PPA_MATCHES( "rotationsPerSecond" ) {
			SetRotationsPerSecond( atof( value.c_str() ));
	} else PPA_MATCHES( "maxSpeed" ) {
			SetMaxSpeed( atof( value.c_str() ));
	} else PPA_MATCHES( "forceOutput" ) {
			SetForceOutput( atof( value.c_str() ));
	} else PPA_MATCHES( "mass" ) {
			SetMass( atoi( value.c_str() ));
	} else PPA_MATCHES( "hullStrength" ) {
			SetHullStrength( atoi( value.c_str() ));
	} else PPA_MATCHES( "shieldStength" ) {
			SetShieldStrength( atoi( value.c_str() ));
	} else PPA_MATCHES( "cargoSpace" ) {
			SetCargoSpace( atoi( value.c_str() ));
	}
	return true;
}

/** \brief Converts the Outfit object to an XML node.
 */
xmlNodePtr Outfit::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );

	xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST this->GetPicture()->GetPath().c_str() );

	snprintf(buff, sizeof(buff), "%f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotsPerSecond", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "force", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetHullStrength() );
	xmlNewChild(section, NULL, BAD_CAST "hull", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetShieldStrength() );
	xmlNewChild(section, NULL, BAD_CAST "shield", BAD_CAST buff );

	return section;
}

/**\brief Prints debugging information (not implemented)
 */
void Outfit::_dbg_PrintInfo( void ) {
	cout << "Outfit: " << name << endl;
}

/**
 * \function GetMSRP
 * \brief Get the msrp
 *
 * \function SetMSRP
 * \brief Set the msrp
 * \param _msrp The new msrp value
 *
 * \function GetPicture
 * \brief Get the picture
 *
 * \function SetPicture
 * \brief Set the picture
 * \param _picture The new picture value
 *
 * \function GetRotationsPerSecond
 * \brief Get the rotPerSecond
 *
 * \function SetRotationsPerSecond
 * \brief Set the rotPerSecond
 * \param _rotPerSecond The new rotPerSecond value
 *
 * \function GetMaxSpeed
 * \brief Get the maxSpeed
 *
 * \function SetMaxSpeed
 * \brief Set the maxSpeed
 * \param _maxSpeed The new maxSpeed value
 *
 * \function GetForceOutput
 * \brief Get the forceOutput
 *
 * \function SetForceOutput
 * \brief Set the forceOutput
 * \param _forceOutput The new forceOutput value
 *
 * \function GetMass
 * \brief Get the mass
 *
 * \function SetMass
 * \brief Set the mass
 * \param _mass The new mass value
 *
 * \function GetCargoSpace
 * \brief Get the cargoSpace
 *
 * \function SetCargoSpace
 * \brief Set the cargoSpace
 * \param _cargoSpace The new cargoSpace value
 *
 * \function GetSurfaceArea
 * \brief Get the surfaceArea
 *
 * \function SetSurfaceArea
 * \brief Set the surfaceArea
 * \param _surfaceArea The new surfaceArea value
 *
 * \function GetHullStrength
 * \brief Get the hullStrength
 *
 * \function SetHullStrength
 * \brief Set the hullStrength
 * \param _hullStrength The new hullStrength value
 *
 * \function GetShieldStrength
 * \brief Get the shieldStrength
 *
 * \function SetShieldStrength
 * \brief Set the shieldStrength
 * \param _shieldStrength The new shieldStrength value
 */

/**\class Outfits
 * \brief Collection of Outfit objects
 */
Outfits *Outfits::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Outfit instance.
 * \return Pointer to the Outfit instance
 */
Outfits *Outfits::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Outfits; // create the sold instance
		pInstance->rootName = "outfits";
		pInstance->componentName = "outfit";
	}
	return( pInstance );
}

/**\fn Outfits::GetOutfit(string name)
 *  \brief Retrieves the Outfit by name
 * \fn Outfits::newComponent()
 *  \brief Creates a new Outfit
 * \fn Outfits::Outfits(const Outfits&)
 *  \brief An empty constructor.
 * \fn Outfits::operator= (const Outfits&)
 *  \brief An empty assignment operator.
 */
