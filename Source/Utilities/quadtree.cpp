/**\file			quadtree.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Tuesday, November 24 2009
 * \date			Modified: Tuesday, November 24 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/quadtree.h"
#include "Graphics/video.h"

const char* PositionNames[4] = { "UPPER_LEFT", "UPPER_RIGHT", "LOWER_LEFT", "LOWER_RIGHT"};

/**\class QuadTree
 * \brief The QuadTree is a recursive datastructure that makes it possible to query for 2 Dimensional points and areas.
 *
 * Like a more traditional tree, each part of the QuadTree is either a Leaf or a Node.
 * Each Node has 4 branches: "UPPER_LEFT", "UPPER_RIGHT", "LOWER_LEFT", and "LOWER_RIGHT".
 * All of the objects are stored in the Leaves.  (In our case the QuadTree stores Sprite pointers.)
 * Once a Leaf contains a certain threshold of Objects, it transforms into a Node and push it's objects into new leaves.
 *
 * There are two important QuadTree uses:
 *   -# Get the Sprite that is nearest a point.
 *   -# Get all the Sprites that are within a given radius of a point.
 *
 * Here is an example QuadTree.
 * Notice that it split twice.
 * \verbatim
   +---+----+--------+
   | o | ooo|        |
   +---+----+        |
   |o  |    |        |
   +---+----+--------+
   |        |        |
   |        |        |
   |        |    o   |
   +--------+--------+
   \endverbatim
 *
 * \see GetNearestSprite
 * \see GetSpritesNear
 *
 */

/** \brief Constructor
 * By default there are no instantiated subtrees.
 */

QuadTree::QuadTree(Coordinate _center, float _radius){
	// cout<<"New QT at "<<_center<<" has R="<<_radius<<endl;
	assert(_radius>MIN_QUAD_SIZE/2);
	for(int t=0;t<4;t++){
		subtrees[t] = NULL;
	}
	this->objects = new list<Sprite*>();
	this->radius = _radius;
	this->center = _center;
	this->objectcount = 0;
	this->isLeaf = true;
	this->isDirty = false;
}

/** \brief Destructor
 */

QuadTree::~QuadTree(){
	this->objects->clear();
	delete objects;
	// Delete the Subtrees (Node)
	for(int t=0;t<4;t++){
		if(NULL != (subtrees[t])){
			delete subtrees[t];
			this->subtrees[t] = NULL;
		}
	}
}

/** \brief The number of Sprites within this QuadTree.
 *
 */

unsigned int QuadTree::Count(){
	return objectcount;
	/*
	if(isLeaf){
		return objects->size();
	} else {
		int total=0;
		for(int t=0;t<4;t++){
			if(NULL != subtrees[t])
				total += subtrees[t]->Count();
		}
		return total;
	}
	*/
}

/** \brief Check if a point is inside this QuadTree.
 * \arg point The point that we want to check.
 * \returns True if the point is inside the QuadTree.
 */

