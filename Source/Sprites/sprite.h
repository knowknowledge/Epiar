/*
 * Filename      : sprite.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_sprite__
#define __h_sprite__

#include "Graphics/image.h"
#include "Graphics/video.h"
#include "Engine/models.h"
#include "Utilities/coordinate.h"

// With the draw order, higher numbers are drawn later (on top)
#define DRAW_ORDER_PLANET  0
#define DRAW_ORDER_SHIP    1
#define DRAW_ORDER_PLAYER  2

class Sprite {
	public:
		Sprite();
		virtual ~Sprite() {};
		
		Coordinate GetWorldPosition( void );
		void SetWorldPosition( Coordinate coord );
		
		virtual void Update( void );
		virtual void Draw( void );
		
		int GetID( void ) { return id; }

		float GetAngle( void ) {
			return( angle );
		}
		void SetAngle( float angle ) {
			this->angle = angle;
		}
		Coordinate GetMomentum( void ) {
			return momentum;
		}
		void SetMomentum( Coordinate momentum ) {
			this->momentum = momentum;
		}
		Coordinate GetAcceleration( void ) {
			return acceleration;
		}
		void SetImage( Image *image ) {
			this->image = image;
			this->radarSize = ( image->GetWidth() + image->GetHeight() ) /(2);
		}
		void SetRadarColor( Color col ){
			this->radarColor = col;
		}
		Image *GetImage( void ) {
			return image;
		}
		short int GetRadarSize( void ) { return radarSize; }
		virtual Color GetRadarColor( void ) { return radarColor; }
		
		virtual int GetDrawOrder( void ) = 0;
		
	private:
		static int sprite_ids;
		int id;
		Coordinate worldPosition;
		Coordinate momentum;
		Coordinate acceleration;
		Coordinate lastMomentum;
		Image *image;
		float angle;
		int radarSize;
		Color radarColor;
};

bool compareSpritePtrs(Sprite* a, Sprite* b);

// Creates a binary comparison object that can be passed to stl sort.
// Sprites will be sorted by distance from the point in ascending order.
struct compareSpriteDistFromPoint
	: public std::binary_function<Sprite*, Sprite*, bool>
{
	compareSpriteDistFromPoint(const Coordinate& c) : point(c) {}

	bool operator() (Sprite* a, Sprite* b) {
		return (point - a->GetWorldPosition()).GetMagnitude()
		     < (point - b->GetWorldPosition()).GetMagnitude() ;
	}

	Coordinate point;
};

#endif // __h_sprite__
