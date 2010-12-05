/**\file			ui_frame.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: August 24, 2010
 * \date			Modified: August 24, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"

/**\class Frame
 * \brief Frame handling. */

/**\brief Creates a new frame with specified parameters.
 */
Frame::Frame( int x, int y, int w, int h ):
		vscrollbar( NULL ){

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	// Load the bitmaps needed for drawing
	bitmaps[0] = Image::Get( "Resources/Graphics/ui_frame_up_left.png" );
	bitmaps[1] = Image::Get( "Resources/Graphics/ui_frame_up.png" );
	bitmaps[2] = Image::Get( "Resources/Graphics/ui_frame_up_right.png" );
	bitmaps[3] = Image::Get( "Resources/Graphics/ui_frame_left.png" );
	bitmaps[4] = Image::Get( "Resources/Graphics/ui_frame_right.png" );
	bitmaps[5] = Image::Get( "Resources/Graphics/ui_frame_low_left.png" );
	bitmaps[6] = Image::Get( "Resources/Graphics/ui_frame_low.png" );
	bitmaps[7] = Image::Get( "Resources/Graphics/ui_frame_low_right.png" );
	bitmaps[8] = Image::Get( "Resources/Graphics/ui_frame_back.png" );
}

/**\brief Adds a widget to the current Frame.
 */
Widget *Frame::AddChild( Widget *widget ){
	// Check to see if widget is past the bounds.
	int vbnd = widget->GetY() + widget->GetH();

	if ( vbnd > this->h ){
		if ( !this->vscrollbar ){
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

	return UIContainer::AddChild( widget );
}

/**\brief Determines focused widget based on scrolled position.*/
Widget *Frame::DetermineMouseFocus( int relx, int rely ){
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

/**\brief Draws the current frame.
 */
void Frame::Draw( int relx, int rely ) {
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

	// Crop children
	Video::SetCropRect(x, y + bitmaps[1]->GetHeight(), w, h - bitmaps[1]->GetHeight());
	
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( (*i) == this->vscrollbar ){
			(*i)->Draw( x, y );
			continue;
		}
		int xscroll=0;
		int yscroll=0;

		if ( this->vscrollbar )
			yscroll = vscrollbar->pos;

		(*i)->Draw( x-xscroll, y-yscroll );
	}
	
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}

bool Frame::MouseDrag( int x, int y ){
	int dx=this->dragX;
	int dy=this->dragY;
	// Only drag by titlebar
	if ( dy < bitmaps[1]->GetHeight() ) {
		this->x= x - dx;
		this->y=y - dy;
	} else {
	// Pass the event onto widget if not handling it.
		UIContainer::MouseDrag( x, y );
	}
	return true;
}

