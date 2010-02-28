/**\file			models.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/models.h"
#include "Utilities/components.h"

/**\class Models
 * \brief Handles ship models. */

Model::Model(): image(NULL),engine(NULL),mass(0.0){
	SetName("dead");
}

Model::Model(const Model& other) {
	name = other.name;
	image = other.image;
	engine = other.engine;
	mass = other.mass;
	thrustOffset = other.thrustOffset;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
}

Model::Model( string _name, Image* _image, Engine* _engine, float _mass, short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _maxEnergyAbsorption) :
	image(_image),
	engine(_engine),
	mass(_mass),
	thrustOffset(_thrustOffset),
	rotPerSecond(_rotPerSecond),
	maxSpeed(_maxSpeed)
{
	SetName(_name);
	//((Component*)this)->SetName(_name);
}

xmlNodePtr Model::ToXMLNode(string componentName) {
	char buff[256];
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST this->GetImage()->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST this->GetEngine()->GetName().c_str() );
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

	return section;
}


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

