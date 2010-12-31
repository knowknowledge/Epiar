/**\file			ui_label.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Graphics/font.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/**\class Label
 * \brief UI label. */

/**\brief Constructor
 */
Label::Label( int x, int y, string label, bool centered) {
	Rect size;
	
	this->x=x;
	this->y=y;

	// w/h is dependent upon the text given
	
	this->centered = centered;
	SetText( label );
}

/**\brief Draw the Label
 */
void Label::Draw(  int relx, int rely ) {
	int x, y;
	vector<string>::iterator iter;
	
	x = this->x + relx;
	y = this->y + rely;
	
	// draw the label
	Font::XPos xpositioning = (centered) ? (Font::CENTER) : (Font::LEFT);
	Font::YPos ypositioning = (centered) ? (Font::MIDDLE) : (Font::TOP);

	for(iter = lines.begin(); iter != lines.end() ; ++iter, y += UI::font->TightHeight() )
	{
		UI::font->Render( x, y, *iter, xpositioning, ypositioning );
	}

	Widget::Draw(relx,rely + UI::font->TightHeight() / 2 );
}

void Label::SetText(string text) {
	lines.clear();
	AppendText( text );
}

void Label::AppendText(string text) {
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
