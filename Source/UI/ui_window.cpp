/**\file			ui_window.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"

/**\class Window
 * \brief Window handling. */

/**\brief Creates a new window with specified parameters.
 */
Window::Window( int x, int y, int w, int h, string caption ) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->caption = caption;
	this->hscrollbar = NULL;
	this->vscrollbar = NULL;

	Debug::Set();
	Debug::Print("new window (%d,%d,%d,%d,%s)\n", x, y, w, h, caption.c_str());
	Debug::Unset();
	
	// Load the bitmaps needed for drawing
	bitmaps[0] = Image::Get( "Resources/Graphics/ui_wnd_up_left.png" );
	bitmaps[1] = Image::Get( "Resources/Graphics/ui_wnd_up.png" );
	bitmaps[2] = Image::Get( "Resources/Graphics/ui_wnd_up_right.png" );
	bitmaps[3] = Image::Get( "Resources/Graphics/ui_wnd_left.png" );
	bitmaps[4] = Image::Get( "Resources/Graphics/ui_wnd_right.png" );
	bitmaps[5] = Image::Get( "Resources/Graphics/ui_wnd_low_left.png" );
	bitmaps[6] = Image::Get( "Resources/Graphics/ui_wnd_low.png" );
	bitmaps[7] = Image::Get( "Resources/Graphics/ui_wnd_low_right.png" );
	bitmaps[8] = Image::Get( "Resources/Graphics/ui_wnd_back.png" );

	//inner_top = bitmaps[0]->GetHeight();
	//inner_left = bitmaps[0]->GetWidth();
	//inner_right = w - bitmaps[7]->GetWidth();
	//inner_low = h - bitmaps[7]->GetHeight();
}

/**\brief Adds a widget to the current Window.
 */
bool Window::AddChild( Widget *widget ){
	// Check to see if widget is past the bounds.
	int hbnd = widget->GetX()+widget->GetWidth();
	int vbnd = widget->GetY()+widget->GetHeight();

	if ( hbnd > this->w ){
		if ( !this->hscrollbar ){
			this->hscrollbar = new Scrollbar( SCROLLBAR_PAD,
				this->h-SCROLLBAR_THICK-SCROLLBAR_PAD,
				this->w-2*SCROLLBAR_PAD, HORIZONTAL,
				this);
			Widget::AddChild( this->hscrollbar );
		}
		this->hscrollbar->maxpos = hbnd;
	}

	if ( vbnd > this->h ){
		if ( !this->vscrollbar ){
			this->vscrollbar = new Scrollbar(
				this->w-SCROLLBAR_THICK-SCROLLBAR_PAD,
				SCROLLBAR_PAD+bitmaps[1]->GetHeight(),
				this->h-2*SCROLLBAR_PAD
				-bitmaps[1]->GetHeight()
				-SCROLLBAR_THICK, VERTICAL,
				this);
			Widget::AddChild( this->vscrollbar );
		}
		this->vscrollbar->maxpos = vbnd;
	}

	return Widget::AddChild( widget );
}

/**\brief Draws the current window.
 */
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
	SansSerif->SetColor( 1., 1., 1. );
	SansSerif->RenderCentered(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), caption.c_str());

	// Crop when necessary
	if ( this->hscrollbar || this->vscrollbar )
		Video::SetCropRect(this->GetX(),
				this->GetY()+bitmaps[1]->GetHeight(),
				this->w-SCROLLBAR_PAD,
				this->h-SCROLLBAR_PAD
				-bitmaps[1]->GetHeight());
	
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( ((*i)==this->hscrollbar) ||
				((*i)==this->vscrollbar) ){
			(*i)->Draw( x, y );
			continue;
		}
		int xscroll=0;
		int yscroll=0;
		if ( this->hscrollbar )
			xscroll = hscrollbar->pos;
		if ( this->vscrollbar )
			yscroll = vscrollbar->pos;
		(*i)->Draw( x-xscroll,y-yscroll );
	}
	
	if ( this->hscrollbar || this->vscrollbar )
		Video::UnsetCropRect();
}

void Window::MouseMotion( int x, int y, int dx, int dy ){
	// Only drag by titlebar
	if ( dy < bitmaps[1]->GetHeight() ) {
		this->SetX( x - dx);
		this->SetY( y - dy);
	} else {
	// Pass the event onto widget if not handling it.
		Widget::MouseMotion( x, y, dx, dy );
	}
}

// wx & wy are coords of mouse down, relative to widget's upper left (0,0)
//void Window::MouseLDown( int wx, int wy ) {
//	cout << "mouse down event on window, relative at " << wx << ", " << wy << endl;
//	Widget *down_on = DetermineMouseFocus( wx, wy );
//	if(down_on) {
//		cout << "mouse down on child of window widget" << endl;
//		down_on->MouseLDown(wx,wy);
//	} else {
//		cout << "mouse NOT down on child of window widget" << endl;
//	}
//}

//Widget *Window::DetermineMouseFocus( int x, int y ) {
	//cout << "window determine mouse focus" << endl;
//	return Widget::DetermineMouseFocus(x - inner_left, y - inner_top );
//}
