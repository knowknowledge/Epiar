/**\file			outfit.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, April 29, 2010
 * \date			Modified: Thursday, April 29, 2010
 * \brief
 * \details
 */
#include "includes.h"
#include "Engine/outfit.h"
#include "Graphics/image.h"
#include "Utilities/log.h"
#include "Utilities/components.h"

/** \class Outfit
 * \brief A piece of Ship Equipment.
 * \details There are two basic types of Outfit objects:
 *          -# Generic Outfit objects that only affect Ship stats (Tougher armor).
 *          -# Specific Ship equipments that require their own classes. (Engines, Outfits, Models)
 * \sa Engine, Model, Outfit
 */

/** Default Constructor
 */
Outfit::Outfit()
	:msrp(0)
	,picture(NULL)
	,description("")
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
				string _description,
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
	,description(_description)
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
	description = other.description;
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
bool Outfit::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"picName")) ){
		Image* pic = Image::Get( NodeToString(doc,attr) );
		// This image can be accessed by either the path or the Engine Name
		Image::Store(name, pic);
		SetPicture(pic);
	} else return false;

	if( (attr = FirstChildNamed(node,"description")) ){
		value = NodeToString(doc,attr);
		SetDescription( value );
	} else {
		LogMsg( WARN, "%s does not have a description.", GetName().c_str() );
	}

	if( (attr = FirstChildNamed(node,"rotsPerSecond")) ){
		value = NodeToString(doc,attr);
		SetRotationsPerSecond( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"maxSpeed")) ){
		value = NodeToString(doc,attr);
		SetMaxSpeed( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"force")) ){
		value = NodeToString(doc,attr);
		SetForceOutput( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"mass")) ){
		value = NodeToString(doc,attr);
		SetMass( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"surfaceArea")) ){
		value = NodeToString(doc,attr);
		SetSurfaceArea( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"hull")) ){
		value = NodeToString(doc,attr);
		SetHullStrength( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"shield")) ){
		value = NodeToString(doc,attr);
		SetShieldStrength( (short)atoi( value.c_str() ));
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

	xmlNewChild(section, NULL, BAD_CAST "description", BAD_CAST this->GetDescription().c_str() );

	snprintf(buff, sizeof(buff), "%f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotsPerSecond", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "force", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetSurfaceArea() );
	xmlNewChild(section, NULL, BAD_CAST "surfaceArea", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetCargoSpace() );
	xmlNewChild(section, NULL, BAD_CAST "cargoSpace", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetHullStrength() );
	xmlNewChild(section, NULL, BAD_CAST "hull", BAD_CAST buff );

	snprintf(buff, sizeof(buff), "%d", this->GetShieldStrength() );
	xmlNewChild(section, NULL, BAD_CAST "shield", BAD_CAST buff );

	return section;
}

/**
 * \fn Outfit::GetMSRP()
 * \brief Get the msrp
 *
 * \fn Outfit::SetMSRP(int _msrp)
 * \brief Set the msrp
 * \param _msrp The new msrp value
 *
 * \fn Outfit::GetPicture()
 * \brief Get the picture
 *
 * \fn Outfit::SetPicture(Image* _picture)
 * \brief Set the picture
 * \param _picture The new picture value
 *
 * \fn Outfit::GetRotationsPerSecond()
 * \brief Get the rotPerSecond
 *
 * \fn Outfit::SetRotationsPerSecond(float _rotPerSecond)
 * \brief Set the rotPerSecond
 * \param _rotPerSecond The new rotPerSecond value
 *
 * \fn Outfit::GetMaxSpeed()
 * \brief Get the maxSpeed
 *
 * \fn Outfit::SetMaxSpeed(float _maxSpeed)
 * \brief Set the maxSpeed
 * \param _maxSpeed The new maxSpeed value
 *
 * \fn Outfit::GetForceOutput()
 * \brief Get the forceOutput
 *
 * \fn Outfit::SetForceOutput(float _forceOutput)
 * \brief Set the forceOutput
 * \param _forceOutput The new forceOutput value
 *
 * \fn Outfit::GetMass()
 * \brief Get the mass
 *
 * \fn Outfit::SetMass(float _mass)
 * \brief Set the mass
 * \param _mass The new mass value
 *
 * \fn Outfit::GetCargoSpace()
 * \brief Get the cargoSpace
 *
 * \fn Outfit::SetCargoSpace(int _cargoSpace)
 * \brief Set the cargoSpace
 * \param _cargoSpace The new cargoSpace value
 *
 * \fn Outfit::GetSurfaceArea()
 * \brief Get the surfaceArea
 *
 * \fn Outfit::SetSurfaceArea(int _surfaceArea)
 * \brief Set the surfaceArea
 * \param _surfaceArea The new surfaceArea value
 *
 * \fn Outfit::GetHullStrength()
 * \brief Get the hullStrength
 *
 * \fn Outfit::SetHullStrength(int _hullStrength)
 * \brief Set the hullStrength
 * \param _hullStrength The new hullStrength value
 *
 * \fn Outfit::GetShieldStrength()
 * \brief Get the shieldStrength
 *
 * \fn Outfit::SetShieldStrength(int _shieldStrength)
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
		pInstance = new Outfits; // create the solid instance
		pInstance->rootName = "outfits";
		pInstance->componentName = "outfit";
	}
	return( pInstance );
}

/**\fn Outfits::GetOutfit(string name)()
 *  \brief Retrieves the Outfit by name
 * \fn Outfits::newComponent()()
 *  \brief Creates a new Outfit
 * \fn Outfits::Outfits(const Outfits&)()
 *  \brief An empty constructor.
 * \fn Outfits::operator= (const Outfits&)()
 *  \brief An empty assignment operator.
 */
