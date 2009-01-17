/*
 * Filename      : spritelist.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Wednesday, July 5, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Header for sprite list services
 * Notes         :
 */

#ifndef __H_SPRITELIST__
#define __H_SPRITELIST__

#include "Sprites/sprite.h"

class SpriteList {
	public:
		SpriteList();
		void Add( Sprite *sprite );
		bool Delete( Sprite *sprite );
		
		void Update( void );
		void Draw( void );

		bool LoadNPCs( string filename );
		
		// Reorders sprite list to ensure correct drawing order
		void Order( void );

		// Enumeration functions - allows outside code to go through out internal list
		list<Sprite *>::iterator Enumerate();
		list<Sprite *>::iterator Enumerate( list<Sprite *>::iterator &i );

	private:
		list<Sprite *> sprites;
};

#endif // __H_SPRITELIST__
