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

QuadTree::~QuadTree(){
	this->objects->clear();
	delete objects;
	// Delete the Subtrees (Node)
	for(int t=0;t<4;t++){
		if(NULL != (subtrees[t])) delete subtrees[t];
	}
}

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

bool QuadTree::Contains(Coordinate point){
	bool insideLeftBorder = (center.GetX()-radius) <= point.GetX();
	bool insideRightBorder = (center.GetX()+radius) >= point.GetX();
	bool insideTopBorder = 	(center.GetY()+radius) >= point.GetY();
	bool insideBottomBorder = (center.GetY()-radius) <= point.GetY();
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

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

list<Sprite *> *QuadTree::GetSprites() {
	list<Sprite*> *other;
	list<Sprite*> *full = new list<Sprite*>();
	if(!isLeaf){ // Node
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

void QuadTree::GetSpritesNear(Coordinate point, float distance, list<Sprite*> *nearby){
	// The Maximum range is when the center and point are on a 45 degree angle.
	//   Root-2 of the radius + the distance
	const float maxrange = 1.42f*radius + distance;

	// If the distance to the point is greater than the max range,
	//   then no collisions are possible
	if( (point-center).GetMagnitudeSquared() > maxrange*maxrange){
		return;
	}

	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				subtrees[t]->GetSpritesNear(point,distance,nearby);
			}
		}
	} else { // Leaf
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			if( (point - (*i)->GetWorldPosition()).GetMagnitudeSquared() < distance*distance + (*i)->GetRadarSize()*(*i)->GetRadarSize() ) {
				nearby->push_back( *i);
			}
		}
	}
}

Sprite* QuadTree::GetNearestSprite(Sprite* obj, float distance){
	// The Maximum range is when the center and point are on a 45 degree angle.
	//   Root-2 of the radius + the distance
	const float maxrange = 1.42f*radius + distance;
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
				possible = subtrees[t]->GetNearestSprite(obj,distance);
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
			tmpdist = (point - (*i)->GetWorldPosition()).GetMagnitudeSquared();
			if( (tmpdist < mindist) && ((*i) != obj)) {
				mindist = tmpdist;
				closest = (*i);
			}
		}
	}
	return closest;
}

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

QuadPosition QuadTree::SubTreeThatContains(Coordinate point){
	bool rightOfCenter = point.GetX() > center.GetX();
	bool aboveCenter = point.GetY() > center.GetY();
	int pos =  (aboveCenter?0:2) | (rightOfCenter?1:0);
	assert(this->Contains(point)); // Ensure that this point is in this region
	return QuadPosition(pos);
}

void QuadTree::CreateSubTree(QuadPosition pos){
	float half = radius/2;
	// Each subtree has a specific new center
	Coordinate offset;
	switch(pos){
	//                                           (  X  ,  Y  )
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

void QuadTree::InsertSubTree(Sprite *obj){
	QuadPosition pos = SubTreeThatContains( obj->GetWorldPosition() );
	if(subtrees[pos]==NULL)
		CreateSubTree(pos);
	assert(subtrees[pos]!=NULL);
	subtrees[pos]->Insert(obj);
}

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
