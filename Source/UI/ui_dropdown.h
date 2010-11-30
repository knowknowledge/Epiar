/**\file			ui_dropdown.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thursday, November 18, 2010
 * \date			Modified: Thursday, November 18, 2010
 * \brief
 * \details
 */

#ifndef __H_DROPDOWN__
#define __H_DROPDOWN__

#include "Graphics/image.h"
#include "UI/ui.h"

class Dropdown : public UIContainer {
	public:
		Dropdown( int x, int y, int w  );
		~Dropdown();
		//Widget *AddChild( Widget *widget );
		Widget *DetermineMouseFocus( int relx, int rely );
		void Draw( int relx = 0, int rely = 0 );
	
		//bool MouseDrag( int x, int y );
		//bool SetDragability( bool _draggable );

		string GetType( void ) { return string("Dropdown"); }

	private:
		//bool draggable;

		//Image *bitmaps[9];
		
		//Scrollbar *hscrollbar;
		Scrollbar *vscrollbar;
};

#endif // __H_DROPDOWN__
