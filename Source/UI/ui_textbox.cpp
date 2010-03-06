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

void Textbox::Initialize( int x, int y, int w, int rows, string text, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = rows * 20; // 20 is the assumed font height. this code should probably be a bit more intelligent
	
	this->text = text;
	this->label = label;

	entryDisabled = false;
	
	hasKeyboardFocus = false;
	
	clickCallBack = NULL;
	lua_callback = "";
}

Textbox::Textbox( int x, int y, int w, int rows ) {
	// Is this default constructor even useful?
	// Why would there ever be a button without a callback?
	
	// Answer: Because a textbox is usually something that you simply get information
	//         from when a form is submitted or a window is closed. You wouldn't want a
	//         callback at all, just merely the ability to collect the information later, e.g.
	//         a GetValue() (which probably should be in Widget since many widgets can have values).
	
	Initialize( x, y, w, rows );
}

Textbox::Textbox( int x, int y, int w, int rows, string text ) {
	Initialize( x, y, w, rows, text );
}

Textbox::Textbox( int x, int y, int w, int rows, string text, string label ) {
	Initialize( x, y, w, rows, text, label );
}

void Textbox::DisableEntry( void ) {
	entryDisabled = true;
}

void Textbox::EnableEntry( void ) {
	entryDisabled = false;
}

Textbox::~Textbox() {
	Log::Message( "Deleting Textbox: '%s'.", (char *)text.c_str() );
}

void Textbox::Draw( int relx, int rely ) {
	int x, y;
	Rect bbox; // bounding box of the drawn text
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, 0.4f, 0.4f, 0.4f );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, 0.15f, 0.15f, 0.15f );

	// draw the text
	Mono->SetColor( 1., 1., 1. );
	bbox = Mono->Render( x + 4, y + 13, (char *)text.c_str() ); // 4 and 12 are "magic numbers" that should
	                                                              // be updated later to actually reflect font size
	
	// draw the cursor (if it has focus and we're on an even second (easy blink every second))
	if( hasKeyboardFocus && ((SDL_GetTicks() % 500) < 300) && !entryDisabled ) {
		Video::DrawRect( x + 6 + static_cast<int>(bbox.w), y + 3, 1, h - 6, .8f, .8f, .8f );
	}
}

void Textbox::MouseDown( int wx, int wy ) {
	if(entryDisabled) return;

	if( clickCallBack ){
		Log::Message( "Clicked on: '%s'.", (char *)text.c_str() );
		clickCallBack();
	} else if("" != lua_callback){
		Log::Message("Clicked on '%s'. Running '%s'", (char *)text.c_str(), (char *)lua_callback.c_str() );
		Lua::Run(lua_callback);
	} else {
		Log::Warning( "Clicked on: '%s' but there was no function to call.", (char *)text.c_str() );
	}
}

void Textbox::FocusKeyboard( void ) {
	cout << "textbox keyboard focus!" << endl;
	
	hasKeyboardFocus = true;
}

void Textbox::UnfocusKeyboard( void ) {
	cout << "textbox keyboard unfocus!" << endl;

	hasKeyboardFocus = false;
}

bool Textbox::KeyPress( SDLKey key ) {
	string keyname = SDL_GetKeyName( key );
	stringstream key_ss;
	string key_s;

	if(entryDisabled) return false;

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
