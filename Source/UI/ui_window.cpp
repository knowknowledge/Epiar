/**\file			ui_window.cpp
 * \author			Chris Thielen (chris@epiar.net)
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
Window::Window( int x, int y, int w, int h, string caption ):
		draggable( true )
{

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->name = caption;

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
}

Window::~Window() {
	bitmaps[0] = NULL;
	bitmaps[1] = NULL;
	bitmaps[2] = NULL;
	bitmaps[3] = NULL;
	bitmaps[4] = NULL;
	bitmaps[5] = NULL;
	bitmaps[6] = NULL;
	bitmaps[7] = NULL;
	bitmaps[8] = NULL;
}

/**\brief Adds a widget to the current Window.
 */
Window *Window::AddChild( Widget *widget ){
	assert( widget != NULL );
	Container::AddChild( widget );
	return this;
}

/**\brief Draws the current window.
 */
void Window::Draw( int relx, int rely ) {
	int x, y;
	static float alpha = 0.95f;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// Draw the background
	bitmaps[8]->DrawTiledAlpha( x + bitmaps[3]->GetWidth(), y + bitmaps[1]->GetHeight(), w - bitmaps[3]->GetWidth() - bitmaps[4]->GetWidth(), h - bitmaps[1]->GetHeight() - bitmaps[6]->GetHeight(), alpha );
	
	// Draw the top section
	bitmaps[0]->Draw( x, y );
	bitmaps[1]->DrawTiled( x + bitmaps[0]->GetWidth(), y, w - bitmaps[0]->GetWidth() - bitmaps[2]->GetWidth(), bitmaps[1]->GetHeight() );
	bitmaps[2]->Draw( x + w - bitmaps[2]->GetWidth(), y );
	
	// Draw the left and right sections
	bitmaps[3]->DrawTiledAlpha( x, y + bitmaps[0]->GetHeight(), bitmaps[3]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight(), alpha );
	bitmaps[4]->DrawTiledAlpha( x + w - bitmaps[4]->GetWidth(), y + bitmaps[0]->GetHeight(), bitmaps[4]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight(), alpha );
	
	// Draw the bottom section
	bitmaps[5]->DrawAlpha( x, y + h - bitmaps[5]->GetHeight(), alpha );
	bitmaps[6]->DrawTiledAlpha( x + bitmaps[5]->GetWidth(), y + h - bitmaps[6]->GetHeight(), w - bitmaps[5]->GetWidth() - bitmaps[7]->GetWidth(), bitmaps[6]->GetHeight(), alpha );
	bitmaps[7]->DrawAlpha( x + w - bitmaps[7]->GetWidth(), y + h - bitmaps[7]->GetHeight(), alpha );

	// Draw the window title
	SansSerif->SetColor( 1., 1., 1. );
	SansSerif->RenderTight(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), name, Font::CENTER,Font::MIDDLE);

	Container::Draw(relx,rely);
}

bool Window::MouseDrag( int x, int y ){
	
		int dx=this->dragX;
		int dy=this->dragY;
		// Only drag by titlebar
		if ( dy < bitmaps[1]->GetHeight() && draggable == true ) {
			this->x= x - dx;
			this->y=y - dy;
		} else {
		// Pass the event onto widget if not handling it.
			Container::MouseDrag( x, y );
		}
	
	return true;
}

bool Window::SetDragability( bool _draggable ){
	draggable = _draggable;
	return true;
}
