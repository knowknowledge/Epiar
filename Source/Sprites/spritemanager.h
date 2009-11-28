/*
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Wednesday, July 5, 2006
 * Purpose       : Sprite management services
 * Notes         :
 */

#ifndef __H_SPRITEMANAGER__
#define __H_SPRITEMANAGER__

#include "Sprites/sprite.h"
#include "Utilities/quadtree.h"

class SpriteManager {
	public:
		SpriteManager();
		void Add( Sprite *sprite );
		bool Delete( Sprite *sprite );
		
		void Update();
		void Draw();

		bool LoadNPCs( string filename );
		
		// Reorders sprite list to ensure correct drawing order
		void Order();

		list<Sprite*> *GetSprites();

	private:
        QuadTree *tree;
};

#endif // __H_SPRITEMANAGER__

