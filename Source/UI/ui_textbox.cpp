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

Textbox::~Textbox() {
	Log::Message( "Deleting Textbox: '%s'.", (char *)text.c_str() );
}

void Textbox::Draw( int relx, int rely ) {
	int x, y;
	Rectangle bbox; // bounding box of the drawn text
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, 0.4, 0.4, 0.4 );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, 0.15, 0.15, 0.15 );

	// draw the text
	Vera10->SetColor( 1., 1., 1. );
	bbox = Vera10->Render( x + 4, y + 12, (char *)text.c_str() ); // 4 and 12 are "magic numbers" that should
	                                                              // be updated later to actually reflect font size
	
	// draw the cursor (if it has focus and we're on an even second (easy blink every second))
	if( hasKeyboardFocus && ((SDL_GetTicks() % 500) < 300) ) {
		Video::DrawRect( x + 4 + bbox.w, y + 3, 1, h - 6, .8, .8, .8 );
	}
}

void Textbox::MouseDown( int wx, int wy ) {
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

void Textbox::KeyPress( SDLKey key ) {
	text.append( SDL_GetKeyName( key ) );
	
	cout << "text is now:" << text << endl;
}
