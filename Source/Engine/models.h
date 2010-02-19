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

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single ship model
class Model {
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

				engine = engines->LookUp( value );
				
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
			return true;
		}
		
		void _dbg_PrintInfo( void ) {
			if( mass <= 0.001 ){
				// Having an incorrect Mass can cause the Model to have NAN position which will cause it to disappear unexpectedly.
				Log::Error("Model %s does not have a valid Mass value (%f).",name.c_str(),mass);
			}
			if(image!=NULL && name!=""){ Image::Store(name,(Resource*)image);}
		}
		
		string GetName( void ) const {
			return name;
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
		string name;
		Image *image;
		Engine *engine;
		float mass;
		short int thrustOffset; // engine flare animation offset
		float rotPerSecond;
		float maxSpeed;
		short int maxEnergyAbsorption; 
};

// Class that holds list of all models; manages them
class Models {
	public:
		static Models *Instance();
		bool Load( string& filename );
		bool Save( string filename );
		Model *GetModel( string& modelName );
		list<string> *GetModelNames();

	protected:
		Models() {};
		Models( const Models & );
		Models& operator= (const Models&);

	private:
		static Models *pInstance;
		list<Model *> models;
};

#endif // __h_models__
