/*
 * Filename      : player.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Wednesday, July 5, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Main player-specific functions and handle
 * Notes         :
 */

#include "includes.h"
#include "Sprites/player.h"

Player *Player::pInstance = 0;

Player *Player::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Player; // create the sold instance
	}

	return( pInstance );
}

Player::Player() {
	this->SetRadarColor( Color::Get(0xFF,0xD7,0) );

}

void Player::Update( void ) {
	Ship::Update();
	if( getHullIntegrityPct() <= 0) {
		Log::Message( "You have been destroyed..." );
	}
}