bool QuadTree::Contains(Coordinate point){
	bool insideLeftBorder = (center.GetX()-radius) <= point.GetX();
	bool insideRightBorder = (center.GetX()+radius) >= point.GetX();
	bool insideTopBorder = (center.GetY()+radius) >= point.GetY();
	bool insideBottomBorder = (center.GetY()-radius) <= point.GetY();
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

/** \brief Add a Sprite to this Tree
 *
 * The Tree is marked as dirty if the Sprite is added to a Leaf.
 *
 * \arg obj The Sprite to delete.
 * \returns TRUE if the Sprite is found and successfully removed.
 */

void QuadTree::Insert(Sprite *obj){
	if(! isLeaf ){ // Node
		InsertSubTree(obj);
	} else { // Leaf
		objects->push_back(obj);
		// An over Full Leaf should become a Node
		isDirty=true;
	}
	objectcount++;
}

/** \brief Remove a Sprite from this Tree
 *
 * The Tree is marked as dirty if the Sprite if successfully found and removed.
 *
 * \arg obj The Sprite to delete.
 * \returns TRUE if the Sprite is found and successfully removed.
 */

bool QuadTree::Delete(Sprite* obj){
	if(0 == this->Count())
		return( false ); // No objects to delete.

	if(!Contains(obj->GetWorldPosition()))
		return( false ); // Out of bounds, nothing to delete.

	if(!isLeaf){ // Node
		QuadTree* dest = subtrees[SubTreeThatContains( obj->GetWorldPosition() )];
		if(dest==NULL)
			return( false ); // That branch is empty, nothing to delete.
		if( dest->Delete(obj) ){
			isDirty=true;
			objectcount--;
			return( true ); // Found that object.
		} else {
			return( false ); // Didn't find that object.
		}
	} else { // Leaf
		list<Sprite *>::iterator i = std::find( objects->begin(), objects->end(), obj);
		if(i != objects->end())
		{
			i = objects->erase( i );
			// Note that leaves don't ReBallance on delete.
			objectcount--;
			return( true );
		} else {
			return( false );
		}
	}
}

/** \brief Get all Sprites in this QuadTree
 *
 * \returns A list of Sprite pointers.
 */

list<Sprite *> *QuadTree::GetSprites() {
	if(!isLeaf){ // Node
		list<Sprite*> *other;
		list<Sprite*> *full = new list<Sprite*>();
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				other = subtrees[t]->GetSprites();
				full->splice(full->end(), *other );
				delete other;
			}
		}
		return full;
	} else { // Leaf
		return new list<Sprite*>(*objects);
	}
}

/** \brief Get all Sprites within a certain radius.
 *
 * \arg point The center of the search radius.
 * \arg distance The maximum search radius.
 * \arg nearby [out] The list of all Sprites found within the search radius.
 * \arg type A DRAW_ORDER mask used to filter for desired Sprite types.
 *
 * The nearby list is passed down the recursive call-stack rather than returned at by each call.
 * This limits the malloc calls.
 *
 * \returns nothing.
 */

void QuadTree::GetSpritesNear(Coordinate point, float distance, list<Sprite*> *nearby, int type){
	// The Maximum range is when the center and point are on a 45 degree angle.
	//   Root-2 of the radius + the distance
	const float maxrange = V_SQRT2*radius + distance;

	// If the distance to the point is greater than the max range,
	//   then no collisions are possible
	if( (point-center).GetMagnitudeSquared() > maxrange*maxrange){
		return;
	}

	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				subtrees[t]->GetSpritesNear(point,distance,nearby,type);
			}
		}
	} else { // Leaf
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			if( ((*i)->GetDrawOrder() & type) == 0) continue;
			if( (point - (*i)->GetWorldPosition()).GetMagnitudeSquared() < distance*distance + (*i)->GetRadarSize()*(*i)->GetRadarSize() ) {
				nearby->push_back( *i);
			}
		}
	}
}

/**\brief Find the Sprite that is closest to a known point.
 *
 * \arg obj The Sprite at the center of the search radius.
 *          This Sprite is ignored while searching.
 *          Otherwise, a Sprite wouldn't be able to find other Sprites near them, they would always find themselves. )
 * \arg distance A Max radius to use while searching.
 *      	     Sprites outside of this radius are ignored.
 *               Each recurse of this function will use the smallest new radius.
 * \arg type A DRAW_ORDER mask used to filter for desired Sprite types.
 *
 * \returns A pointer to the Sprite nearest the obj Sprite, within a certain distance, and of the correct type.
 */

