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
	spritelookup = new map<int,Sprite*>();
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
	spritelookup->insert(make_pair(sprite->GetID(),sprite));
	tree->Insert(sprite);
	//cout<<"ADD COMPLETE\n\n";
}

bool SpriteManager::DeleteSprite( Sprite *sprite ) {
	spritelist->remove(sprite);
	spritelookup->erase( sprite->GetID() );
	return ( tree->Delete(sprite) );
}

bool SpriteManager::Delete( Sprite *sprite ) {
	spritesToDelete.push_back(sprite);
	return true;
}
void SpriteManager::Update() {
	list<Sprite *>::iterator i;

	spritelist->sort(compareSpritePtrs);
	tree->Update();
	list<Sprite *>* oob = tree->FixOutOfBounds();
	if(oob->size()){
		// Gracefully handle Out of Bounds Sprites
		for( i = oob->begin(); i != oob->end(); ++i ) {
			Log::Error("\tSprite #%d went out of bounds at (%lf,%lf)",
					(*i)->GetID(),
					(*i)->GetWorldPosition().GetX(),
					(*i)->GetWorldPosition().GetY() );
			Delete(*i);
		}
	}
	delete oob;

	//Delete all sprites queued to be deleted
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

Sprite *SpriteManager::GetSpriteByID(int id) {
	map<int,Sprite*>::iterator val = spritelookup->find( id );
	if( val != spritelookup->end() ){
		return val->second;
	}
	return NULL;
}

list<Sprite*> *SpriteManager::GetSpritesNear(Coordinate c, float r) {
	list<Sprite*> *sprites = new list<Sprite*>();
	tree->GetSpritesNear(c,r,sprites);
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}

