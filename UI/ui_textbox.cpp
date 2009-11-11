/*
 * Filename      : ui_textbox.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, November 9, 2009
 * Last Modified : Monday, November 9, 2009
 * Purpose       : 
 * Notes         :
 */

#include "common.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "includes.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"

void Textbox::init( int x, int y, int w, int h, string text ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->text = text;
	
	// Load the bitmaps needed for drawing
	textbox = new Image( "ui_textbox.png" );
	//bitmap->Resize( w, h );
	//bitmap_pressed->Resize( w, h );

	this->clickCallBack = NULL;
	this->lua_callback = "";
}

Textbox::Textbox( int x, int y, int w, int h ) {
	// Is this default constructor even useful?
	// Why would there ever be a button without a callback?
	init( x, y, w, h, "" );
}

Textbox::Textbox( int x, int y, int w, int h, string text ) {
	init( x, y, w, h, text );
}

Textbox::~Textbox() {
	Log::Message( "Deleting Textbox: '%s'.", (char *)text.c_str() );
	delete textbox;
}

void Textbox::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	textbox->Draw( x, y );

	// draw the text
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered( x, y, (char *)text.c_str() );
}

void Textbox::Focus( int x, int y ) {
	cout << "textbox focus: " << text << endl;
}

void Textbox::Unfocus( void ) {
	cout << "textbox unfocus: " << text << endl;
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
