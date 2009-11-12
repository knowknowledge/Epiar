/*
 * Filename      : models.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
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
		bool parserCB( string& sectionName, string& subName, string& value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "image" ) {
				image.Load( value );
			} else PPA_MATCHES( "mass" ) {
				mass = atoi( value.c_str() );
			} else PPA_MATCHES( "rotationsPerSecond" ) {
				rotPerSecond = atof( value.c_str() );
			} else PPA_MATCHES( "engine" ) {
				Engines *engines = Engines::Instance();

				engine = engines->LookUp( value );
				
				if( engine == NULL ) {
					Log::Error( "Model parser could not find engine '%s'.", value.c_str() );
				}
			} else PPA_MATCHES( "thrustOffset" ) {
				thrustOffset = atoi( value.c_str() );
			} else PPA_MATCHES( "maxEnergyAbsorption" ) {
				maxEnergyAbsorption = atoi( value.c_str() );
			}
			
			return true;
		}
		
		void _dbg_PrintInfo( void ) {
		
		}
		
		string GetName( void ) const {
			return name;
		}
		
		float GetRotationsPerSecond( void ) {
			return rotPerSecond;
		}
		
		float GetAcceleration( void ) {
			if( engine ) {
				return( engine->GetForceOutput() / (float)mass );
			} else {
				return( 0. ); // no engine
			}
		}
		
		Image *GetImage( void ) {
			return &image;
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
		
	private:
		string name;
		Image image;
		Engine *engine;
		short int mass;
		short int thrustOffset; // engine flare animation offset
		float rotPerSecond;
		short int maxEnergyAbsorption; 
};

// Class that holds list of all models; manages them
class Models {
	public:
		static Models *Instance();
		bool Load( string& filename );
		Model *GetModel( string& modelName );

	protected:
		Models() {};
		Models( const Models & );
		Models& operator= (const Models&);

	private:
		static Models *pInstance;
		list<Model *> models;
};

#endif // __h_models__
