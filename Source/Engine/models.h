/**\file			models.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_models__
#define __h_models__

#include "Engine/engines.h"
#include "Graphics/image.h"
#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/components.h"

// Abstraction of a single ship model
class Model : public Outfit {
	public:
		Model();
  		Model& operator= (const Model&);
		Model( string _name, Image* _image, float _mass, short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _hullStrength, int _msrp, int _cargoSpace);
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		void _dbg_PrintInfo( void );
		
		Image *GetImage( void ) { return image; }
		int GetThrustOffset( void ) { return thrustOffset; }
		
	private:
		Image *image; ///< The Image used when drawing these ships in space.
		short int thrustOffset; ///< The number of pixels engine flare animation offset
};

// Class that holds list of all models; manages them
class Models : public Components {
	public:
		static Models *Instance();
		Model* GetModel(string name) { return (Model*) this->Get(name); }
		Component* newComponent() { return new Model(); }

	protected:
		Models() {};
		Models( const Models & );
		Models& operator= (const Models&);

	private:
		static Models *pInstance;
};

#endif // __h_models__
