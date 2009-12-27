/**\file			ui_textbox.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#ifndef __H_TEXTBOX__
#define __H_TEXTBOX__

#include "Graphics/image.h"
#include "UI/ui.h"

class Textbox : public Widget {
	public:
		Textbox( int x, int y, int w, int rows );
		Textbox( int x, int y, int w, int rows, string text );
		Textbox( int x, int y, int w, int rows, string text, string label );
		~Textbox();
		
		void Draw( int relx = 0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		
		void FocusKeyboard( void );
		void UnfocusKeyboard( void );
	
		void MouseDown( int wx, int wy );
		
		void KeyPress( SDLKey key );
	private:
		void Initialize( int x, int y, int w, int rows, string text = "", string caption = "" );
		int w, h;
		string text, label;
		bool hasKeyboardFocus;

		string lua_callback;
		void (*clickCallBack)();
};

#endif // __H_TEXTBOX__
