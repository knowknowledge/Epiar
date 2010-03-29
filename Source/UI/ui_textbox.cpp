/**\file			ui_textbox.cpp
 * \author			Chris Thielen (chris@luethy.net)
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
#include "Utilities/debug.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"

/**\class Textbox
 * \brief UI textbox. */

/**\brief This is used to construct the Textbox.*/
Textbox::Textbox( int x, int y, int w, int rows, string text, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=rows * 20; // 20 is the assumed font height. this code should probably be a bit more intelligent
	this->name = label;
	
	this->text = text;
	
	clickCallBack = NULL;
	lua_callback = "";
}

/**\brief Draws the textbox.*/
void Textbox::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, 0.4f, 0.4f, 0.4f );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, 0.15f, 0.15f, 0.15f );

	// draw the text
	Video::SetCropRect(x,y,this->w,this->h);
	Mono->SetColor( 1., 1., 1. );
	int tw = Mono->Render( x, y, text );
	
	// draw the cursor (if it has focus and we're on an even second (easy blink every second))
	if( IsActive() && ((SDL_GetTicks() % 500) < 300) && !this->disabled ) {
		Video::DrawRect( x + 6 + tw, y + 3, 1, h - 6, .8f, .8f, .8f );
	}
	Video::UnsetCropRect();
}

bool Textbox::MouseLUp( int wx, int wy ) {
	if( clickCallBack ){
		Log::Message( "Clicked on: '%s'.", (char *)text.c_str() );
		clickCallBack();
	} else if("" != lua_callback){
		Log::Message("Clicked on '%s'. Running '%s'", (char *)text.c_str(), (char *)lua_callback.c_str() );
		Lua::Run(lua_callback);
	} else {
		Log::Warning( "Clicked on: '%s' but there was no function to call.", (char *)text.c_str() );
	}
	return true;
}

bool Textbox::KeyPress( SDLKey key ) {
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
		int len = text.length() - 1;
		if(len < 0) len = 0;
		text.erase( len );
	} else if(keyname == "space") {
		text.append( " " );
	} else {
		text.append( key_s );
	}
	
	return true;
}
