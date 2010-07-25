/**\file			sprite.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_sprite__
#define __h_sprite__

#include "Graphics/image.h"
#include "Graphics/video.h"
#include "Engine/models.h"
#include "Utilities/coordinate.h"

// With the draw order, higher numbers are drawn later (on top)
// By using non-overlapping bits we can bit mask during searches
#define DRAW_ORDER_PLANET              0x0001
#define DRAW_ORDER_GATE_BOTTOM         0x0002
#define DRAW_ORDER_WEAPON              0x0004
#define DRAW_ORDER_SHIP                0x0008
#define DRAW_ORDER_PLAYER              0x0010
#define DRAW_ORDER_GATE_TOP            0x0020
#define DRAW_ORDER_EFFECT              0x0040
#define DRAW_ORDER_ALL                 0xFFFF

class Sprite {
	public:
		Sprite();
		virtual ~Sprite() {};
		
		Coordinate GetWorldPosition( void ) const;
		void SetWorldPosition( Coordinate coord );
		
		virtual void Update( void );
		virtual void Draw( void );
		
		int GetID( void ) { return id; }

		float GetAngle( void ) const {
			return( angle );
		}
		void SetAngle( float angle ) {
			this->angle = angle;
		}
		Coordinate GetMomentum( void ) const {
			return momentum;
		}
		void SetMomentum( Coordinate momentum ) {
			this->momentum = momentum;
		}
		Coordinate GetAcceleration( void ) const {
			return acceleration;
		}
		void SetImage( Image *image ) {
			assert(image);
			this->image = image;
			this->radarSize = ( image->GetWidth() + image->GetHeight() ) /(2);
		}
		void SetRadarColor( Color col ){
			this->radarColor = col;
		}
		Image *GetImage( void ) const {
			return image;
		}
		int GetRadarSize( void ) { return radarSize; }
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
		return (point - a->GetWorldPosition()).GetMagnitudeSquared()
		     < (point - b->GetWorldPosition()).GetMagnitudeSquared() ;
	}

	Coordinate point;
};

#endif // __h_sprite__
