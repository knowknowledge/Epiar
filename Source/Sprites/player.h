/*
 * Filename      : player.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Wednesday, July 5, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Header for player class
 * Notes         :
 */

#ifndef __H_PLAYER__
#define __H_PLAYER__

#include "includes.h"
#include "Sprites/ship.h"

class Player : public Ship {
	public:
		static Player *Instance();

	protected:
		Player();
		Player( const Player & );
		Player& operator= (const Player&);

		void Update( void );

		Color GetRadarColor( void ) { return Color::Get(0xFF,0xD7,0); }
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLAYER );
		}

	private:
		static Player *pInstance;
};

#endif // __H_PLAYER__
