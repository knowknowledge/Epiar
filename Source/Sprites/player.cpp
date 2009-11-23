/**\file			player.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */

#include "includes.h"
#include "Sprites/player.h"

/**\class Player
 * \brief Main player-specific functions and handle. */

Player *Player::pInstance = 0;

Player *Player::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Player; // create the sold instance
	}

	return( pInstance );
}

Player::Player() {

}

