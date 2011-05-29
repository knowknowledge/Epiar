/**\file			ui_map.h
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Saturday, May 28, 2011
 * \brief			
 * \details
 */

#ifndef __H_UI_MAP
#define __H_UI_MAP

#include "Graphics/image.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Sprites/spritemanager.h"
#include "Utilities/coordinate.h"

class Map: public Widget {
	public:
		Map( int x, int y, int w, int h, Coordinate center, SpriteManager* sprites );
		~Map();

		void Draw( int relx = 0, int rely = 0 );

		Coordinate ClickToWorld( Coordinate click );
		Coordinate WorldToClick( Coordinate world );

		string GetType( void ) { return string("Map"); }
		virtual int GetMask( void ) { return WIDGET_MAP; }
	protected:
		virtual bool MouseLUp( int xi, int yi );
		virtual bool MouseLDown( int xi, int yi );
		virtual bool MouseWUp( int xi, int yi );
		virtual bool MouseWDown( int xi, int yi );
		virtual bool MouseDrag( int xi, int yi );

	private:
		float alpha;
		float scale;
		Coordinate center;
		SpriteManager* sprites;
		static Font *MapFont;
};

#endif // __H_UI_MAP

