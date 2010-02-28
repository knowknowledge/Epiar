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
		bool parserCB( string sectionName, string subName, string value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "image" ) {
				image = (Image*)Image::Get( value );
			} else PPA_MATCHES( "mass" ) {
				mass = (float)atof( value.c_str() );
			} else PPA_MATCHES( "rotationsPerSecond" ) {
				rotPerSecond = static_cast<float>(atof( value.c_str() ));
			} else PPA_MATCHES( "engine" ) {
				Engines *engines = Engines::Instance();

				engine = engines->GetEngine( value );
				
				if( engine == NULL ) {
					Log::Error( "Model parser could not find engine '%s'.", value.c_str() );
				}
			} else PPA_MATCHES( "thrustOffset" ) {
				thrustOffset = (short)atoi( value.c_str() );
			} else PPA_MATCHES( "maxSpeed" ) {
				maxSpeed = (float)atof( value.c_str() );
			} else PPA_MATCHES( "maxEnergyAbsorption" ) {
				maxEnergyAbsorption = (short)atoi( value.c_str() );
			}
			// TODO This is a bad spot for this.
			if(image!=NULL && name!="dead"){ 
				Image::Store(name,(Resource*)image);
			}
			return true;
		}
		xmlNodePtr ToXMLNode(string componentName);
		
		void _dbg_PrintInfo( void ) {
			if( mass <= 0.001 ){
				// Having an incorrect Mass can cause the Model to have NAN position which will cause it to disappear unexpectedly.
				Log::Error("Model %s does not have a valid Mass value (%f).",name.c_str(),mass);
			}
			if(image!=NULL && name!=""){ 
				cout<<"Storing Image for "<<name<<endl;
				Image::Store(name,(Resource*)image);
			} else { assert(0); }
		}

		float GetRotationsPerSecond( void ) {
			return rotPerSecond;
		}

		float GetMaxSpeed( void ) {
			return maxSpeed;
		}

		Engine* GetEngine( void ) const {
			return engine;
		}

		float GetAcceleration( void ) {
			if( engine ) {
				return( engine->GetForceOutput() / (float)mass );
			} else {
				return( 0. ); // no engine
			}
		}

		float GetMass(void ) { return mass; }
		
		Image *GetImage( void ) {
			return image;
		}
		
		string GetFlareAnimation( void ) {
			if( engine ) {
				return( engine->GetFlareAnimation() );
			} else {
				return string("Error");
			}
		}
		
		int GetThrustOffset( void ) {
			return thrustOffset;
		}
		
		short int getMaxEnergyAbsorption() { return maxEnergyAbsorption; }

		void PlayEngineThrust( float volume, Coordinate offset ){
			this->engine->thrustsound->SetVolume( volume );
			this->engine->thrustsound->PlayNoRestart( offset );
		};
		
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
