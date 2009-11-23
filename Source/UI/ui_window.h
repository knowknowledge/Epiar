/**\file			ui_window.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef __H_WINDOW__
#define __H_WINDOW__

#include "Graphics/image.h"
#include "UI/ui.h"

class Window : public Widget {
	public:
		Window( int x, int y, int w, int h, string caption );
		~Window();
		
		void Draw( int relx = 0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
	
		void Focus( int x, int y );
		void Unfocus( void );

		Widget *DetermineMouseFocus( int x, int y );

		//void MouseDown( int wx, int wy ); // coords of mouse down, relative to widget's upper left (0,0)

	private:
		int w, h;
		int inner_top,inner_left,inner_right,inner_low;
		string caption;
		Image *bitmaps[9];
};

#endif // __H_WINDOW__
