/**\file			outfit.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, April 29, 2010
 * \date			Modified: Thursday, April 29, 2010
 * \brief
 * \details
 */

#include "Engine/outfit.h"
#include "Graphics/image.h"

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
 * \function GetRotPerSecond
 * \brief Get the rotPerSecond
 *
 * \function SetRotPerSecond
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
