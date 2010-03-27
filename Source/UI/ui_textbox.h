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
		Textbox( int x, int y, int w, int rows, string text = "", string label = "");
		
		string GetType( void ) {return string("Textbox");}
		void Draw( int relx, int rely = 0 );
		bool MouseLUp( int wx, int wy );
		bool KeyPress( SDLKey key );

		string GetText() { return text; }
		void SetText(string s) { text = s; }
	private:
		string text;

		string lua_callback;
		void (*clickCallBack)();
};

#endif // __H_TEXTBOX__
