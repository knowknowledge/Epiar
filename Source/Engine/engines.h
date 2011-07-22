/**\file			engines.h
 * \author			Chris Thielen (chris@epiar.net)
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
#include "Engine/outfit.h"
#include "includes.h"

// Abstraction of a single engine
class Engine : public Outfit {
	public:
		Engine();
		Engine& operator= (const Engine&);
		Engine( string _name, Image* _pic, string _description, Sound* _sound, float _forceOutput, short int _msrp, bool _foldDrive, string _flareAnimation);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		string GetFlareAnimation( void ) { return flareAnimation; }
		short int GetFoldDrive( void ) { return foldDrive; }
		Sound* GetSound() { return thrustsound; }

	private:
		Sound *thrustsound;
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
