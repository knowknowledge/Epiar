/**\file			ui_text.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Monday, August 22, 2011
 * \date			Modified: Monday, August 22, 2011
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "UI/ui.h"
#include "UI/ui_text.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Text
 * \brief UI
 */
Text::Text( Font* _font, string _text, int _maxwidth )
	:font(_font)
	,maxwidth(_maxwidth)
	,width(0)
	
{
	assert( _font != NULL );
	SetText( _text );
}

/**\brief Set the text string of this Widget
 */
void Text::SetText( string text ) {
	lines.clear();
	AppendText( text );
}

/**\brief Insert some text into the middle of the content.
 */
void Text::InsertText( int pos, string text ) {
	string temp = GetText();
	temp.insert(pos,text);
	SetText( temp );
}

/**\brief Append some text to the current text
 */
void Text::AppendText( string text ) {
	vector<string> temp;
	vector<string>::iterator iter;
	string curline = "";
	int curwidth = 0;
	int widthspace = font->TextWidth( " " );

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
			if( font->TextWidth( curline + word ) >= maxwidth )
			{
				lines.push_back( curline );
				curline = "";
			}
			curline += word;
		}
		// Recalculate the width on every iteration because
		// the sum of the line may not be equal to its parts.
		curwidth = font->TextWidth( curline );
	}

	// Don't forget about the last line.
	if( curline != "" )
	{
		lines.push_back( curline );
	}

	// Update width.
	width = 0;
	for(unsigned int i = 0; i<lines.size(); i++)
	{
		curwidth = font->TextWidth( curline );
		if( curwidth > width) width = curwidth;
	}
}

string Text::GetText() {
	string result = "";
	vector<string>::iterator iter;
	for(iter = lines.begin(); iter != lines.end() ; ++iter ) {
		result += (*iter);
	}
	return result;
}

void Text::Render( int x, int y, Font::XPos xpositioning, Font::YPos ypositioning ) {
	vector<string>::iterator iter;
	for(iter = lines.begin(); iter != lines.end() ; ++iter, y += UI::font->TightHeight() )
	{
		font->Render( x, y, *iter, xpositioning, ypositioning );
	}
}

/** @} */
