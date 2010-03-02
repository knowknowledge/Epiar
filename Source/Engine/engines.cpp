/**\file			engines.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/engines.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"

Engine::Engine() :
	thrustsound(NULL),
	forceOutput(0.0),
	msrp(0),
	foldDrive(false),
	flareAnimation("")
{
	SetName("dead");
}

Engine& Engine::operator= (const Engine& other) {
	name = other.name;
	thrustsound = other.thrustsound;
	forceOutput = other.forceOutput;
	msrp = other.msrp;
	foldDrive = other.foldDrive;
	flareAnimation = other.flareAnimation;
	return *this;
}

Engine::Engine( string _name, Sound* _thrustsound, float _forceOutput, short int _msrp, bool _foldDrive, string _flareAnimation) :
	thrustsound(_thrustsound),
	forceOutput(_forceOutput),
	msrp(_msrp),
	foldDrive(_foldDrive),
	flareAnimation(_flareAnimation)
{
	SetName(_name);
}

bool Engine::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "forceOutput" ) {
		forceOutput = static_cast<float> (atof( value.c_str() ));
	} else PPA_MATCHES( "msrp" ) {
		msrp = (short int)atoi( value.c_str() );
	} else PPA_MATCHES( "foldDrive" ) {
		foldDrive = (atoi( value.c_str() ) != 0);
	} else PPA_MATCHES( "flareAnimation" ) {
		flareAnimation = value;
	} else PPA_MATCHES( "thrustSound" ){
		this->thrustsound = Sound::Get( value );
	}

	return true;
}

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

	return section;
}

void Engine::_dbg_PrintInfo( void ) {
	//cout << "Engine called " << name << ", priced at " << msrp << " with force of " << forceOutput << " and fold capability set to " << foldDrive << endl;
}

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

