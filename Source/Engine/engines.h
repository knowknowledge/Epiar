/*
 * Filename      : engines.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_engines__
#define __h_engines__

#include "Graphics/animation.h"
#include "includes.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single engine
class Engine {
	public:
		bool parserCB( string sectionName, string subName, string value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "forceOutput" ) {
				forceOutput = atof( value.c_str() );
			} else PPA_MATCHES( "msrp" ) {
				msrp = (short int)atoi( value.c_str() );
			} else PPA_MATCHES( "foldDrive" ) {
				foldDrive = (bool)atoi( value.c_str() );
			} else PPA_MATCHES( "flareAnimation" ) {
				flareAnimation = value;
			}
			
			return true;
		}
		
		void _dbg_PrintInfo( void ) {
			//cout << "Engine called " << name << ", priced at " << msrp << " with force of " << forceOutput << " and fold capability set to " << foldDrive << endl;
		}
		
		bool IsNamed( string engineName ) {
			if( engineName == name )
				return true;
			
			return false;
		}
		
		float GetForceOutput( void ) {
			return forceOutput;
		}
		
		string GetFlareAnimation( void ) {
			return flareAnimation;
		}
		
	private:
		string name;
		float forceOutput;
		short int msrp;
		bool foldDrive;
		string flareAnimation;
};

// Class that holds list of all planets; manages them
class Engines {
	public:
		static Engines *Instance();
		bool Load( string filename );
		
		Engine *LookUp( string engineName );

	protected:
		Engines() {};
		Engines( const Engines & );
		Engines& operator= (const Engines&);
		
	private:
		static Engines *pInstance;
		list<Engine *> engines;
};

#endif // __h_planets__
