/**\file			engines.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/engines.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"

/**\class Engine
 * \brief A ship propeller system.
 */

/**\brief Initializes a new Engine to default values.
 *
 */
Engine::Engine() :
	thrustsound(NULL),
	foldDrive(false),
	flareAnimation("")
{
	SetName("dead");
}

/**\brief Assignment constructor - copies all fields.
 */
Engine& Engine::operator= (const Engine& other) {
	Outfit(*this) = Outfit(other);

	name = other.name;
	thrustsound = other.thrustsound;
	forceOutput = other.forceOutput;
	foldDrive = other.foldDrive;
	flareAnimation = other.flareAnimation;
	return *this;
}

/**\brief Initializes a new Engine with the given parameters.
 * \param _name Name of the Engine
 * \param _thrustsound Pointer to a Sound object that will be played for thrust
 * \param _forceOutput The amount of force the Engine generates
 * \param _msrp Price of the Engine
 * \param foldDrive Fold capable
 * \param _flareAnimation Thrust animation
 */
Engine::Engine( string _name, Sound* _thrustsound, float _forceOutput,
		short int _msrp, bool _foldDrive, string _flareAnimation, Image* _pic) :
	thrustsound(_thrustsound),
	foldDrive(_foldDrive),
	flareAnimation(_flareAnimation)
{
	SetName(_name);
	SetMSRP(_msrp);
	SetPicture(_pic);
	SetForceOutput(_forceOutput);
}

/**\brief Parser to parse the XML file
 */
bool Engine::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"forceOutput")) ){
		value = NodeToString(doc,attr);
		SetForceOutput( static_cast<float> (atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( (short int)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"foldDrive")) ){
		value = NodeToString(doc,attr);
		foldDrive = (atoi( value.c_str() ) != 0);
	} else return false;

	if( (attr = FirstChildNamed(node,"flareAnimation")) ){
		flareAnimation = NodeToString(doc,attr);
	} else return false;

	if( (attr = FirstChildNamed(node,"thrustSound")) ){
		thrustsound = Sound::Get( NodeToString(doc,attr) );
	} else return false;

	if( (attr = FirstChildNamed(node,"picName")) ){
		Image* pic = Image::Get( NodeToString(doc,attr) );
		// This image can be accessed by either the path or the Engine Name
		Image::Store(name, pic);
		SetPicture(pic);
	} else return false;

	return true;
}

/**\brief Converts the Engine object to an XML node.
 */
xmlNodePtr Engine::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%1.1f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "forceOutput", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "foldDrive", BAD_CAST (this->GetFoldDrive()?"1":"0") );
	xmlNewChild(section, NULL, BAD_CAST "flareAnimation", BAD_CAST this->GetFlareAnimation().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "thrustSound", BAD_CAST this->thrustsound->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST this->GetPicture()->GetPath().c_str() );

	return section;
}

/**\brief Prints debugging information
 */
void Engine::_dbg_PrintInfo( void ) {
	//cout << "Engine called " << name << ", priced at " << msrp << " with force of " << forceOutput << " and fold capability set to " << foldDrive << endl;
}

/**\fn Engine::GetForceOutput()
 * \brief Returns the force output.
 */

/**\fn Engine::GetFlareAnimation()
 * \brief Gets the animation.
 */

/**\fn Engine::GetMSRP()
 * \brief Retrieves the price
 */

/**\fn Engine::GetFoldDrive()
 * \brief Retrieves fold capability
 */

/**\class Engines
 * \brief Handles ship engines. */

Engines *Engines::pInstance = 0; // initialize pointer

/**\brief Initializes the instance or gets a pointer to it.
 * \return Pointer to an Engine object
 */
Engines *Engines::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Engines; // create the sold instance
		pInstance->rootName = "engines";
		pInstance->componentName = "engine";
	}
	return( pInstance );
}

/**\fn Engines::GetEngine(string name)
 * \brief Returns the named Engine
 */

/**\fn Engines::newComponent()
 * \brief Creates a new Engine object.
 */

// Projected members
/**\fn Engines::Engines( const Engines & )
 * \brief Creates a new Engines object
 */

/**\fn Engines::operator= (const Engines &)
 * \brief Assignment operator (empty)
 */

