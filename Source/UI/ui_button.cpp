/**\file			ui_button.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
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

/**\class Button
 * \brief UI button. */

void Button::Initialize( int x, int y, int w, int h, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
	
	// Load the bitmaps needed for drawing
	bitmap_normal = new Image( "Resources/Graphics/ui_button.png" );
	bitmap_normal->Resize( w, h );
	bitmap_pressed = new Image( "Resources/Graphics/ui_button_pressed.png" );
	bitmap_pressed->Resize( w, h );
	bitmap_current = bitmap_normal;

	this->clickCallBack = NULL;
	this->lua_callback = "";
}

Button::Button( int x, int y, int w, int h, string label ) {
	// Is this default constructor even useful?
	// Why would there ever be a button without a callback?
	Initialize( x, y, w, h, label );
	this->clickCallBack = debugClick;// TODO: set this to NULL
}

Button::Button( int x, int y, int w, int h, string label, void (*function)(void)) {
	Initialize( x, y, w, h, label );
	this->clickCallBack = function;
}

Button::Button( int x, int y, int w, int h, string label, string lua_code) {
	Initialize( x, y, w, h, label );
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
	
	Video::DrawRect( x, y, w, h, 1., 1., 1. );

	// draw the button (loaded image is simply scaled)
	bitmap_current->Draw( x, y );

	// draw the label
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered( x + (w / 2), y + (h / 2), (char *)label.c_str() );
}

void Button::FocusMouse( int x, int y ) {
	bitmap_current = bitmap_pressed;
}

void Button::UnfocusMouse( void ) {
	bitmap_current = bitmap_normal;
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
