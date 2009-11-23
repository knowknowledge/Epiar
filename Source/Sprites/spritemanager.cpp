/**\file			spritemanager.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */

#include "includes.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"

/**\class SpriteManager
 * \brief Mangers sprites. */

SpriteManager::SpriteManager() {

}

void SpriteManager::Add( Sprite *sprite ) {
	if( sprite )
		sprites.push_back( sprite );
}

bool SpriteManager::Delete( Sprite *sprite ) {
	list<Sprite *>::iterator i = std::find( sprites.begin(), sprites.end(), sprite );

	if(i != sprites.end())
	{
		i = sprites.erase( i );
		return( true );
	}
	
	return( false );
}

void SpriteManager::Update() {
	list<Sprite *>::iterator i;

	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		(*i)->Update();
	}
}

void SpriteManager::Draw() {
	list<Sprite *>::iterator i;

	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		(*i)->Draw();
	}
}

// Reorders sprite list to ensure correct drawing order
void SpriteManager::Order() {
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

const list<Sprite *>& SpriteManager::GetSprites() {
	return( sprites );
}


