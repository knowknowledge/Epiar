/**\file			models.cpp
 * \author			Chris Thielen (chris@epiar.net)
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
	,thrustOffset(0)
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
	hullStrength = other.hullStrength;
	shieldStrength = other.shieldStrength;
	return *this;
}

/**\brief Creates a Model with the given parameters.
 * \param _name Name of the ship
 * \param _image Image of the ship
 * \param _mass Mass of the ship
 * \param _thrustOffset For animation
 * \param _rotPerSecond Rotation per second
 * \param _maxSpeed Maximum speed
 * \param _hullStrength Maximum damage it can take
 * \param _shieldStrength Maximum damage it can take
 * \param _msrp Price
 * \param _cargoSpace Tons of cargo space
 */

Model::Model( string _name, Image* _image, float _mass, short int _thrustOffset, float _rotPerSecond,
		float _maxSpeed, int _hullStrength, int _shieldStrength, int _msrp, int _cargoSpace) :
	image(_image),
	thrustOffset(_thrustOffset)
{
	SetName(_name);
	SetMass(_mass);
	SetRotationsPerSecond(_rotPerSecond);
	SetMaxSpeed(_maxSpeed);
	SetMSRP(_msrp);
	SetCargoSpace(_cargoSpace);
	SetHullStrength(_hullStrength);
	SetShieldStrength(_shieldStrength);
	Outfit::ConfigureWeaponSlots();
	//((Component*)this)->SetName(_name);
}

Model::Model( string _name, Image* _image, float _mass, short int _thrustOffset, float _rotPerSecond,
		float _maxSpeed, int _hullStrength, int _shieldStrength, int _msrp, int _cargoSpace,
		vector<struct ws>& _weaponSlots) :
	image(_image),
	thrustOffset(_thrustOffset)
{
	SetName(_name);
	SetMass(_mass);
	SetRotationsPerSecond(_rotPerSecond);
	SetMaxSpeed(_maxSpeed);
	SetMSRP(_msrp);
	SetCargoSpace(_cargoSpace);
	SetHullStrength(_hullStrength);
	SetShieldStrength(_shieldStrength);
	Outfit::ConfigureWeaponSlots(_weaponSlots);
	//((Component*)this)->SetName(_name);
}

/**\brief For parsing XML file into fields.
 */
bool Model::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"image")) ){
		image = Image::Get( NodeToString(doc,attr) );
		Image::Store(name, image);
		SetPicture(image);
	} else return false;

	if( (attr = FirstChildNamed(node,"mass")) ){
		value = NodeToString(doc,attr);
		SetMass( static_cast<float> (atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"rotationsPerSecond")) ){
		value = NodeToString(doc,attr);
		SetRotationsPerSecond( static_cast<float>(atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"thrustOffset")) ){
		value = NodeToString(doc,attr);
		thrustOffset = static_cast<short>(atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"maxSpeed")) ){
		value = NodeToString(doc,attr);
		SetMaxSpeed( static_cast<float>(atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( (short int)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"hullStrength")) ){
		value = NodeToString(doc,attr);
		SetHullStrength( (short)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"shieldStrength")) ){
		value = NodeToString(doc,attr);
		SetShieldStrength( (short)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"weaponSlots")) ){
		// pass the weaponSlots XML node into a handler function
		Outfit::ConfigureWeaponSlots( doc, attr );
	} else {
		cout << "Model::FromXMLNode(): Did not find weapon slot configuration - assuming defaults." << endl;
		// with no parameters, it sets default values
		Outfit::ConfigureWeaponSlots();
	}

	return true;
}

/**\brief Prints debugging information.
 */
void Model::_dbg_PrintInfo( void ) {
	if( mass <= 0.001 ){
		// Having an incorrect Mass can cause the Model to have NAN position which will cause it to disappear unexpectedly.
		LogMsg(ERR,"Model %s does not have a valid Mass value (%f).",name.c_str(),mass);
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
	snprintf(buff, sizeof(buff), "%d", this->GetHullStrength() );
	xmlNewChild(section, NULL, BAD_CAST "hullStrength", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetShieldStrength() );
	xmlNewChild(section, NULL, BAD_CAST "shieldStrength", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetCargoSpace() );
	xmlNewChild(section, NULL, BAD_CAST "cargoSpace", BAD_CAST buff );

	return section;
}


/**\fn Model::GetImage()
 *  \brief Retrieves a pointer to the Image
 * \fn Model::GetThrustOffset()
 *  \brief Retrieves the offset of the thrust from center
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
