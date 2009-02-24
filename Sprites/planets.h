/*
 * Filename      : planets.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_planets__
#define __h_planets__

#include "includes.h"
#include "Sprites/sprite.h"
#include "Sprites/spritemanager.h"
#include "Utilities/coordinate.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

// Abstraction of a single planet
class cPlanet : public Sprite {
	public:
		bool parserCB( string sectionName, string subName, string value ) {
			PPA_MATCHES( "name" ) {
				name = value;
			} else PPA_MATCHES( "alliance" ) {
				alliance = value;
			} else PPA_MATCHES( "x" ) {
				Coordinate pos = GetWorldPosition();
				pos.SetX( (double)atof( value.c_str() ) );
				SetWorldPosition( pos );
			} else PPA_MATCHES( "y" ) {
				Coordinate pos = GetWorldPosition();
				pos.SetY( (double)atof( value.c_str() ) );
				SetWorldPosition( pos );
			} else PPA_MATCHES( "landable" ) {
				landable = (bool)atoi( value.c_str() );
			} else PPA_MATCHES( "traffic" ) {
				traffic = (short int)atoi( value.c_str() );
			} else PPA_MATCHES( "image" ) {
				Image *image = new Image( value );
				SetImage( image );
			}
			SetRadarColor(Color::Get(48, 160, 255));
			return true;
		}
		
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLANET );
		}
		
		void _dbg_PrintInfo( void ) {
			cout << "Planet: " << name << " at (" << GetWorldPosition() << ") under alliance " << alliance << " with landable option set to " << landable << " and average traffic count of " << traffic << " ships" << endl;
		}
		
		~cPlanet() {
			Image *image = GetImage();
			if( image )
				delete image; // planets delete their own images. not all Sprites do
		}
		
	private:
		string name;
		string alliance;
		bool landable;
		short int traffic;
		short int militiaSize;
		list<Sprite *> militia;
};

// Class that holds list of all planets; manages them
class Planets {
	public:
		static Planets *Instance();
		
		bool Load( string filename );
		
		void RegisterAll( SpriteManager *sprites );

	protected:
		Planets() {};
		Planets( const Planets & );
		Planets& operator= (const Planets&);

	private:
		static Planets *pInstance;
		list<cPlanet *> planets;
};

#endif // __h_planets__
