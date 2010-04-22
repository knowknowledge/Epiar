/**\file			models.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/components.h"
#include "Engine/models.h"

/**\class Models
 * \brief Handles ship models. */

/**\brief Creates an empty Model object.
 */
Model::Model()
	:image(NULL)
	,mass(0.0f)
	,thrustOffset(0)
	,rotPerSecond(0.0f)
	,maxSpeed(0.0f)
	,msrp(0)
	,cargoSpace(0)
	,maxEnergyAbsorption(0)
{
	SetName("dead");
}

/**\brief Assignment operator (copy fields).
 */
Model& Model::operator=(const Model& other) {
	name = other.name;
	image = other.image;
	mass = other.mass;
	thrustOffset = other.thrustOffset;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
	msrp = other.msrp;
	cargoSpace = other.cargoSpace;
	maxEnergyAbsorption = other.maxEnergyAbsorption;
	return *this;
}

/**\brief Creates a Model with the given parameters.
 * \param _name Name of the ship
 * \param _image Image of the ship
 * \param _mass Mass of the ship
 * \param _thrustOffset For animation
 * \param _rotPerSecond Rotation per second
 * \param _maxSpeed Maximum speed
 * \param _maxEnergyAbsorption Maximum damage it can take
 * \param _msrp Price
 * \param _cargoSpace Tons of cargo space
 */
Model::Model( string _name, Image* _image, float _mass,
		short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _maxEnergyAbsorption, int _msrp, int _cargoSpace) :
	image(_image),
	mass(_mass),
	thrustOffset(_thrustOffset),
	rotPerSecond(_rotPerSecond),
	maxSpeed(_maxSpeed),
	msrp(_msrp),
	cargoSpace(_cargoSpace),
	maxEnergyAbsorption(_maxEnergyAbsorption)
{
	SetName(_name);
	//((Component*)this)->SetName(_name);
}

/**\brief For parsing XML file into fields.
 */
bool Model::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "image" ) {
		image = (Image*)Image::Get( value );
	} else PPA_MATCHES( "mass" ) {
		mass = (float)atof( value.c_str() );
	} else PPA_MATCHES( "rotationsPerSecond" ) {
		rotPerSecond = static_cast<float>(atof( value.c_str() ));
	} else PPA_MATCHES( "thrustOffset" ) {
		thrustOffset = (short)atoi( value.c_str() );
	} else PPA_MATCHES( "maxSpeed" ) {
		maxSpeed = (float)atof( value.c_str() );
	} else PPA_MATCHES( "msrp" ) {
		msrp = atoi( value.c_str() );
	} else PPA_MATCHES( "cargoSpace" ) {
		cargoSpace = atoi( value.c_str() );
	} else PPA_MATCHES( "maxEnergyAbsorption" ) {
		maxEnergyAbsorption = (short)atoi( value.c_str() );
	}
	// TODO This is a bad spot for this.
	if(image!=NULL && name!="dead"){ 
		Image::Store(name,(Resource*)image);
	}
	return true;
}

/**\brief Prints debugging information.
 */
void Model::_dbg_PrintInfo( void ) {
	if( mass <= 0.001 ){
		// Having an incorrect Mass can cause the Model to have NAN position which will cause it to disappear unexpectedly.
		LogMsg(ERROR,"Model %s does not have a valid Mass value (%f).",name.c_str(),mass);
	}
	if(image!=NULL && name!=""){ 
		cout<<"Storing Image for "<<name<<endl;
		Image::Store(name,(Resource*)image);
	} else { assert(0); }
}

/**\brief Converts the Model to an XML node.
 */
xmlNodePtr Model::ToXMLNode(string componentName) {
	char buff[256];
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST this->GetImage()->GetPath().c_str() );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotationsPerSecond", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetThrustOffset() );
	xmlNewChild(section, NULL, BAD_CAST "thrustOffset", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.1f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->getMaxEnergyAbsorption() );
	xmlNewChild(section, NULL, BAD_CAST "maxEnergyAbsorption", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetCargoSpace() );
	xmlNewChild(section, NULL, BAD_CAST "cargoSpace", BAD_CAST buff );

	return section;
}

/**\fn Model::GetRotationsPerSecond()
 *  \brief Retrieves the rotation per second of the model.
 * \fn Model::GetMaxSpeed()
 *  \brief Retrieves the maximum speed
 * \fn Model::GetMass()
 *  \brief Retrieves the mass of the Model
 * \fn Model::GetImage()
 *  \brief Retrieves a pointer to the Image
 * \fn Model::GetThrustOffset()
 *  \brief Retrieves the offset of the thrust from center
 * \fn Model::getMaxEnergyAbsorption()
 *  \brief Retrieves the maximum energy it can absorb
 * \fn Model::GetMSRP()
 *  \brief Retrieves the price of the Model
 */


/**\class Models
 * \brief Collection of Model objects
 */
Models *Models::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Model instance.
 * \return Pointer to the Model instance
 */
Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the sold instance
		pInstance->rootName = "models";
		pInstance->componentName = "model";
	}
	return( pInstance );
}

/**\fn Models::GetModel(string name)
 *  \brief Retrieves the Model by name
 * \fn Models::newComponent()
 *  \brief Creates a new Model
 * \fn Models::Models(const Models&)
 *  \brief An empty constructor.
 * \fn Models::operator= (const Models&)
 *  \brief An empty assignment operator.
 */
