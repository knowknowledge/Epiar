/**\file			ui_window.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef __H_WINDOW__
#define __H_WINDOW__

#include "Graphics/image.h"
#include "UI/ui.h"

class Window : public UIContainer {
	public:
		Window( int x, int y, int w, int h, string caption );
		~Window();
		Widget *AddChild( Widget *widget );
		Widget *DetermineMouseFocus( int relx, int rely );
		void Draw( int relx = 0, int rely = 0 );
	
		bool MouseDrag( int x, int y );
		bool SetDragability( bool _draggable );

		string GetType( void ) { return string("Window"); }

	private:
		bool draggable;

		Image *bitmaps[9];
		
		Scrollbar *hscrollbar;
		Scrollbar *vscrollbar;
};

#endif // __H_WINDOW__
