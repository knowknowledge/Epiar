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
	Rect size;
	
	this->x=x;
	this->y=y;

	// w/h is dependent upon the text given
	
	this->centered = centered;
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
	int maxwidth = 0;
	vector<string> temp;
	vector<string>::iterator iter;

	temp = TokenizedString( text, "\n" );
	for(iter = temp.begin(); iter != temp.end() ; ++iter ) {
		// Skip tokens (newlines)
		if( (*iter) ==  "\n" ) { continue; }
		lines.push_back( (*iter) );
		int linelength = UI::font->TextWidth( *iter );
		if( linelength > maxwidth ) maxwidth = linelength;
	}

	this->name = text;
	this->w = maxwidth;
	this->h = lines.size() * UI::font->TightHeight( );
}

/** @} */
