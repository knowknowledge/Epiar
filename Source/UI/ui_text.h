/**\file			ui_text.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Monday, August 22, 2011
 * \date			Modified: Monday, August 22, 2011
 * \brief
 * \details
 */

#ifndef __H_UI_TEXT__
#define __H_UI_TEXT__

#include "Graphics/font.h"

class Text {
	public:
		Text( Font* font, string text, int maxwidth );
		~Text(){};

		void SetText(string text);
		void InsertText(int pos, string text);
		void AppendText(string text);
		string GetText();

		void Render( int x, int y, Font::XPos xpositioning, Font::YPos ypositioning );

		int GetHeight() { return lines.size() * font->TightHeight(); }

		void SetWidth( int w ) { width = w; }
		int GetWidth() { return width; }
		
		int GetMaxWidth() { return maxwidth; }

	protected:
		Font *font; ///< The Font used to render
		int maxwidth; ///< The total width of the text
		int width; ///< current width
		vector<string> lines; ///< The lines of text
};

#endif // __H_UI_TEXT__
