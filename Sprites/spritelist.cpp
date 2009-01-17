/*
 * Filename      : spritelist.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Sprites/player.h"
#include "Sprites/spritelist.h"

SpriteList::SpriteList() {

}

void SpriteList::Add( Sprite *sprite ) {
	if( sprite )
		sprites.push_back( sprite );
}

bool SpriteList::Delete( Sprite *sprite ) {
	list<Sprite *>::iterator i;

	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		if( (*i) == sprite ) {
			// Found sprite
			sprites.erase( i );
			
			return( true );
		}
	}
	
	return( false );
}

void SpriteList::Update( void ) {
	list<Sprite *>::iterator i;

	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		(*i)->Update();
	}
}

void SpriteList::Draw( void ) {
	list<Sprite *>::iterator i;

	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		(*i)->Draw();
	}
}

// Reorders sprite list to ensure correct drawing order
void SpriteList::Order( void ) {
	list<Sprite *>::iterator i;
	list<int> layerIDs;
	list<Sprite *> planets;
	list<Sprite *> ships;
	list<Sprite *> newSprites;

	// break our master list into its type lists
	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		int order = (*i)->GetDrawOrder();
		
		switch( order ) {
			case DRAW_ORDER_PLANET:
				planets.push_back( (*i) );
				break;
			case DRAW_ORDER_SHIP:
				ships.push_back( (*i) );
				break;
			default:
				break;
		}
	}
	
	// insert the various type lists in the correct order
	// planets
	for( i = planets.begin(); i != planets.end(); ++i ) {
		newSprites.push_back( (*i) );
	}

	// ships
	for( i = ships.begin(); i != ships.end(); ++i ) {
		newSprites.push_back( (*i) );
	}
	
	// the player
	Sprite *player = Player::Instance()->GetSprite();
	newSprites.push_back( player );
	
	sprites = newSprites;
}

list<Sprite *>::iterator SpriteList::Enumerate() {
	return( sprites.begin() );
}

list<Sprite *>::iterator SpriteList::Enumerate( list<Sprite *>::iterator &i ) {
	i++;
	
	if( i == sprites.end() )
		return( (list<Sprite *>::iterator)NULL );
	
	return( i );
}
