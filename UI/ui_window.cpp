/*
 * Filename      : ui_window.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "common.h"
#include "Graphics/video.h"
#include "includes.h"
#include "UI/ui.h"
#include "Utilities/debug.h"

Window::Window( int x, int y, int w, int h, string caption ) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->caption = caption;

	Debug::Set();
	Debug::Print("new window (%d,%d,%d,%d,%s)\n", x, y, w, h, caption.c_str());
	Debug::Unset();
	
	// Load the bitmaps needed for drawing
	bitmaps[0] = new Image( "ui_wnd_up_left.png" );
	bitmaps[1] = new Image( "ui_wnd_up.png" );
	bitmaps[2] = new Image( "ui_wnd_up_right.png" );
	bitmaps[3] = new Image( "ui_wnd_left.png" );
	bitmaps[4] = new Image( "ui_wnd_right.png" );
	bitmaps[5] = new Image( "ui_wnd_low_left.png" );
	bitmaps[6] = new Image( "ui_wnd_low.png" );
	bitmaps[7] = new Image( "ui_wnd_low_right.png" );
	bitmaps[8] = new Image( "ui_wnd_back.png" );
}

Window::~Window() {
	Log::Message( "Deleting Window: '%s'.", (char *)caption.c_str() );
	delete bitmaps[0];
	delete bitmaps[1];
	delete bitmaps[2];
	delete bitmaps[3];
	delete bitmaps[4];
	delete bitmaps[5];
	delete bitmaps[6];
	delete bitmaps[7];
	delete bitmaps[8];
}

void Window::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// Draw the background
	bitmaps[8]->DrawTiled( x + bitmaps[3]->GetWidth(), y + bitmaps[1]->GetHeight(), w - bitmaps[3]->GetWidth() - bitmaps[4]->GetWidth(), h - bitmaps[1]->GetHeight() - bitmaps[6]->GetHeight() );
	
	// Draw the top section
	bitmaps[0]->Draw( x, y );
	bitmaps[1]->DrawTiled( x + bitmaps[0]->GetWidth(), y, w - bitmaps[0]->GetWidth() - bitmaps[2]->GetWidth(), bitmaps[1]->GetHeight() );
	bitmaps[2]->Draw( x + w - bitmaps[2]->GetWidth(), y );
	
	// Draw the left and right sections
	bitmaps[3]->DrawTiled( x, y + bitmaps[0]->GetHeight(), bitmaps[3]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight() );
	bitmaps[4]->DrawTiled( x + w - bitmaps[4]->GetWidth(), y + bitmaps[0]->GetHeight(), bitmaps[4]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight() );
	
	// Draw the bottom section
	bitmaps[5]->Draw( x, y + h - bitmaps[5]->GetHeight() );
	bitmaps[6]->DrawTiled( x + bitmaps[5]->GetWidth(), y + h - bitmaps[6]->GetHeight(), w - bitmaps[5]->GetWidth() - bitmaps[7]->GetWidth(), bitmaps[6]->GetHeight() );
	bitmaps[7]->Draw( x + w - bitmaps[7]->GetWidth(), y + h - bitmaps[7]->GetHeight() );

	// Draw the window title
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), caption.c_str());
	
	Widget::Draw();
}

void Window::Focus( int x, int y ) {
	cout << "window has focus" << endl;
	Widget::Focus( x, y );
}

void Window::Unfocus( void ) {
	cout << "window lost focus" << endl;
}

// wx & wy are coords of mouse down, relative to widget's upper left (0,0)
void Window::MouseDown( int wx, int wy ) {
	cout << "mouse down event on window, relative at " << wx << ", " << wy << endl;
	Widget *down_on = DetermineMouseFocus( wx, wy );
	if(down_on) {
		cout << "mouse down on child of window widget" << endl;
		down_on->MouseDown(wx,wy);
	} else {
		cout << "mouse NOT down on child of window widget" << endl;
	}
}
