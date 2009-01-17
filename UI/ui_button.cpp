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

Button::Button( int x, int y, int w, int h, string label ) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
	
	// Load the bitmaps needed for drawing
	bitmap = new Image( "ui_button.png" );
	bitmap->SetScaling( w, h );
	bitmap_pressed = new Image( "ui_button_pressed.png" );
	bitmap_pressed->SetScaling( w, h );
}

Button::~Button() {
	delete bitmap;
	delete bitmap_pressed;
}

void Button::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = rely - GetY();

	// draw the button (loaded image is simply scaled)
	bitmap->DrawAbsolute( x, y );
	// draw the label
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered( x + (w / 2), y - (h / 2), (char *)label.c_str() );
}

void Button::Focus( int x, int y ) {
	cout << "button has focus" << endl;
}

void Button::Unfocus( void ) {
	cout << "button lost focus" << endl;
}
