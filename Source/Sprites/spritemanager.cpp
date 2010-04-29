/**\file			spritemanager.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/spritemanager.h"
#include "Utilities/quadtree.h"
#include "Utilities/camera.h"


/**\class SpriteManager
 * \brief Mangers sprites. */

/**\brief Constructs a new sprite manager.
 */
SpriteManager::SpriteManager() {
	spritelist = new list<Sprite*>();
	spritelookup = new map<int,Sprite*>();
}

SpriteManager *SpriteManager::pInstance = 0; // initialize pointer

/**\brief Retrieves or creates new SpriteManager instance.
 * \return Pointer to SpriteManager
 */
SpriteManager *SpriteManager::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new SpriteManager; // create the sold instance
	}
	return( pInstance );
}

/**\brief Adds a sprite to the manager.
 * \param sprite Pointer to the sprite
 */
void SpriteManager::Add( Sprite *sprite ) {
	spritelist->push_back(sprite);
	spritelookup->insert(make_pair(sprite->GetID(),sprite));
	GetQuadrant( sprite->GetWorldPosition() )->Insert( sprite );
}

/**\brief Deletes a sprite from the manager (Internal use).
 * \param sprite Pointer to the sprite
 * \details
 * This performs the actual deletion.
 */
bool SpriteManager::DeleteSprite( Sprite *sprite ) {
	spritelist->remove(sprite);
	spritelookup->erase( sprite->GetID() );
	if( sprite==Camera::Instance()->GetFocus() ) {
		Camera::Instance()->Focus(NULL);
	}
	GetQuadrant( sprite->GetWorldPosition() )->Delete( sprite );
	// Delete the sprite itself unless it is a Planet or Player.
	// Planets and Players are special sprites since they are Components and get saved.
	if( !(sprite->GetDrawOrder() & (DRAW_ORDER_PLAYER | DRAW_ORDER_PLANET)) ) {
		delete sprite;
	}
	return true;
}

/**\brief Deletes a sprite.
 * \param sprite Pointer to the sprite object
 * \details
 * This just queues the sprite up to be deleted.
 */
bool SpriteManager::Delete( Sprite *sprite ) {
	spritesToDelete.push_back(sprite);
	return true;
}

/**\brief SpriteManager update function.
 */
void SpriteManager::Update() {
	// Update the sprites inside each quadrant
	// TODO: Update only the sprites that are in nearby Quadrants
	list<Sprite *> all_oob;
	list<QuadTree*> nearby = GetQuadrantsNear( Camera::Instance()->GetFocusCoordinate(), QUADRANTSIZE*4 );
	list<QuadTree*>::iterator iter;
	for ( iter = nearby.begin(); iter != nearby.end(); ++iter ) {
		(*iter)->Update();
		list<Sprite *>* oob = (*iter)->FixOutOfBounds();
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
		spritesToDelete.unique();
		for( i = spritesToDelete.begin(); i != spritesToDelete.end(); ++i ) {
			DeleteSprite(*i);
		}
		spritesToDelete.clear();
	}

	for ( iter = nearby.begin(); iter != nearby.end(); ++iter ) {
		(*iter)->ReBallance();
	}

	DeleteEmptyQuadrants();
}

/**\brief Deletes empty QuadTrees (Internal use)
 */
void SpriteManager::DeleteEmptyQuadrants() {
	map<Coordinate,QuadTree*>::iterator iter;
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
		trees.erase((*emptyIter)->GetCenter());
		delete (*emptyIter);
	}
}

/**\brief Draws the current sprites
 */
void SpriteManager::Draw() {
	if( OPTION(int,"options/development/debug-quadtree") )
		GetQuadrant( Camera::Instance()->GetFocusCoordinate() )->Draw( GetQuadrantCenter( Camera::Instance()->GetFocusCoordinate() ) );

	list<Sprite *>::iterator i;
	list<Sprite*> *onscreen;
	float r = (Video::GetHalfHeight() < Video::GetHalfWidth() ? Video::GetHalfWidth() : Video::GetHalfHeight()) *1.42f;
	onscreen = GetSpritesNear( Camera::Instance()->GetFocusCoordinate(), r, DRAW_ORDER_ALL);

	onscreen->sort(compareSpritePtrs);

	for( i = onscreen->begin(); i != onscreen->end(); ++i ) {
		(*i)->Draw();
	}
	delete onscreen;
}

/**\brief Retrieves a list of the current sprites.
 * \return std::list of Sprite pointers.
 */
list<Sprite *> *SpriteManager::GetSprites(int type) {
	list<Sprite *>::iterator i;
	list<Sprite *> *filtered;
	if( type==DRAW_ORDER_ALL ){
		filtered = new list<Sprite*>(*spritelist);
	} else {
		filtered = new list<Sprite*>();
		// Collect only the Sprites of this type
		for( i = spritelist->begin(); i != spritelist->end(); ++i ) {
			if( (*i)->GetDrawOrder() & type){
				filtered->push_back( (*i) );
			}
		}
	}
	return( filtered);
}

