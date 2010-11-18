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
	,rotPerSecond(0)
	,maxSpeed(0)
	,forceOutput(0)
	,mass(0)
	,cargoSpace(0)
	,surfaceArea(0)
	,hullStrength(0)
	,shieldStrength(0)
	,weaponSlots(0)
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
	            int _shieldStrength,
	            int _weaponSlots
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
	,weaponSlots(_weaponSlots)
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
	weaponSlots = other.weaponSlots;
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
	total.weaponSlots = weaponSlots;
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
	//weaponSlots += other.weaponSlots;
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

	if( (attr = FirstChildNamed(node,"rotationsPerSecond")) ){
		value = NodeToString(doc,attr);
		SetRotationsPerSecond( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"maxSpeed")) ){
		value = NodeToString(doc,attr);
		SetMaxSpeed( static_cast<float>(atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"forceOutput")) ){
		value = NodeToString(doc,attr);
		SetForceOutput( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"mass")) ){
		value = NodeToString(doc,attr);
		SetMass( static_cast<float> (atof( value.c_str() )));
	}

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"surfaceArea")) ){
		value = NodeToString(doc,attr);
		SetSurfaceArea( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"hullStrength")) ){
		value = NodeToString(doc,attr);
		SetHullStrength( atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"shieldStrength")) ){
		value = NodeToString(doc,attr);
		SetShieldStrength( (short)atoi( value.c_str() ));
	}

	if( (attr = FirstChildNamed(node,"weaponSlots")) ){
		ConfigureWeaponSlots(doc, attr);
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

	//snprintf(buff, sizeof(buff), "%d", this->GetWeaponSlots() );
	//xmlNewChild(section, NULL, BAD_CAST "weaponSlots", BAD_CAST buff );

	assert(true == false); // FIXME ^^     need to create XML exporting routine for weaponSlots structure

	return section;
}

/**\brief Prints debugging information (not implemented)
 */
void Outfit::_dbg_PrintInfo( void ) {
	cout << "Outfit: " << name << endl;
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


/**\brief Configure the ship's weapon slots based on the XML node weaponSlots.
 * These routines are in Outfit rather than Model for flexibility reasons.
 */
bool Outfit::ConfigureWeaponSlots( xmlDocPtr doc, xmlNodePtr node ) {

	xmlNodePtr slotPtr;
	string value;

	//if( (slotPtr = FirstChildNamed(node,"slot")) ){
        for( slotPtr = FirstChildNamed(node,"slot"); slotPtr != NULL; slotPtr = NextSiblingNamed(slotPtr,"slot") ){
		struct ws newSlot;

		xmlNodePtr attr;

		if( (attr = FirstChildNamed(slotPtr,"name")) ){
			value = NodeToString(doc,attr);
			newSlot.name = value;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"coord")) ){
			value = NodeToString(doc,attr);
			// go deeper...

			xmlNodePtr coordAttr;
			if( (coordAttr = FirstChildNamed(attr,"mode")) ){
				value = NodeToString(doc,coordAttr);
				newSlot.mode = value;
			} else return false;
			if( (coordAttr = FirstChildNamed(attr,"x")) ){
				value = NodeToString(doc,coordAttr);
				newSlot.x = atof(value.c_str());
			} else return false;
			if( (coordAttr = FirstChildNamed(attr,"y")) ){
				value = NodeToString(doc,coordAttr);
				newSlot.y = atof(value.c_str());
			} else return false;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"angle")) ){
			value = NodeToString(doc,attr);
			newSlot.angle = atof(value.c_str());
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"motionAngle")) ){
			value = NodeToString(doc,attr);
			newSlot.motionAngle = atof(value.c_str());
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"content")) ){
			// this check is necessary because NodeToString() won't translate <item></item> into ""
			if(attr->xmlChildrenNode)
				value = NodeToString(doc,attr);
			else
				value = ""; // slot is empty

			newSlot.content = value;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"firingGroup")) ){
			value = NodeToString(doc,attr);
			newSlot.firingGroup = (short)atoi(value.c_str());
		} else return false;

		//WSDebug(newSlot);

		weaponSlots.push_back(newSlot);

		//Outfit::ConfigureWeaponSlots( doc, node );
	}

	WSDebug(weaponSlots);


        return true;
}

/**\brief Configure the ship's weapon slots based on a list passed in (probably from the constructor)
 */
bool Outfit::ConfigureWeaponSlots( vector<struct ws>& slots ) {
        this->weaponSlots = slots;
        return true;
}

/**\brief Configure the ship's weapon slots using default values.
 */
bool Outfit::ConfigureWeaponSlots() {
        struct ws wsFront1;
        struct ws wsFront2;

        wsFront1.name = "front 1";
        wsFront1.x = -0.3;
	wsFront1.y = 2.0;
        wsFront1.angle = 0.0;
        wsFront1.motionAngle = 0.0;
	wsFront1.content = "";
	wsFront1.firingGroup = 0;

        wsFront2.name = "front 2";
        wsFront1.x = 0.3;
	wsFront1.y = 2.0;
        wsFront2.angle = 0.0;
        wsFront2.motionAngle = 0.0;
	wsFront2.firingGroup = 1;

	vector<struct ws> newSlots;
        newSlots.push_back(wsFront1);
        newSlots.push_back(wsFront2);
	this->weaponSlots = newSlots;

        return true;
}

/**\brief Return the total number of weapon slots of any kind that this Outfit (probably a Model) has.
 */
int Outfit::GetWeaponSlotCount(){
	return this->weaponSlots.size();
}

void Outfit::WSDebug(struct Outfit::ws slot){
	printf("WSD      name=%s x=%f y=%f angle=%f motionAngle=%f content=%s firingGroup=%d\n", slot.name.c_str(), slot.x, slot.y, slot.angle, slot.motionAngle, slot.content.c_str(), slot.firingGroup);
}

void Outfit::WSDebug(vector<struct ws>& slots){
	cout << "WSD  Ship model: " << this->GetName() << endl;
	for(unsigned int i = 0; i < slots.size(); i++){
		WSDebug(slots[i]);
	}
}


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

/**\fn Outfits::GetOutfit(string name)()
 *  \brief Retrieves the Outfit by name
 * \fn Outfits::newComponent()()
 *  \brief Creates a new Outfit
 * \fn Outfits::Outfits(const Outfits&)()
 *  \brief An empty constructor.
 * \fn Outfits::operator= (const Outfits&)()
 *  \brief An empty assignment operator.
 */
