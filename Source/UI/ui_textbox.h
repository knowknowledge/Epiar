/**\file			ui_textbox.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#ifndef __H_TEXTBOX__
#define __H_TEXTBOX__

#include "Graphics/video.h"
#include "Graphics/font.h"
#include "Graphics/image.h"
#include "UI/ui.h"

class Textbox : public Widget {
	public:
		Textbox( int x, int y, int w, int rows, string text = "", string label = "");
		
		void Draw( int relx, int rely = 0 );

		string GetType( void ) {return string("Textbox");}
		virtual int GetMask( void ) { return WIDGET_TEXTBOX; }

		string GetText() { return text; }
		void SetText(string s) { text = s; }

	protected:
		bool MouseLUp( int wx, int wy );
		bool KeyPress( SDLKey key );

	private:
		string text;
		int rowPad; ///< The padding around each row of text

		string lua_callback;
		void (*clickCallBack)();

		static Font* font;
		static Color foreground;
		static Color background;
		static Color edge;
};

#endif // __H_TEXTBOX__
