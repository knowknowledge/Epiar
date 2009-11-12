/*
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Wednesday, July 5, 2006
 * Purpose       : Sprite management services
 * Notes         :
 */

#ifndef __H_SPRITEMANAGER__
#define __H_SPRITEMANAGER__

#include "Sprites/sprite.h"

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

		const list<Sprite*>& GetSprites();

	private:
		list<Sprite *> sprites;
};

#endif // __H_SPRITEMANAGER__

