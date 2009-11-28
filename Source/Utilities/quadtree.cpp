/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#include "Utilities/quadtree.h"

QuadTree::QuadTree(Coordinate _center, float _radius, int _maxobjects){
    ll = lr = ul = ur = NULL;
    this->objects = new list<Sprite*>();
    this->radius = _radius;
    this->center = _center;
    treeCount=0;
    this->maxobjects = _maxobjects;
}

QuadTree::~QuadTree(){
	// Delete the Subtrees (Node)
	QuadTree* subtrees[4] = {ur,ul,ll,lr};
	int t;
	for(t=0;t<4;t++){
		if(NULL != (subtrees[t])) delete subtrees[t];
	}

	// Delete objects (Leaf)
	if( objects ) delete objects;
}

void QuadTree::Build(list<Sprite*> *sprites){
    list<Sprite *>::iterator i;
    // Insert each object
	for( i = sprites->begin(); i != sprites->end(); ++i ) {
		this->Insert(*i);
	}
}

bool QuadTree::Contains(Coordinate point){
	bool insideLeftBorder = center.GetX()+radius < point.GetX();
	bool insideRightBorder = center.GetX()-radius > point.GetX();
	bool insideTopBorder = center.GetY()+radius > point.GetY();
	bool insideBottomBorder = center.GetY()-radius < point.GetY();
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

void QuadTree::Insert(Sprite *obj){
	cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is Inserting a Sprite ("<<treeCount<<" Sprites).\n";
    treeCount++;
    if(treeCount>maxobjects){ // Node
        QuadTree** dest = SubTreeThatContains( obj->GetWorldPosition() );
        if(!*dest) CreateSubTree(dest);
        (*dest)->Insert(obj);
    } else { // Leaf
        objects->push_back(obj);
        // A Full Leaf should become a Node
        if (treeCount==maxobjects) {
			cout << "LEAF at "<<center.GetX()<<","<<center.GetY()<<" is becoming a NODE.\n";
            // Become Node
            this->Build(objects); 
            treeCount = objects->size();
            objects->clear();
        }
    }
	cout<<"Finally: Tree at "<<center.GetX()<<","<<center.GetY()<<" has "<<treeCount<<" Sprites\n";
}

bool QuadTree::Delete(Sprite* obj){
	if(!treeCount)
		return( false );

	if(!Contains(obj->GetWorldPosition()))
		return( false );

    if(treeCount>maxobjects){ // Node
		QuadTree** dest = SubTreeThatContains( obj->GetWorldPosition() );
		if(!*dest) 
			return( false );
		if( (*dest)->Delete(obj) ){
			treeCount--;
			if (treeCount<maxobjects) { // This Node should become a Leaf
				cout << "NODE at "<<center.GetX()<<","<<center.GetY()<<" is becoming a LEAF.\n";
				QuadTree* subtrees[4] = {ur,ul,ll,lr};
				int t;
				for(t=0;t<4;t++){
					if(NULL != (subtrees[t])){
						list<Sprite*>::iterator i;
						list<Sprite*> *other = subtrees[t]->GetSprites();
						for( i = other->begin(); i != other->end(); ++i ) {
							objects->push_back( *i );
						}
						delete other;
						delete subtrees[t];
					}
				}
				
			}
			return( true );
		} else {
			return( false );
		}
    } else { // Leaf
		list<Sprite *>::iterator i = std::find( objects->begin(), objects->end(), obj);
		if(i != objects->end())
		{
			i = objects->erase( i );
			treeCount--;
			return( true );
		} else {
			return( false );
		}
	}
}

list<Sprite *> *QuadTree::GetSprites() {
	cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is getting Sprites.\n";
	list<Sprite*> *other;
	list<Sprite*> *full = new list<Sprite*>();
	if(treeCount>maxobjects){ // Node
		cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is a NODE with "<<treeCount<<" Sprites.\n";
		QuadTree* subtrees[4] = {ur,ul,ll,lr};
		int t;
		for(t=0;t<4;t++){
			cout<<"Size before processing subtree["<<t<<"] is "<<full->size()<<" Sprites.\n";
			if(NULL != (subtrees[t])){
				list<Sprite*>::iterator i;
				other = subtrees[t]->GetSprites();
				for( i = other->begin(); i != other->end(); ++i ) {
					full->push_back( *i);
				}
				delete other;
			}
			cout<<"Size after processing subtree["<<t<<"] is "<<full->size()<<" Sprites.\n";
		}
		return full;
	} else { // Leaf
		cout<<"Tree at "<<center.GetX()<<","<<center.GetY()<<" is a LEAF with "<<treeCount<<" Sprites.\n";
		list<Sprite*>::iterator i;
		for( i = objects->begin(); i != objects->end(); ++i ) {
			full->push_back( *i);
			cout<<"\tSprite is at "<<((*i)->GetWorldPosition().GetX())<<','<<((*i)->GetWorldPosition().GetY())<<" .\n";
		}
		cout<<"\tFound "<<full->size()<<" Sprites.\n";
		return full;
	}
}

void QuadTree::Update(){
	if(treeCount>maxobjects){ // Node
		QuadTree* subtrees[4] = {ur,ul,ll,lr};
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
	if(treeCount>maxobjects){ // Node
		QuadTree* subtrees[4] = {ur,ul,ll,lr};
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

QuadTree** QuadTree::SubTreeThatContains(Coordinate point){
    QuadTree** subtrees[] = {&ur,&ul,&ll,&lr};
    int index = 0;
    index |= int(point.GetX() < center.GetX()); // 0 == right, 1 == left
    index |= int(point.GetY() < center.GetY())<<1; // 0 == top, 2 == bottom
    return subtrees[index]; // Return a pointer to the correct subtree
}

void QuadTree::CreateSubTree(QuadTree** sub){
    float half = radius/2;
    // Each subtree has a specific new center
    //                                                           (  X  ,  Y  )
    if       (sub == &ur) {*sub = new QuadTree(center+Coordinate(+half,+half),half,maxobjects); // Upper Right
    } else if(sub == &ul) {*sub = new QuadTree(center+Coordinate(-half,+half),half,maxobjects); // Upper Left
    } else if(sub == &lr) {*sub = new QuadTree(center+Coordinate(+half,-half),half,maxobjects); // Lower Right
    } else if(sub == &ll) {*sub = new QuadTree(center+Coordinate(-half,-half),half,maxobjects); // Lower Left
    } else {
        // Wasn't passed a real subtree!!
        assert(1);
    }
}
