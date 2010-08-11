/**\file			spritemanager.cpp
 * \author			Chris Thielen (chris@epiar.net)
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
			//initialise the tick stuff - these should probably be set by an option somewhere, hardcode for now
SpriteManager::SpriteManager() :
	 tickCount (0)
	 , semiRegularPeriod (15)		//every 16 ticks we want to have updated the semi-regular distance quadrants
	 , fullUpdatePeriod (120)		//update the full quadrant map every 120 ticks
	 , numRegularBands (2)			//the regular (per-tick) updates are on this number of bands
	 , numSemiRegularBands (5)		//the semi-regular updates are on this number of bands - this SHOULD be easily divisible into semiRegularPeriod
{
	spritelist = new list<Sprite*>();
	spritelookup = new map<int,Sprite*>();


			//fill in the ticksToBandNum map based on the semiRegularPeriod and numSemiRegularBands
	int updateGap = semiRegularPeriod / numSemiRegularBands;

	for (int i = 0; i < numSemiRegularBands; i ++) {
		ticksToBandNum[(updateGap * i)] = numRegularBands + 1 + i;		//assign one of the semi-regular bands to a tick
	}
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
	list<QuadTree*> quadList;		//this will contain every quadrant that we will potentially want to update
	
			//if update-all is given then we update every quadrant
			//we do the same if tickCount == 0 even if update-all is not given
			// (in wave update mode, tickCount == 0 is when we want to update all quadrants)
	if( 0 != OPTION(int,"options/simulation/update-all") || tickCount == 0) {
			GetAllQuadrants(&quadList);			//need to get all of the quadrants in our map
	}
	else {				//wave update mode with tickCount != 0 -- update some quadrants
		Coordinate currentPoint (Camera::Instance()->GetFocusCoordinate());				//always update centered on where we're at

		quadList.push_back (GetQuadrant (currentPoint));		//we ALWAYS update the current quadrant

					//we also ALWAYS update the 'regular' bands
					//	the first band is at index 1 - index 0 would be the single quadrant in the middle
					//	when we get the list of quadrants back we splice them onto the end of our overall list
		for (int i = 1; i <= numRegularBands; i ++) {
			list<QuadTree*> tempBandList = GetQuadrantsInBand (currentPoint, i);
			quadList.splice (quadList.end(), tempBandList);
		}

					//now - we SOMETIMES update the semi-regular bands
					//   the ticks that each band is updated in is stored in the map
					//   so we get our semiRegular update modulus of the ticks and then check the map
					//    - the map has the tick index as the key and the band to update as the value
		int semiRegularTick = tickCount % semiRegularPeriod;
		map<int,int>::iterator findBand = ticksToBandNum.find (semiRegularTick);
		if (findBand != ticksToBandNum.end()) {		//found the key
			//cout << "tick = " << tickCount << ", semiRegularTick = " << semiRegularTick << ", band = " << findBand->second << endl;
			list<QuadTree*> tempBandList = GetQuadrantsInBand (currentPoint, findBand->second);
			quadList.splice (quadList.end(), tempBandList);
		}
		else {
				//no semi-regular bands to update at this tick, do nothing
		}
	}


	list<Sprite *> all_oob;

	list<QuadTree*>::iterator iter;
	for ( iter = quadList.begin(); iter != quadList.end(); ++iter ) {
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

	for ( iter = quadList.begin(); iter != quadList.end(); ++iter ) {
		(*iter)->ReBallance();
	}

	DeleteEmptyQuadrants();

			//update the tick count after all updates for this tick are done
	UpdateTickCount ();
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
	if( emptyTrees.size() ) {
		AdjustBoundaries();
	}
}

/**\brief Draws the current sprites
 */
