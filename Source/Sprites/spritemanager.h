/**\file			spritemanager.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
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

