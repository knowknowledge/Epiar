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
#include "UI/ui_button.h"

class Window : public Container {
	public:
		Window( int w, int h, string caption );
		Window( int x, int y, int w, int h, string caption );
		~Window();
		Window *AddChild( Widget *widget );
		void Draw( int relx = 0, int rely = 0 );
	
		bool SetDragability( bool _draggable );
		void AddCloseButton();
		bool HasCloseButton() { return closeButton != NULL; }

		string GetType( void ) { return string("Window"); }
		virtual int GetMask( void ) { return WIDGET_WINDOW | WIDGET_CONTAINER; }

	protected:
		bool MouseDrag( int x, int y );

	private:
		bool draggable;
		static void CloseWindow( void* win);

		Image *bitmaps[9];
		Button *closeButton;
};

#endif // __H_WINDOW__
