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

class Frame : public UIContainer {
	public:
		Frame( int x, int y, int w, int h );
		Frame *AddChild( Widget *widget );
		Widget *DetermineMouseFocus( int relx, int rely );
		void Draw( int relx = 0, int rely = 0 );
	
		bool MouseDrag( int x, int y );

		string GetType( void ) { return string("Frame"); }

	private:
		Image *bitmaps[9];

		Scrollbar *hscrollbar;
		Scrollbar *vscrollbar;
};

#endif // __H_FRAME__
