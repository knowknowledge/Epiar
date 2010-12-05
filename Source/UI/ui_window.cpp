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

Font *Window::font = NULL;

/**\brief Creates a new window with specified parameters.
 */
Window::Window( int x, int y, int w, int h, string caption ):
		draggable( true ),
		vscrollbar( NULL )
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

	if( font == NULL ) {
		font = Font::GetSkin( "Font/UI/Window" );
	}
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

	// Do not delete the scrollbar:
	// If these are non-NULL then they are children.
	// Therefore they will be deleted by the UIContainer destructor.
	vscrollbar = NULL;

}

/**\brief Adds a widget to the current Window.
 */
Window *Window::AddChild( Widget *widget ){
	assert( widget != NULL );

	// Check to see if widget is past the bounds.
	int vbnd = widget->GetY() + widget->GetH();

	if ( vbnd > this->h ){
		if ( !this->vscrollbar ){
			UIContainer::DelChild( this->vscrollbar );
			this->vscrollbar = new Scrollbar(
				this->w-SCROLLBAR_THICK-SCROLLBAR_PAD,
				SCROLLBAR_PAD+bitmaps[1]->GetHeight(),
				this->h-2*SCROLLBAR_PAD
				-bitmaps[1]->GetHeight()
				-SCROLLBAR_THICK,
				this);

			UIContainer::AddChild( this->vscrollbar );
		}

		this->vscrollbar->maxpos = vbnd;
	}

	return (Window*)UIContainer::AddChild( widget );
}

/**\brief Determines focused widget based on scrolled position.*/
Widget *Window::DetermineMouseFocus( int relx, int rely ){
	list<Widget *>::iterator i;

	int xoffset = 0;
	int yoffset = this->vscrollbar ? this->vscrollbar->pos : 0;


	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->Contains(relx+xoffset, rely+yoffset) ) {
			return (*i);
		}
	}
	return( NULL );
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
	font->SetColor( 1., 1., 1. );
	font->RenderTight(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), name, Font::CENTER,Font::MIDDLE);

	// Crop children drawing
	Video::SetCropRect(x, y + bitmaps[1]->GetHeight(), w, h - bitmaps[1]->GetHeight());
	
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( (*i) == this->vscrollbar ) {
			(*i)->Draw( x, y );
			continue;
		}

		int xscroll = 0;
		int yscroll = 0;

		if ( this->vscrollbar )
			yscroll = vscrollbar->pos;

		(*i)->Draw( x-xscroll, y-yscroll );
	}
	
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
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
			UIContainer::MouseDrag( x, y );
		}
	
	return true;
}

bool Window::SetDragability( bool _draggable ){
	draggable = _draggable;
	return true;
}
