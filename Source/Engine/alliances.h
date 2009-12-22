/**\file			alliances.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_alliance__
#define __h_alliance__

#include "includes.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single planet
class Alliance {
	public:
		bool parserCB( string sectionName, string subName, string value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "aggressiveness" ) {
				aggressiveness = static_cast<float>(atof( value.c_str() ) / 10.);
			} else PPA_MATCHES( "attackSize" ) {
				attackSize = (short int)atof( value.c_str() );
			}
			
			return true;
		}
		
	private:
		string name;
		short int attackSize;
		float aggressiveness;
};

// Class that holds list of all planets; manages them
class Alliances {
	public:
		static Alliances *Instance();
		
		bool Load( string& filename );

	protected:
		Alliances() {};
		Alliances( const Alliances & );
		Alliances& operator= (const Alliances&);

	private:
		static Alliances *pInstance;
		list<Alliance *> alliances;
};

#endif // __h_alliances__
