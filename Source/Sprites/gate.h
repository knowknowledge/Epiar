/*
 * Filename      : gate.h
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, March 16, 2010
 * Last Modified : Tuesday, March 16, 2010
 * Purpose       : Sprite SubClass for Warp Gates
 * Notes         : This
 */

#ifndef __H_EFFECT__
#define __H_EFFECT__

#include "Graphics/animation.h"
#include "Sprites/sprite.h"
#include "Graphics/image.h"
#include "includes.h"

class Gate : public Sprite {
	public:
		Gate(Coordinate pos); // Creates a Gate Top
		Gate(Coordinate pos, int topID); // Creates a Gate Bottom
		~Gate();
		virtual int GetDrawOrder( void ) {
			return( top? DRAW_ORDER_GATE_TOP : DRAW_ORDER_GATE_BOTTOM );
		}
	private:
		bool top;
		int partner;
};

#endif // __H_EFFECT__

