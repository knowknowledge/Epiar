/*
 * Filename      : ui_button.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, April 25, 2008
 * Last Modified : Friday, April 25, 2008
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

void Button::init( int x, int y, int w, int h, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
	
	// Load the bitmaps needed for drawing
	bitmap_normal = new Image( "ui_button.png" );
	//bitmap->Resize( w, h );
	bitmap_pressed = new Image( "ui_button_pressed.png" );
	bitmap_current = bitmap_normal;
	//bitmap_pressed->Resize( w, h );

	this->clickCallBack = NULL;
	this->lua_callback = "";
}

Button::Button( int x, int y, int w, int h, string label ) {
	// Is this default constructor even useful?
	// Why would there ever be a button without a callback?
	init( x, y, w, h, label );
	this->clickCallBack = debugClick;// TODO: set this to NULL
}

Button::Button( int x, int y, int w, int h, string label, void (*function)(void)) {
	init( x, y, w, h, label );
	this->clickCallBack = function;
}

Button::Button( int x, int y, int w, int h, string label, string lua_code) {
	init( x, y, w, h, label );
	this->lua_callback = lua_code;
}

Button::~Button() {
	Log::Message( "Deleting Button: '%s'.", (char *)label.c_str() );
	delete bitmap_normal;
	delete bitmap_pressed;
}

void Button::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	bitmap_current->Draw( x, y );

	// draw the label
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered( x + (w / 2), y + (h / 2), (char *)label.c_str() );
}

void Button::Focus( int x, int y ) {
	bitmap_current = bitmap_pressed;
	cout << "button focus: " << label << endl;
}

void Button::Unfocus( void ) {
	bitmap_current = bitmap_normal;
	cout << "button unfocus: " << label << endl;
}

void Button::MouseDown( int wx, int wy ) {
	if( clickCallBack ){
		Log::Message( "Clicked on: '%s'.", (char *)label.c_str() );
		clickCallBack();
	} else if("" != lua_callback){
		Log::Message("Clicked on '%s'. Running '%s'", (char *)label.c_str(), (char *)lua_callback.c_str() );
		Lua::Run(lua_callback);
	} else {
		Log::Warning( "Clicked on: '%s' but there was no function to call.", (char *)label.c_str() );
	}
}

void Button::debugClick(){
	Log::Message( "DEBUG Click at %d,%d", 18, 20);
}
