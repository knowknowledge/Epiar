/**\file			spritemanager.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */

#include "includes.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "Utilities/quadtree.h"

/**\class SpriteManager
 * \brief Mangers sprites. */

SpriteManager::SpriteManager() {
	spritelist = new list<Sprite*>();
	tree = new QuadTree(Coordinate(0,0), 65536.0f, 3);
}

SpriteManager *SpriteManager::pInstance = 0; // initialize pointer

SpriteManager *SpriteManager::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new SpriteManager; // create the sold instance
	}
	return( pInstance );
}

void SpriteManager::Add( Sprite *sprite ) {
	//cout<<"Adding Sprite at "<<(sprite->GetWorldPosition()).GetX()<<","<<(sprite->GetWorldPosition()).GetY()<<endl;
	spritelist->push_back(sprite);
	tree->Insert(sprite);
	//cout<<"ADD COMPLETE\n\n";
}

bool SpriteManager::DeleteSprite( Sprite *sprite ) {
	spritelist->remove(sprite);
	return ( tree->Delete(sprite) );
}

bool SpriteManager::Delete( Sprite *sprite ) {
	spritesToDelete.push_back(sprite);
	return true;
}
void SpriteManager::Update() {
	spritelist->sort(compareSpritePtrs);
	tree->Update();
	list<Sprite *>* oob = tree->FixOutOfBounds();
	if(oob->size())
		Log::Error("%d ships went out of bounds",oob->size());
	delete oob;

	//Delete all sprites queued to be deleted
	list<Sprite *>::iterator i;
	if (!spritesToDelete.empty()) {
		for( i = spritesToDelete.begin(); i != spritesToDelete.end(); ++i ) {
			DeleteSprite(*i);
		}
		spritesToDelete.clear();
	}
}

void SpriteManager::Draw() {
	list<Sprite *>::iterator i;
	// TODO Have the drawing based directly on the screen dimensions
	list<Sprite*> *onscreen = new list<Sprite*>();
	tree->GetSpritesNear( Player::Instance()->GetWorldPosition(), 1000.0f, onscreen);
	//cout<<onscreen->size()<<" sprites are in range.\n";

	onscreen->sort(compareSpritePtrs);

	for( i = onscreen->begin(); i != onscreen->end(); ++i ) {
		(*i)->Draw();
	}
}

list<Sprite *> *SpriteManager::GetSprites() {
	//list<Sprite*> *sprites = tree->GetSprites();
	return( spritelist );
}

list<Sprite*> *SpriteManager::GetSpritesNear(Coordinate c, float r) {
	list<Sprite*> *sprites = new list<Sprite*>();
	tree->GetSpritesNear(c,r,sprites);
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}

