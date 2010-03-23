/**\file			player.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
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
		~Player();

		void Update( void );

		Color GetRadarColor( void ) { return Color::Get(0xFF,0xD7,0); }
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLAYER );
		}

	private:
		static Player *pInstance;
};

#endif // __H_PLAYER__
