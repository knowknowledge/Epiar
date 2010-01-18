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

#define QUADRANTSIZE 4096.0f

/**\class SpriteManager
 * \brief Mangers sprites. */

SpriteManager::SpriteManager() {
	spritelist = new list<Sprite*>();
	spritelookup = new map<int,Sprite*>();
}

SpriteManager *SpriteManager::pInstance = 0; // initialize pointer

SpriteManager *SpriteManager::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new SpriteManager; // create the sold instance
	}
	return( pInstance );
}

void SpriteManager::Add( Sprite *sprite ) {
	spritelist->push_back(sprite);
	spritelookup->insert(make_pair(sprite->GetID(),sprite));
	GetQuadrant( sprite->GetWorldPosition() )->Insert( sprite );
}

bool SpriteManager::DeleteSprite( Sprite *sprite ) {
	spritelist->remove(sprite);
	spritelookup->erase( sprite->GetID() );
	return ( GetQuadrant( sprite->GetWorldPosition() )->Delete( sprite ) );
}

bool SpriteManager::Delete( Sprite *sprite ) {
	spritesToDelete.push_back(sprite);
	return true;
}
void SpriteManager::Update() {
	// Update the sprites inside each quadrant
	// TODO: Update only the sprites that are in nearby Quadrants
	map<Coordinate,QuadTree*>::iterator iter;
	list<Sprite *> all_oob;
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) {
		iter->second->Update();
		list<Sprite *>* oob = iter->second->FixOutOfBounds();
		all_oob.splice(all_oob.end(), *oob );
		delete oob;
	}

	// Move sprites to adjacent Quadrants as they cross boundaries
	list<Sprite *>::iterator i;
	for( i = all_oob.begin(); i != all_oob.end(); ++i ) {
		GetQuadrant( (*i)->GetWorldPosition() )->Insert( *i );
	}

	//Delete all sprites queued to be deleted
	if (!spritesToDelete.empty()) {
		for( i = spritesToDelete.begin(); i != spritesToDelete.end(); ++i ) {
			DeleteSprite(*i);
		}
		spritesToDelete.clear();
	}

	// Delete QuadTrees that are empty
	// TODO: Delete QuadTrees that are far away from 
	list<QuadTree*> emptyTrees;
	// Collect empty trees
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) { 
		if ( iter->second->Count() == 0 ) {
			emptyTrees.push_back(iter->second);
		}
	}
	// Delete empty trees
	list<QuadTree*>::iterator emptyIter;
	for ( emptyIter = emptyTrees.begin(); emptyIter != emptyTrees.end(); ++emptyIter) {
			//cout<<"Deleting the empty tree at "<<(*emptyIter)->GetCenter()<<endl;
			delete (*emptyIter);
			trees.erase((*emptyIter)->GetCenter());
	}
}

void SpriteManager::Draw() {
	list<Sprite *>::iterator i;
	// TODO Have the drawing based directly on the screen dimensions
	list<Sprite*> *onscreen = new list<Sprite*>();
	onscreen = GetSpritesNear( Player::Instance()->GetWorldPosition(), 1000.0f);

	onscreen->sort(compareSpritePtrs);

	for( i = onscreen->begin(); i != onscreen->end(); ++i ) {
		(*i)->Draw();
	}
}

list<Sprite *> *SpriteManager::GetSprites() {
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
	map<Coordinate,QuadTree*>::iterator iter;
	list<Sprite*> *sprites = new list<Sprite*>();

	// The possibleQuadrants are those trees adjacent and within a radius r
	// Gather more trees when r is greater than the size of a quadrant
	set<Coordinate> possibleQuadrants;
	possibleQuadrants.insert( GetQuadrantCenter(c));
	float R = r;
	do{
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,-0)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-0,+R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+0,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,+0)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(-R,+R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,-R)));
		possibleQuadrants.insert( GetQuadrantCenter(c + Coordinate(+R,+R)));
		R/=2;
	} while(R>QUADRANTSIZE);

	// Search the possible quadrants
	set<Coordinate>::iterator it;
	for(it = possibleQuadrants.begin(); it != possibleQuadrants.end(); ++it) {
		iter = trees.find(*it);
		if(iter != trees.end() && iter->second->PossiblyNear(c,r)){
			list<Sprite *>* nearby = new list<Sprite*>;
			iter->second->GetSpritesNear(c,r,nearby);
			sprites->splice(sprites->end(), *nearby);
			delete nearby;
		}
	}

	// Sort sprites by their distance from the coordinate c
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}


Coordinate SpriteManager::GetQuadrantCenter(Coordinate point){
	// Figure out where the new Tree should go.
	// Quadrants are tiled adjacent to the central Quadrant centered at (0,0).
	double cx, cy;
	cx = float(floor( (point.GetX()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	cy = float(floor( (point.GetY()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	return Coordinate(cx,cy);
}

int SpriteManager::GetNumSprites() {
	int total = 0;
	map<Coordinate,QuadTree*>::iterator iter;
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) { 
		total += iter->second->Count();
	}
	assert( total == spritelist->size() );
	assert( total == spritelookup->size() );
	return total;
}

QuadTree* SpriteManager::GetQuadrant( Coordinate point ) {
	Coordinate treeCenter = GetQuadrantCenter(point);

	// Check in the known Quadrant
	map<Coordinate,QuadTree*>::iterator iter;
	iter = trees.find( treeCenter );
	if( iter != trees.end() ) {
		return iter->second;
	}

	// Create the new Tree and attach it to the universe
	QuadTree *newTree = new QuadTree(treeCenter, QUADRANTSIZE, 3);
	assert(treeCenter == newTree->GetCenter() );
	assert(newTree->Contains(point));
	trees.insert(make_pair(treeCenter, newTree));

	// Debug
	//cout<<"A Tree at "<<treeCenter<<" was created to contain "<<point<<". "<<trees.size()<<" Quadrants exist now."<<endl;

	return newTree;
}
