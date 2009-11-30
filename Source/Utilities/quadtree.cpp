/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#include "Utilities/quadtree.h"

const char* PositionNames[4] = { "UPPER_LEFT", "UPPER_RIGHT", "LOWER_LEFT", "LOWER_RIGHT"};

QuadTree::QuadTree(Coordinate _center, int _radius, unsigned int _maxobjects){
	assert(_maxobjects>0);
	assert(_radius>0.0f);
	for(int t=0;t<4;t++){
		subtrees[t] = NULL;
	}
	this->objects = new list<Sprite*>();
	this->radius = _radius;
	this->center = _center;
	this->maxobjects = _maxobjects;
	this->isLeaf = true;
}

QuadTree::~QuadTree(){
	// Delete the Subtrees (Node)
	for(int t=0;t<4;t++){
		if(NULL != (subtrees[t])) delete subtrees[t];
	}
}

unsigned int QuadTree::Count(){
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
				list<Sprite*>::iterator i;
				other = subtrees[t]->GetSprites();
				for( i = other->begin(); i != other->end(); ++i ) {
					full->push_back( *i);
				}
				delete other;
			}
		}
		return full;
	} else { // Leaf
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			full->push_back( *i);
		}
		return full;
	}
}

list<Sprite*> *QuadTree::FixOutOfBounds(){
	list<Sprite*>::iterator i;
	list<Sprite*> *other;
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
		// Insert any sprites that are inside of this Tree
		for( i = outofbounds->begin(); i != outofbounds->end(); ++i ) {
			if( this->Contains((*i)->GetWorldPosition()) ){
				this->Insert(*i);
				outofbounds->erase(i);
			}
		}
	} else { // Leaf
		// Collect and forget any out of bound sprites from object list
		for( i = objects->begin(); i != objects->end(); ++i ) {
			if(! this->Contains((*i)->GetWorldPosition()) ){
				outofbounds->push_back( *i );
				objects->erase(i);
			}
		}
	}
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
	if(!isLeaf){ // Node
		for(int t=0;t<4;t++){
			if(NULL != (subtrees[t])) subtrees[t]->Draw();
		}
	} else { // Leaf
		list<Sprite *>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			(*i)->Draw();
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
	
	if( isLeaf && numObjects>maxobjects ){
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
