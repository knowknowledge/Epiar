/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Sprites/sprite.h"

class QuadTree {
    public:
        QuadTree(Coordinate center, float radius,int maxobjects);
		~QuadTree();
        void Build(list<Sprite*> *sprites);

		bool Contains(Coordinate point);

        void Insert(Sprite* obj);
		bool Delete(Sprite* obj);

		list<Sprite*> *GetSprites();
		//list<Sprite*> GetObjects(Coordinate point, float radius);

		void Update();
		void Draw();

    private:
        QuadTree** SubTreeThatContains(Coordinate point);
        void CreateSubTree(QuadTree**);

        QuadTree *ll, *lr, *ul, *ur;
        list<Sprite*> *objects;
        Coordinate center;
        float radius;
        int treeCount; // total objects, counting this one
        int maxobjects;
};

