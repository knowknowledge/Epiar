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

		// Manipulation
		void SetText(string text);
		void InsertText(int pos, string text);
		void AppendText(string text);
		void Erase(int delchars);

		// Drawing
		void Render( int x, int y, Font::XPos xpositioning, Font::YPos ypositioning );

		// Getters
		string GetText();
		int GetWidth() { return width; }
		int GetMaxWidth() { return maxwidth; }
		int GetHeight() { return lines.size() * font->TightHeight(); }

	protected:
		Font *font; ///< The Font used to render
		int maxwidth; ///< The total width of the text
		int width; ///< current width
		vector<string> lines; ///< The lines of text
};

#endif // __H_UI_TEXT__