void SpriteManager::Draw() {
	if( OPTION(int,"options/development/debug-quadtree") )
		GetQuadrant( Camera::Instance()->GetFocusCoordinate() )->Draw( GetQuadrantCenter( Camera::Instance()->GetFocusCoordinate() ) );

	list<Sprite *>::iterator i;
	list<Sprite*> *onscreen;
	float r = (Video::GetHalfHeight() < Video::GetHalfWidth() ? Video::GetHalfWidth() : Video::GetHalfHeight()) *V_SQRT2;
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

/**\brief Retrieves nearby QuadTrees in a square band at <bandIndex> quadrants distant from the coordinate
 * \param c Coordinate
 * \param bandIndex number of quadrants distant from c
 * \return std::list of QuadTree pointers.
 */
list<QuadTree*> SpriteManager::GetQuadrantsInBand ( Coordinate c, int bandIndex) {
	// The possibleQuadrants here are the quadrants that are in the square band
	//  at distance bandIndex from the coordinate
	// After we get the possible quadrants we prune them by making sure they exist
	//  (ie that something is in them)

	list<QuadTree*> nearbyQuadrants;
	set<Coordinate> possibleQuadrants;

			//note that the QUADRANTSIZE define is the
			//	distance from the middle to the edge of a quadrant
			//to get the square band of co-ordinates we have to
			//		- start at bottom left 
			//			loop over increasing Y (to get 'west' line)
			//			loop over increasing X (to get 'south' line)
			//		- start at top right
			//			loop over decreasing Y (to get 'east' line)
			//			loop over decreasing X (to get 'north' line)
			
	int edgeDistance = bandIndex * QUADRANTSIZE * 2;		//number of pixels from middle to the band
	Coordinate bottomLeft (c - Coordinate (edgeDistance, edgeDistance));
	Coordinate topLeft (c + Coordinate (-edgeDistance, edgeDistance));
	Coordinate topRight (c + Coordinate (edgeDistance, edgeDistance));
	Coordinate bottomRight (c + Coordinate (edgeDistance, -edgeDistance));

			//the 'full' length of one of the lines is (bandindex * 2) + 1
			//we don't need the +1 as we deal with the corners individually, separately
	int bandLength = (bandIndex * 2);

			//deal with the un-included corners first
			//we're using bottomLeft and topRight as the anchors, 
			// so topLeft and bottomRight are added here
	possibleQuadrants.insert (GetQuadrantCenter (topLeft));
	possibleQuadrants.insert (GetQuadrantCenter (bottomRight));
	for (int i = 0; i < bandLength; i ++) {
		int offset = ((QUADRANTSIZE * 2) * i);
		possibleQuadrants.insert (GetQuadrantCenter (bottomLeft + Coordinate (0, offset)));		//west
		possibleQuadrants.insert (GetQuadrantCenter (bottomLeft + Coordinate (offset, 0)));		//south
		possibleQuadrants.insert (GetQuadrantCenter (topRight - Coordinate (offset, 0)));		//north
		possibleQuadrants.insert (GetQuadrantCenter (topRight - Coordinate (0, offset)));		//east
	}

				//here we're checking to see if this possible quadrant is one of the existing quadrants
				// and if it is then we add its QuadTree to the vector we're returning
				// if it's not then there's nothing in it anyway so we don't care about it
	set<Coordinate>::iterator it;
	map<Coordinate,QuadTree*>::iterator iter;
	for(it = possibleQuadrants.begin(); it != possibleQuadrants.end(); ++it) {
		iter = trees.find(*it);
		if(iter != trees.end()) {
			nearbyQuadrants.push_back(iter->second);
		}
		
	}
	return nearbyQuadrants;
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
				//here we're checking to see if this possible quadrant is one of the existing quadrants
				// and if it is then we add its QuadTree to the vector we're returning
				//how about we try creating the quadrant if it does not already exist
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
	AdjustBoundaries();

	// Debug
	//cout<<"A Tree at "<<treeCenter<<" was created to contain "<<point<<". "<<trees.size()<<" Quadrants exist now."<<endl;

	return newTree;
}


void SpriteManager::GetBoundaries(float *_northEdge, float *_southEdge, float *_eastEdge, float *_westEdge)
{
	*_northEdge = northEdge;
	*_southEdge = southEdge;
	*_eastEdge  = eastEdge;
	*_westEdge  = westEdge;
}

void SpriteManager::AdjustBoundaries()
{
	Coordinate c;
	map<Coordinate,QuadTree*>::iterator iter;

	northEdge = southEdge = eastEdge = westEdge = 0;
	for ( iter = trees.begin(); iter != trees.end(); ++iter ) { 
		c = iter->first;
		if( c.GetY() > northEdge) northEdge = c.GetY();
		if( c.GetY() < southEdge) southEdge = c.GetY();
		if( c.GetX() > eastEdge)  eastEdge  = c.GetX();
		if( c.GetX() < westEdge)  westEdge  = c.GetX();
	}
}

void SpriteManager::Save() {
	map<Coordinate,QuadTree*>::iterator iter;
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL;/* node pointers */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "Sprites" );
    xmlDocSetRootElement(doc, root_node);

	for ( iter = trees.begin(); iter != trees.end(); ++iter ) { 
		xmlAddChild( root_node, iter->second->ToNode() );
	}

	xmlSaveFormatFileEnc( "Sprites.xml" , doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
	
}

void SpriteManager::UpdateTickCount ()
{
	tickCount ++;
			//we could do a modulus here but I think this will average out more efficient
	if (tickCount >= fullUpdatePeriod)
		tickCount -= fullUpdatePeriod;
}

		//this is shit and not very efficient, but std::transform doesn't work...
		// (if for some reason we got transform working, the idea would be to have
		//   a helper method to get map->second to pass as the 4th argument of transform
		//   with the third argument being a back_inserter into the list we want)
void SpriteManager::GetAllQuadrants (list<QuadTree*> *newList)
{
	map<Coordinate,QuadTree*>::iterator mapIter = trees.begin();
	while (mapIter != trees.end())
	{
		newList->push_back (mapIter->second);
		++ mapIter;
	}
}


