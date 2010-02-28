/**\file			models.h
 * \author			Chris Thielen (chris@luethy.net)
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
class Model : public Component {
	public:
		Model();
		Model(const Model& other);
		Model( string _name, Image* _image, Engine* _engine, float _mass, short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _maxEnergyAbsorption);
		bool parserCB( string sectionName, string subName, string value );
		xmlNodePtr ToXMLNode(string componentName);
		void _dbg_PrintInfo( void );
		
		float GetRotationsPerSecond( void ) { return rotPerSecond; }
		float GetMaxSpeed( void ) { return maxSpeed; }
		Engine* GetEngine( void ) const { return engine; }
		float GetAcceleration( void );
		float GetMass(void ) { return mass; }
		Image *GetImage( void ) { return image; }
		string GetFlareAnimation( void );
		int GetThrustOffset( void ) { return thrustOffset; }
		short int getMaxEnergyAbsorption() { return maxEnergyAbsorption; }
		void PlayEngineThrust( float volume, Coordinate offset );
		
	private:
		Image *image;
		Engine *engine;
		float mass;
		short int thrustOffset; // engine flare animation offset
		float rotPerSecond;
		float maxSpeed;
		short int maxEnergyAbsorption; 
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
