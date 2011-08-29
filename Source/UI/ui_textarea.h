/**\file			ui_textbox.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#ifndef __H_TEXTAREA__
#define __H_TEXTAREA__

#include "Graphics/video.h"
#include "UI/ui.h"
#include "UI/ui_text.h"

class Textarea : public Widget {
	public:
		Textarea( int _x, int _y, int _w, int _h, string text = "", string label = "");
		
		void Draw( int relx, int rely = 0 );

		string GetType( void ) {return string("Textarea");}
		virtual int GetMask( void ) { return WIDGET_TEXTAREA; }

		string GetText() { return lines.GetText(); }
		void SetText(string s) { lines.SetText(s); }

	protected:
		bool KeyPress( SDLKey key );

	private:
		Text lines;

		static Color foreground;
		static Color background;
		static Color edge;
};

#endif // __H_TEXTAREA__
