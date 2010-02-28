/**\file			engines.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_engines__
#define __h_engines__

#include "Audio/sound.h"
#include "Graphics/animation.h"
#include "Utilities/components.h"
#include "includes.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single engine
class Engine : public Component {
	public:
		bool parserCB( string sectionName, string subName, string value );
		xmlNodePtr ToXMLNode(string componentName);

		void _dbg_PrintInfo( void );
		
		float GetForceOutput( void ) { return forceOutput; }
		string GetFlareAnimation( void ) { return flareAnimation; }
		short int GetMSRP( void ) { return msrp; }
		short int GetFoldDrive( void ) { return foldDrive; }
		
		Sound *thrustsound;

	private:
		float forceOutput;
		short int msrp;
		bool foldDrive;
		string flareAnimation;
};

// Class that holds list of all planets; manages them
class Engines : public Components {
	public:
		static Engines *Instance();
		Engine* GetEngine(string name) { return (Engine*) this->Get(name); }
		Component* newComponent() { return new Engine(); }

	protected:
		Engines() {};
		Engines( const Engines & );
		Engines& operator= (const Engines&);
		
	private:
		static Engines *pInstance;
};

#endif // __h_planets__