Sprite* QuadTree::GetNearestSprite(Sprite* obj, float distance, int type){
	// The Maximum range is when the center and point are on a 45 degree angle.
	//   Root-2 of the radius + the distance
	const float maxrange = V_SQRT2*radius + distance;
	Sprite* closest=NULL;
	Sprite* possible=NULL;
	float tmpdist;
	float mindist=distance;
	Coordinate point = obj->GetWorldPosition();

	// If the distance to the point is greater than the max range,
	//   then no collisions are possible
	if( (point-center).GetMagnitudeSquared() > maxrange*maxrange){
		return NULL;
	}
	if(!isLeaf){ // Node
		// Nodes work in linear space
		mindist=distance;
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				possible = subtrees[t]->GetNearestSprite(obj,distance,type);
				if(possible==NULL) continue; // This tree short circuited
				tmpdist = (point-possible->GetWorldPosition()).GetMagnitude();
				if( tmpdist < mindist ){
					mindist = tmpdist;
					closest = possible;
				}
			}
		}
	} else { // Leaf
		// Leaves work in square space
		mindist=distance*distance;
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			if(((*i) == obj) || (((*i)->GetDrawOrder() & type) == 0))
				continue;
			tmpdist = (point - (*i)->GetWorldPosition()).GetMagnitudeSquared();
			if( tmpdist < mindist ) {
				mindist = tmpdist;
				closest = (*i);
			}
		}
	}
	return closest;
}

/** \brief  Check and remove any Sprites are not contained in this QuadTree.
 *
 * This check for Sprites that are out of bounds of the children subtrees.
 * Any Sprites that can be re-inserted into this QuadTree will be re-inserted.
 * Sprites that are outside of this this QuadTree are removed and forgotten.
 *
 * \returns List of all Sprites outside of this QuadTree.
 */

list<Sprite*> *QuadTree::FixOutOfBounds(){
	list<Sprite*>::iterator i;
	list<Sprite*> *other;
	list<Sprite*> *stillinside = new list<Sprite*>();
	list<Sprite*> *outofbounds = new list<Sprite*>();
	if(!isLeaf){ // Node
		// Collect out of bound sprites from sub-trees
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				other = subtrees[t]->FixOutOfBounds();
				outofbounds->splice(outofbounds->end(),*other);
				delete other;
			}
		}
		objectcount-= outofbounds->size();
		if(outofbounds->size()) isDirty=true;
		// Insert any sprites that are inside of this Tree
		for( i = outofbounds->begin(); i != outofbounds->end(); ++i ) {
			if( this->Contains((*i)->GetWorldPosition()) ) {
				stillinside->push_back(*i);
			}
		}
		for( i = stillinside->begin(); i != stillinside->end(); ++i ) {
			this->Insert(*i);
			outofbounds->remove(*i);
		}
	} else { // Leaf
		// Collect and forget any out of bound sprites from object list
		for( i = objects->begin(); i != objects->end(); ++i ) {
			if(! this->Contains((*i)->GetWorldPosition()) ) {
				outofbounds->push_back( *i );
			}
		}
		for( i = outofbounds->begin(); i != outofbounds->end(); ++i ) {
			objects->remove(*i);
		}
		objectcount-= outofbounds->size();
	}
	delete stillinside;
	if(outofbounds->size()) isDirty=true;
	// Return any sprites that couldn't be re-inserted
	return outofbounds;
}

/** \brief Update all Sprites in this QuadTree
 */

void QuadTree::Update(){
	list<Sprite*>::iterator i;
	// Update all internal sprites
	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				subtrees[t]->Update();
			}
		}
	} else { // Leaf
		for( i = objects->begin(); i != objects->end(); ++i ) {
			(*i)->Update();
		}
	}
}

/**  Draw the QuadTree
 *
 * /arg root The center coordinate for the root of this QuadTree.
 *
 * (Useful for debugging.)
 */

