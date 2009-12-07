/*
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "Utilities/quadtree.h"

SpriteManager::SpriteManager() {
	spritelist = new list<Sprite*>();
	tree = new QuadTree(Coordinate(0,0), 65536.0f, 3);
}

void SpriteManager::Add( Sprite *sprite ) {
	cout<<"Adding Sprite at "<<(sprite->GetWorldPosition()).GetX()<<","<<(sprite->GetWorldPosition()).GetY()<<endl;
	spritelist->push_back(sprite);
	tree->Insert(sprite);
	cout<<"ADD COMPLETE\n\n";
}

bool SpriteManager::Delete( Sprite *sprite ) {
	spritelist->remove(sprite);
	return ( tree->Delete(sprite) );
}

void SpriteManager::Update() {
	spritelist->sort(compareSpritePtrs);
	tree->Update();
	list<Sprite *>* oob = tree->FixOutOfBounds();
	if(oob->size())
		Log::Error("%d ships went out of bounds",oob->size());
	delete oob;
}

void SpriteManager::Draw() {
	list<Sprite *>::iterator i;
	// TODO Have the drawing based directly on the screen dimensions
	list<Sprite *>* onscreen = tree->GetSpritesNear( Player::Instance()->GetWorldPosition(), 1000.0f);
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
	list<Sprite*> *sprites = tree->GetSpritesNear(c,r);
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}