/**\brief Queries for sprite by the ID
 * \param id Identification of the sprite.
 */
Sprite *SpriteManager::GetSpriteByID(int id) {
	map<int,Sprite*>::iterator val = spritelookup->find( id );
	if( val != spritelookup->end() ){
		return val->second;
	}
	return NULL;
}

/**\brief Retrieves nearby QuadTrees
 * \param c Coordinate
 * \param r Radius
 * \return std::list of QuadTree pointers.
 */
list<QuadTree*> SpriteManager::GetQuadrantsNear( Coordinate c, float r) {
	// The possibleQuadrants are those trees adjacent and within a radius r
	// Gather more trees when r is greater than the size of a quadrant
	map<Coordinate,QuadTree*>::iterator iter;
	list<QuadTree*> nearbyQuadrants;
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
	set<Coordinate>::iterator it;
	for(it = possibleQuadrants.begin(); it != possibleQuadrants.end(); ++it) {
		iter = trees.find(*it);
		if(iter != trees.end() && iter->second->PossiblyNear(c,r)){
			nearbyQuadrants.push_back(iter->second);
		}
	}
	return nearbyQuadrants;
}

/**\brief Returns a list of sprites that are near coordinate.
 * \param c Coordinate
 * \param r Radius
 * \return std::list of Sprite pointers.
 */
list<Sprite*> *SpriteManager::GetSpritesNear(Coordinate c, float r, int type) {
	list<Sprite*> *sprites = new list<Sprite*>();
	
	// Search the possible quadrants
	list<QuadTree*> nearbyQuadrants = GetQuadrantsNear(c,r);
	list<QuadTree*>::iterator it;
	for(it = nearbyQuadrants.begin(); it != nearbyQuadrants.end(); ++it) {
			list<Sprite *>* nearby = new list<Sprite*>;
			(*it)->GetSpritesNear(c,r,nearby,type);
			sprites->splice(sprites->end(), *nearby);
			delete nearby;
	}

	// Sort sprites by their distance from the coordinate c
	sprites->sort(compareSpriteDistFromPoint(c));
	return( sprites );
}

Sprite* SpriteManager::GetNearestSprite(Sprite* obj, float r, int type) {
	float tmpdist;
	Sprite* closest=NULL;
	Sprite* possible=NULL;
	if(obj==NULL)
		return (Sprite*)NULL;
	list<QuadTree*> nearbyQuadrants = GetQuadrantsNear(obj->GetWorldPosition(),r);
	list<QuadTree*>::iterator it;
	for(it = nearbyQuadrants.begin(); it != nearbyQuadrants.end(); ++it) {
		possible = (*it)->GetNearestSprite(obj,r, type);
		if(possible!=NULL) {
			tmpdist = (obj->GetWorldPosition()-possible->GetWorldPosition()).GetMagnitude();
			if(tmpdist<r) {
				r = tmpdist;
				closest = possible;
			}
		}
	}
	return closest;
}

/**\brief Returns QuadTree center.
 * \param point Coordinate
 * \return Coordinate of centerpointer
 */
Coordinate SpriteManager::GetQuadrantCenter(Coordinate point){
	// Figure out where the new Tree should go.
	// Quadrants are tiled adjacent to the central Quadrant centered at (0,0).
	double cx, cy;
	cx = float(floor( (point.GetX()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	cy = float(floor( (point.GetY()+QUADRANTSIZE)/(QUADRANTSIZE*2.0f)) * QUADRANTSIZE*2.f);
	return Coordinate(cx,cy);
}

/**\brief Gets the number of Sprites in the SpriteManager
 */
int SpriteManager::GetNumSprites() {
	unsigned int total = 0;
	map<Coordinate,QuadTree*>::iterator iter;
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) { 
		total += iter->second->Count();
	}
	assert( total == spritelist->size() );
	assert( total == spritelookup->size() );
	return total;
}

/**\brief Returns QuadTree at Coordinate
 * \param point Coordinate
 */
QuadTree* SpriteManager::GetQuadrant( Coordinate point ) {
	Coordinate treeCenter = GetQuadrantCenter(point);

	// Check in the known Quadrant
	map<Coordinate,QuadTree*>::iterator iter;
	iter = trees.find( treeCenter );
	if( iter != trees.end() ) {
		return iter->second;
	}

	// Create the new Tree and attach it to the universe
	QuadTree *newTree = new QuadTree(treeCenter, QUADRANTSIZE);
	assert(treeCenter == newTree->GetCenter() );
	assert(newTree->Contains(point));
	trees.insert(make_pair(treeCenter, newTree));

	// Debug
	//cout<<"A Tree at "<<treeCenter<<" was created to contain "<<point<<". "<<trees.size()<<" Quadrants exist now."<<endl;

	return newTree;
}
