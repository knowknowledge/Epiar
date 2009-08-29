/*
 * Filename      : ship.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, July 7, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Header for ship class, child of sprite class, Sprite
 * Notes         :
 */

#ifndef __H_SHIP__
#define __H_SHIP__

#include "Engine/models.h"
#include "Sprites/sprite.h"

enum Direction {_LEFT=-1,_STRAIGHT,_RIGHT};

class Ship : public Sprite {
	public:
		Ship();
		
		bool SetModel( Model *model );
		bool SetFlareAnimation( string filename );
		Sprite *GetSprite();
		
		void Update( void );
		void Rotate( int direction );
		void Accelerate( void );
		void Draw( void );
		
		/* Status functions */
        Direction directionTowards(Coordinate c);
        Direction directionTowards(float angle);
		float getHullIntegrityPct();
		
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_SHIP );
		}

	private:
		Model *model;
		Animation *flareAnimation;
		struct {
			/* Related to ship's condition */
			short int hullEnergyAbsorbed; /* hull health - once the hull absorbs to much energy, your ship blows apart, hooray! :) */
			
			/* Flags */
			bool isAccelerating; // cleared by update, set by accelerate (so it's always updated twice a loop)
		} status;
};

#endif // __H_SHIP__