void QuadTree::Draw(Coordinate root){
	// The QuadTree is scaled so that it always fits on the screen.
	float scale = (Video::GetHalfHeight() > Video::GetHalfWidth() ?
		static_cast<float>(Video::GetHalfWidth()) : static_cast<float>(Video::GetHalfHeight()) -5);
	float r = scale* radius / QUADRANTSIZE;
	float x = (scale* static_cast<float>((center-root).GetX()) / QUADRANTSIZE)
		+ static_cast<float>(Video::GetHalfWidth())  -r;
	float y = (scale* static_cast<float>((center-root).GetY()) / QUADRANTSIZE)
		+ static_cast<float>(Video::GetHalfHeight()) -r;
	Video::DrawRect( static_cast<int>(x),static_cast<int>(y),
		static_cast<int>(2*r),static_cast<int>(2*r), 0,255.f,0.f, .1f);

	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])) subtrees[t]->Draw(root);
		}
	} else { // Leaf
		list<Sprite *>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			Coordinate pos = (*i)->GetWorldPosition() - root;
			int posx = static_cast<int>((scale* (float)pos.GetX() / QUADRANTSIZE) + (float)Video::GetHalfWidth());
			int posy = static_cast<int>((scale* (float)pos.GetY() / QUADRANTSIZE) + (float)Video::GetHalfHeight());
			Color col = (*i)->GetRadarColor();
			// The 17 is here because it looks nice.  I can't explain why.
			Video::DrawCircle( posx, posy, static_cast<int>(17.f*(*i)->GetRadarSize()/scale),2, col.r,col.g,col.b );
		}
	}
}

/** \brief Get the QuadTree Position that would contain a point
 * \arg point The point that we're checking.
 * \returns The QuadTree Position.
 */

QuadPosition QuadTree::SubTreeThatContains(Coordinate point){
	bool rightOfCenter = point.GetX() > center.GetX();
	bool aboveCenter = point.GetY() > center.GetY();
	int pos =  (aboveCenter?0:2) | (rightOfCenter?1:0);
	assert(this->Contains(point)); // Ensure that this point is in this region
	return QuadPosition(pos);
}

/** \brief Build a new subtree at the given QuadPosition
 * \arg pos The QuadPosition that should be created.
 */

void QuadTree::CreateSubTree(QuadPosition pos){
	float half = radius/2;
	// Each subtree has a specific new center
	Coordinate offset;
	switch(pos){
		// Center of this QuadPosition:      (  X  ,  Y  )
		case UPPER_LEFT:  offset = Coordinate(-half,+half); break;
		case UPPER_RIGHT: offset = Coordinate(+half,+half); break;
		case LOWER_LEFT:  offset = Coordinate(-half,-half); break;
		case LOWER_RIGHT: offset = Coordinate(+half,-half); break;
		default: assert(0);
	}
	assert(subtrees[pos]==NULL);
	subtrees[pos] = new QuadTree(center+offset,half);
	assert(subtrees[pos]!=NULL);
}

/** \brief Insert an object into a SubTree
 *  If the associated QuadTree subtree doesn't exist, this creates it.
 *  This is used for moving Sprites around internally.
 *  It doesn't do any accounting for this Tree.
 */

void QuadTree::InsertSubTree(Sprite *obj){
	QuadPosition pos = SubTreeThatContains( obj->GetWorldPosition() );
	if(subtrees[pos]==NULL)
		CreateSubTree(pos);
	assert(subtrees[pos]!=NULL);
	subtrees[pos]->Insert(obj);
}

/** \brief Ballance the QuadTree by splitting and merging subtrees
 *
 * If this is a Leaf that contains more than QUADMAXOBJECTS Sprites, it splits itself.
 *
 * If this is a Node that contains fewer than QUADMAXOBJECTS Sprites, it merges all subtrees into itself.
 *
 * (Leaf Trees smaller than a specific size will not split.)
 */

