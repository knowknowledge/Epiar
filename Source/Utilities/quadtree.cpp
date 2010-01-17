/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Utilities/quadtree.h"
#include "Graphics/video.h"

const char* PositionNames[4] = { "UPPER_LEFT", "UPPER_RIGHT", "LOWER_LEFT", "LOWER_RIGHT"};

#define MIN_QUAD_SIZE 10.0f

QuadTree::QuadTree(Coordinate _center, float _radius, unsigned int _maxobjects){
	//cout<<"New QT at "<<_center<<" has R="<<_radius<<endl;
	assert(_maxobjects>0);
	assert(_radius>MIN_QUAD_SIZE/2);
	for(int t=0;t<4;t++){
		subtrees[t] = NULL;
	}
	this->objects = new list<Sprite*>();
	this->radius = _radius;
	this->center = _center;
	this->maxobjects = _maxobjects;
	this->isLeaf = true;
	this->objectcount = 0;
}

QuadTree::~QuadTree(){
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
		ReBallance();
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
			ReBallance();
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
	const float maxrange = 1.42*radius + distance;

	// If the distance to the point is greater than the max range,
	//   then no collisions are possible
	if( (point-center).GetMagnitude() > maxrange){
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
			if( (point - (*i)->GetWorldPosition()).GetMagnitude() < distance ) {
				nearby->push_back( *i);
			}
		}
	}
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
	ReBallance();
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

void QuadTree::Draw(){
	float scale = 8000;
	float r = scale* radius / 65536.0f;
	float x = (scale* center.GetX() / 65536.0f) + Video::GetHalfWidth()  -r;
	float y = (scale* center.GetY() / 65536.0f) + Video::GetHalfHeight() -r;
	Video::DrawRect( x,y, 2*r, 2*r, 0,255,0, .1);

	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])) subtrees[t]->Draw();
		}
	} else { // Leaf
		list<Sprite *>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			Coordinate pos = (*i)->GetWorldPosition();
			int posx = (scale* (float)pos.GetX() / 65536.0f) + (float)Video::GetHalfWidth();
			int posy = (scale* (float)pos.GetY() / 65536.0f) + (float)Video::GetHalfHeight();
			Color col = (*i)->GetRadarColor();
			Video::DrawCircle( posx, posy, (*i)->GetRadarSize()/17,2, col.r,col.g,col.b );
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
	subtrees[pos] = new QuadTree(center+offset,half,maxobjects);
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
	
	if( isLeaf && numObjects>maxobjects && radius>MIN_QUAD_SIZE){
		//cout << "LEAF at "<<center<<" is becoming a NODE.\n";
		isLeaf = false;

		assert(0 != objects->size()); // The Leaf list should not be empty
		
		for( i = objects->begin(); i != objects->end(); ++i ) {
			InsertSubTree(*i);
		}
		assert(!isLeaf); // Still a Node
		this->objects->clear();
	} else if(!isLeaf && numObjects<=maxobjects ){
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
	assert(numObjects == this->Count()); // ReBallancing should never change the total number of elements
}
