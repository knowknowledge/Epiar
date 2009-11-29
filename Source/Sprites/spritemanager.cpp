/*
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"

SpriteManager::SpriteManager() {
    tree = new QuadTree(Coordinate(0,0), 1<<16, 3);
}

void SpriteManager::Add( Sprite *sprite ) {
	cout<<"Adding Sprite at "<<(sprite->GetWorldPosition()).GetX()<<","<<(sprite->GetWorldPosition()).GetY()<<endl;
	tree->Insert(sprite);
	cout<<"ADD COMPLETE\n\n";
}

bool SpriteManager::Delete( Sprite *sprite ) {
	return tree->Delete(sprite);
}

void SpriteManager::Update() {
	tree->Update();
}

void SpriteManager::Draw() {
	tree->Draw();
}

list<Sprite *> *SpriteManager::GetSprites() {
	return( tree->GetSprites() );
}


// Reorders sprite list to ensure correct drawing order
void SpriteManager::Order() {
	list<Sprite *>::iterator i;
	list<int> layerIDs;
	list<Sprite *> planets;
	list<Sprite *> ships;
	list<Sprite *> newSprites;
	list<Sprite *> *sprites = tree->GetSprites();

	// break our master list into its type lists
	for( i = sprites->begin(); i != sprites->end(); ++i ) {
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
	
	//sprites = newSprites;
}
