/**\file			ui_textbox.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "UI/ui_textarea.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"

/** \addtogroup UI
 * @{
 */

Color Textarea::foreground = WHITE;
Color Textarea::background = GREY;
Color Textarea::edge = BLACK;

/**\class Textarea
 * \brief UI Textarea for editing multiple lines of text.
 */

/**\brief Constructor for the Textarea.
 */
Textarea::Textarea( int _x, int _y, int _w, int _h, string text, string label )
	:lines( Font::Get( SKIN( "Skin/UI/Textbox/Font" ) ), text, _w )
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	foreground = Color( SKIN( "Skin/UI/Textbox/Color/Foreground" ) );
	background = Color( SKIN( "Skin/UI/Textbox/Color/Background" ) );
	edge       = Color( SKIN( "Skin/UI/Textbox/Color/Edge" ) );
	name = label;
}

/**\brief Draws the Textarea.*/
void Textarea::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, background );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, edge );

	// draw the text
	Video::SetCropRect(x, y, w, h);
	lines.Render( x, y, Font::LEFT, Font::TOP );
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}

bool Textarea::KeyPress( SDLKey key ) {
	string keyname = SDL_GetKeyName( key );
	stringstream key_ss;
	string key_s;

	switch(key){
	// Ignore Modifiers
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
	case SDLK_RMETA:
	case SDLK_LMETA:
	case SDLK_RALT:
	case SDLK_LALT:
	case SDLK_RCTRL:
	case SDLK_LCTRL:
	case SDLK_RSUPER:
	case SDLK_LSUPER:
	// Special Non-Printable Keys
	case SDLK_ESCAPE:
	// TODO: add cursor movement support
	case SDLK_LEFT:
	case SDLK_RIGHT:
	case SDLK_UP:
	case SDLK_DOWN:
		return false;
	default:
		break;
	}
	
	key_ss << (char)key;
	key_ss >> key_s;
	
	
	if(keyname == "backspace") {
		lines.Erase( 1 );
	} else if(keyname == "space") {
		lines.AppendText( " " );
	} else {
		lines.AppendText( key_s );
	}
	
	return true;
}

/** @} */

