/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#include "Utilities/quadtree.h"

char* PositionNames[4] = { "UPPER_LEFT", "UPPER_RIGHT", "LOWER_LEFT", "LOWER_RIGHT"};

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
	//cout<<"NEW QuadTree at "<<center.GetX()<<","<<center.GetY()<<" with radius "<<radius<<".\n";
}

QuadTree::~QuadTree(){
	// Delete the Subtrees (Node)
	int t;
	for(t=0;t<4;t++){
		if(NULL != (subtrees[t])) delete subtrees[t];
	}

	// Delete objects (Leaf)
	if( objects ) delete objects;
}

void QuadTree::Build(list<Sprite*> *sprites){
	list<Sprite *>::iterator i;
	//cout<<"BUILDING a new QuadTree from a list with "<<sprites->size()<<" Sprites\n";
	// Insert each object
	for( i = sprites->begin(); i != sprites->end(); ++i ) {
		this->Insert(*i);
	}
	assert(sprites->size() == this->Count()); // All elements must be added
}

unsigned int QuadTree::Count(){
	if(isLeaf){
		return objects->size();
	} else {
		int t,total=0;
		for(t=0;t<4;t++){
			if(NULL != subtrees[t])
				total += subtrees[t]->Count();
		}
		return total;
	}
}

bool QuadTree::Contains(Coordinate point){
	//cout<<"Bounding box for NODE at "<<center.GetX()<<","<<center.GetY()<<" with radius "<<radius<<".\n";
	bool insideLeftBorder = (center.GetX()-radius) <= point.GetX();
	bool insideRightBorder = (center.GetX()+radius) >= point.GetX();
	bool insideTopBorder = 	(center.GetY()+radius) >= point.GetY();
	bool insideBottomBorder = (center.GetY()-radius) <= point.GetY();
	//cout<<"\tLEFT "<<(center.GetX()-radius)<<" : "<<(insideLeftBorder?"PASS":"FAIL")<<endl;
	//cout<<"\tRIGHT "<<(center.GetX()+radius)<<" : "<<(insideRightBorder?"PASS":"FAIL")<<endl;
	//cout<<"\tTOP "<<(center.GetY()+radius)<<" : "<<(insideTopBorder?"PASS":"FAIL")<<endl;
	//cout<<"\tBOTTOM "<<(center.GetY()-radius)<<" : "<<(insideBottomBorder?"PASS":"FAIL")<<endl;
	//cout<<"Point "<<point.GetX()<<","<<point.GetY()<<" is "<<((insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder)?"IN":"OUT")<<"SIDE of these bounds"<<endl;
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

void QuadTree::Insert(Sprite *obj){
	//cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is Inserting Sprite #"<<(this->Count()+1)<<".\n";
	if(! isLeaf ){ // Node
		InsertSubTree(obj);
	} else { // Leaf
		objects->push_back(obj);
		// An over Full Leaf should become a Node
		ReBallance();
	}
	//cout<<"Finally: Tree at "<<center.GetX()<<","<<center.GetY()<<" has "<<(this->Count())<<" Sprites\n";
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
	//cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is getting Sprites.\n";
	list<Sprite*> *other;
	list<Sprite*> *full = new list<Sprite*>();
	if(!isLeaf){ // Node
		//cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is a NODE with "<<this->Count()<<" Sprites.\n";
		int t;
		for(t=0;t<4;t++){
			//cout<<"Size before processing subtree["<<t<<"] is "<<full->size()<<" Sprites.\n";
			if(NULL != (subtrees[t])){
				list<Sprite*>::iterator i;
				other = subtrees[t]->GetSprites();
				for( i = other->begin(); i != other->end(); ++i ) {
					full->push_back( *i);
				}
				delete other;
			}
			//cout<<"Size after processing subtree["<<t<<"] is "<<full->size()<<" Sprites.\n";
		}
		return full;
	} else { // Leaf
		//cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is a LEAF with "<<this->Count()<<" Sprites.\n";
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			full->push_back( *i);
			//cout<<"\tSprite is at "<<((*i)->GetWorldPosition().GetX())<<','<<((*i)->GetWorldPosition().GetY())<<" .\n";
		}
		//cout<<"\tFound "<<full->size()<<" Sprites.\n";
		return full;
	}
}

void QuadTree::Update(){
	if(!isLeaf){ // Node
		int t;
		for(t=0;t<4;t++){
			if(NULL != (subtrees[t])) subtrees[t]->Update();
		}
	} else { // Leaf
		list<Sprite *>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			(*i)->Update();
		}
	}
}

void QuadTree::Draw(){
	if(!isLeaf){ // Node
		int t;
		for(t=0;t<4;t++){
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
	//cout<<"Point "<<point.GetX()<<","<<point.GetY()<<" should go in subtree["<<pos<<"] of NODE at "<<center.GetX()<<","<<center.GetY()<<" with radius "<<radius<<".\n";
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
	//cout << "INSERTING Sprite at "<<obj->GetWorldPosition().GetX()<<","<<obj->GetWorldPosition().GetY()<<" into NODE at "<<center.GetX()<<","<<center.GetY()<<".\n";
	QuadPosition pos = SubTreeThatContains( obj->GetWorldPosition() );
	//cout << "\t..into subtree "<<PositionNames[pos]<<" of NODE at "<<center.GetX()<<","<<center.GetY()<<".\n";
	if(subtrees[pos]==NULL)
		CreateSubTree(pos);
	subtrees[pos]->Insert(obj);
}

void QuadTree::ReBallance(){
	unsigned int numObjects = this->Count();
	list<Sprite*>::iterator i;
	
	if( isLeaf && numObjects>maxobjects ){
		//cout << "LEAF at "<<center.GetX()<<","<<center.GetY()<<" is becoming a NODE.\n";
		isLeaf = false;

		assert(0 != objects->size()); // The Leaf list should not be empty
		
		for( i = objects->begin(); i != objects->end(); ++i ) {
			InsertSubTree(*i);
		}
		assert(!isLeaf); // Still a Node
		this->objects->clear();
	} else if(!isLeaf && numObjects<=maxobjects ){
		assert(0 == objects->size()); // The Leaf list should be empty
		//cout << "NODE at "<<center.GetX()<<","<<center.GetY()<<" is becoming a LEAF.\n";
		int t;
		for(t=0;t<4;t++){
			if(NULL != (subtrees[t])){
				list<Sprite*> *other = subtrees[t]->GetSprites();
				for( i = other->begin(); i != other->end(); ++i ) {
					objects->push_back( *i );
				}
				delete other;
				delete subtrees[t];
			}
		}
		assert(isLeaf); // Still a Leaf
	}
	assert(numObjects == this->Count()); // ReBallancing should never change the total number of elements
}
