/**\file			ui_frame.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: August 24, 2010
 * \date			Modified: August 24, 2010
 * \brief
 * \details
 */

#ifndef __H_FRAME__
#define __H_FRAME__

#include "Graphics/image.h"
#include "UI/ui.h"

class Frame : public Container {
	public:
		Frame( int x, int y, int w, int h );
		Frame *AddChild( Widget *widget );
		void Draw( int relx = 0, int rely = 0 );
	
		bool MouseDrag( int x, int y );

		string GetType( void ) { return string("Frame"); }
		virtual int GetMask( void ) { return WIDGET_FRAME | WIDGET_CONTAINER; }

	private:
		Image *bitmaps[9];
};

#endif // __H_FRAME__