void QuadTree::ReBallance(){
	unsigned int numObjects = this->Count();
	list<Sprite*>::iterator i;
	
	if( isDirty && isLeaf && numObjects>QUADMAXOBJECTS && radius>MIN_QUAD_SIZE){
		//cout << "LEAF at "<<center<<" is becoming a NODE.\n";
		isLeaf = false;

		assert(0 != objects->size()); // The Leaf list should not be empty
		
		for( i = objects->begin(); i != objects->end(); ++i ) {
			InsertSubTree(*i);
		}
		assert(!isLeaf); // Still a Node
		this->objects->clear();
	} else if(isDirty && !isLeaf && numObjects<=QUADMAXOBJECTS ){
		assert(0 == objects->size()); // The Leaf list should be empty
		//cout << "NODE at "<<center<<" is becoming a LEAF.\n";
		isLeaf = true;
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				list<Sprite*> *other = subtrees[t]->GetSprites();
				for( i = other->begin(); i != other->end(); ++i ) {
					objects->push_back( *i );
				}
				delete other;
				delete subtrees[t];
				subtrees[t] = NULL;
			}
		}
		assert(isLeaf); // Still a Leaf
	}
	// ReBallance the subtrees
	for(int t=0;t<4;t++){
		if(NULL != (subtrees[t])){
			if(subtrees[t]->Count()==0){
				delete subtrees[t];
				subtrees[t] = NULL;
			} else {
				subtrees[t]->ReBallance();
			}
		}
	}
	isDirty=false;
	assert(numObjects == this->Count()); // ReBallancing should never change the total number of elements
}

/** \brief Generate an XML Node of this QuadTree.
 *
 * (Useful for debugging.)
 *
 * \returns xmlNodePtr of this QuadTree
 */

xmlNodePtr QuadTree::ToNode() {
	xmlNodePtr thisNode, objNode;
	char buff[256];
	list<Sprite*>::iterator i;

	thisNode = xmlNewNode(NULL, BAD_CAST "QuadTree" );

	snprintf(buff, sizeof(buff), "%d", (int) center.GetX() );
	xmlSetProp( thisNode, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int) center.GetY() );
	xmlSetProp( thisNode, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int) this->radius );
	xmlSetProp( thisNode, BAD_CAST "r", BAD_CAST buff );
	
	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				xmlAddChild(thisNode, subtrees[t]->ToNode() );
			}
		}
	} else { // Leaf
		for( i = objects->begin(); i != objects->end(); ++i ) {
			switch((*i)->GetDrawOrder()) {
				case DRAW_ORDER_PLANET:
					snprintf(buff, sizeof(buff), "%s", "Planet" );
					break;
				case DRAW_ORDER_WEAPON:
					snprintf(buff, sizeof(buff), "%s", "Weapon" );
					break;
				case DRAW_ORDER_SHIP:
					snprintf(buff, sizeof(buff), "%s", "Ship" );
					break;
				case DRAW_ORDER_PLAYER:
					snprintf(buff, sizeof(buff), "%s", "Player" );
					break;
				case DRAW_ORDER_GATE_TOP:
					snprintf(buff, sizeof(buff), "%s", "Gate" );
					break;
				case DRAW_ORDER_EFFECT:
					snprintf(buff, sizeof(buff), "%s", "Effect" );
					break;
				case DRAW_ORDER_GATE_BOTTOM: // Ignore
					continue;
				default:
					LogMsg(ERR,"Unknown Sprite Type: %d",(*i)->GetDrawOrder());
					assert(0);
					break;
			}
			objNode = xmlNewNode(NULL, BAD_CAST buff);
			snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetWorldPosition().GetX() );
			xmlSetProp( objNode, BAD_CAST "x", BAD_CAST buff );
			snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetWorldPosition().GetY() );
			xmlSetProp( objNode, BAD_CAST "y", BAD_CAST buff );
			snprintf(buff, sizeof(buff), "%d", (int) (*i)->GetAngle() );
			xmlSetProp( objNode, BAD_CAST "angle", BAD_CAST buff );
			xmlAddChild(thisNode, objNode);
		}
	}

	return thisNode;
}

