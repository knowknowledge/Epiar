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
Paragraph::Paragraph( int x, int y, int maxw, int h, string text ) 
{
	this->x=x;
	this->y=y;
	this->h=h;

	centered=false;
	maxwidth=maxw;

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

	// Start from the last line
	if( lines.size() > 0 )
	{
		curline = lines[ lines.size()-1 ];
	}

	// Break the message into words
	temp = TokenizedString( text, " \n" );
	for(iter = temp.begin(); iter != temp.end() ; ++iter ) {
		// Line endings always end the line.
		if( (*iter) ==  "\n" ) {
			lines.push_back( curline );
			curline = "";
		}
		// Spaces 
		else if( (*iter) ==  " " )
		{
			curline += " ";
			if( curwidth + widthspace >= maxwidth )
			{
				lines.push_back( curline );
				curline = "";
			}
			else
			{
				curline += " ";
			}
		}
		// Words
		else 
		{
			string word = *iter;
			if( UI::font->TextWidth( curline + word ) >= maxwidth )
			{
				lines.push_back( curline );
				curline = "";
			}
			curline += word;
		}
		// Recalculate the width on every iteration because
		// the sum of the line may not be equal to its parts.
		curwidth = UI::font->TextWidth( curline );
	}

	// Don't forget about the last line.
	if( curline != "" )
	{
		lines.push_back( curline );
	}

	this->name = text;
	this->h = lines.size() * UI::font->TightHeight( );

	// Update for the max width.
	w = 0;
	for(unsigned int i = 0; i<lines.size(); i++)
	{
		curwidth = UI::font->TextWidth( curline );
		if( curwidth > w) w = curwidth;
	}
}

/** @} */
