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
		static SpriteManager *Instance();

		void Add( Sprite *sprite );
		bool Delete( Sprite *sprite );
		
		void Update();
		void Draw();

		bool LoadNPCs( string filename );
		
		list<Sprite*> *GetSprites();
		list<Sprite*> *GetSpritesNear(Coordinate c, float r);

	protected:
		SpriteManager();
	private:
		// Use the tree when referring to the sprites at a location.
        QuadTree *tree;
		// Use the list when referring to all sprites.
		list<Sprite*> *spritelist;
		list<Sprite *> spritesToDelete;
		static SpriteManager *pInstance;
		bool DeleteSprite( Sprite *sprite );
};

#endif // __H_SPRITEMANAGER__

