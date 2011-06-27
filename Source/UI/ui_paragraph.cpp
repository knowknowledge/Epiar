/**\file			ui_paragraph.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Sunday, June 26, 2011
 * \date			Modified: Sunday, June 26, 2011
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Graphics/font.h"
#include "UI/ui.h"
#include "UI/ui_paragraph.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Paragraph
 * \brief UI Paragraph
 */

/**\brief Constructor
 */
Paragraph::Paragraph( int x, int y, int w, int h, string text ) 
{
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	this->centered = false;

	SetText( text );
}

/**\brief Draw the Paragraph
 */
void Paragraph::Draw(  int relx, int rely ) {
	int x, y;
	vector<string>::iterator iter;
	
	x = this->x + relx;
	y = this->y + rely;
	
	// draw the text 
	Font::XPos xpositioning = (centered) ? (Font::CENTER) : (Font::LEFT);
	Font::YPos ypositioning = (centered) ? (Font::MIDDLE) : (Font::TOP);

	for(iter = lines.begin(); iter != lines.end() ; ++iter, y += UI::font->TightHeight() )
	{
		UI::font->Render( x, y, *iter, xpositioning, ypositioning );
	}

	Widget::Draw(relx,rely + UI::font->TightHeight() / 2 );
}

/**\brief Set the text string of this Widget
 */
void Paragraph::SetText(string text) {
	lines.clear();
	AppendText( text );
}

/**\brief Append some text to the current text
 */
void Paragraph::AppendText(string text) {
	vector<string> temp;
	vector<string>::iterator iter;
	string curline = "";
	int curwidth = 0;
	int widthspace = UI::font->TextWidth( " " );

	temp = TokenizedString( text, " \n" );
	for(iter = temp.begin(); iter != temp.end() ; ++iter ) {
		if( (*iter) ==  "\n" ) {
			lines.push_back( curline );
			curline = "";
			curwidth = 0;
		}
		else if( (*iter) ==  " " )
		{
			curline += " ";
			if( curwidth + widthspace >= w )
			{
				lines.push_back( curline );
				curline = "";
				curwidth = 0;
			}
			else
			{
				curwidth += widthspace;
				curline += " ";
			}
		}
		else // Words
		{
			int wordwidth = UI::font->TextWidth( *iter );
			if( curwidth + wordwidth >= w )
			{
				lines.push_back( curline );
				curline = *iter;
				curwidth = 0;
				curline = *iter;
			}
			else
			{
				curwidth += wordwidth;
				curline += *iter;
			}
		}
	}

	this->name = text;
	this->h = lines.size() * UI::font->TightHeight( );
}

/** @} */
