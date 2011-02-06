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
#include "common.h"
#include "Sprites/sprite.h"

#define MIN_QUAD_SIZE 10.0f
#define QUADRANTSIZE 4096.0f
#define QUADMAXOBJECTS 3

enum QuadPosition{ UPPER_LEFT, UPPER_RIGHT,
                   LOWER_LEFT, LOWER_RIGHT };

class QuadTree {
	public:
		QuadTree(Coordinate center, float radius);
		~QuadTree();

		unsigned int Count();
		const Coordinate GetCenter() {return center;}

		bool Contains(Coordinate point);
		inline bool PossiblyNear(Coordinate, float distance);

		void Insert(Sprite* obj);
		bool Delete(Sprite* obj);

		list<Sprite*> *GetSprites();
		void GetSpritesNear(Coordinate point, float distance, list<Sprite*> *returnList, int type = DRAW_ORDER_ALL);
		Sprite* GetNearestSprite(Sprite* obj, float distance, int type = DRAW_ORDER_ALL);
		list<Sprite*> *FixOutOfBounds();

		void Update( lua_State *L );
		void Draw(Coordinate root);
		void ReBallance();

		xmlNodePtr ToNode();

	private:
		QuadPosition SubTreeThatContains(Coordinate point);
		void CreateSubTree(QuadPosition pos);
		void InsertSubTree(Sprite* obj);

		QuadTree* subtrees[4];
		list<Sprite*> *objects;
		Coordinate center;
		float radius;
		unsigned int objectcount;
		union{
			// Unnamed struct so that these flags can be accessed directly
			struct{
				Uint32 isLeaf:1;
				Uint32 isDirty:1;
				Uint32 extra:30;
			};
			Uint32 flags;
		};
};

inline bool QuadTree::PossiblyNear(Coordinate point, float distance) {
	// The Maximum range is when the center and point are on a 45 degree angle.
	//   Root-2 of the radius + the distance
	// If the distance to the point is greater than the max range,
	//   then no collisions are possible
	// Math should be done in square-space to save time.
	const float maxrange = static_cast<float>(V_SQRT2)*radius + distance;
	return( (point-center).GetMagnitudeSquared() <= maxrange*maxrange );
}

#endif // __h_quadtree__
