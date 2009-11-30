/*
 * Filename      : quadtree.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Tuesday, November 24 2009
 * Last Modified : Tuesday, November 24 2009
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_quadtree__
#define __h_quadtree__

#include "includes.h"
#include "Sprites/sprite.h"

enum QuadPosition{ UPPER_LEFT, UPPER_RIGHT,
                   LOWER_LEFT, LOWER_RIGHT };

class QuadTree {
	public:
		QuadTree(Coordinate center, int radius,unsigned int maxobjects);
		~QuadTree();

		unsigned int Count();
		bool Contains(Coordinate point);

		void Insert(Sprite* obj);
		bool Delete(Sprite* obj);

		list<Sprite*> *GetSprites();
		list<Sprite*> *GetSpritesNear(Coordinate point, int distance);
		list<Sprite*> *FixOutOfBounds();

		void Update();
		void Draw();

	private:
		QuadPosition SubTreeThatContains(Coordinate point);
		void CreateSubTree(QuadPosition pos);
		void InsertSubTree(Sprite* obj);

		void ReBallance();

		QuadTree* subtrees[4];
		list<Sprite*> *objects;
		Coordinate center;
		int radius;
		unsigned int maxobjects;
		bool isLeaf;
};

#endif // __h_quadtree__
