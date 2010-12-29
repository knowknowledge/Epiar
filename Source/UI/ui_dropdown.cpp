/**\file			ui_dropdown.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thursday, November 18, 2010
 * \date			Modified: Thursday, November 18, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"

/**\class Dropdown
 * \brief Dropdown handling. */

/**\brief Creates a new dropdown with specified parameters.
 */
Dropdown::Dropdown( int x, int y, int w ):
		//draggable( true ),
		//hscrollbar( NULL ),
		vscrollbar( NULL )
{

	this->x = x;
	this->y = y;
	this->w = w;
}

Dropdown::~Dropdown() {
	// Do not delete the scrollbars:
	// If these are non-NULL then they are children.
	// Therefore they will be deleted by the Container destructor.
	vscrollbar = NULL;

}

/**\brief Adds a widget to the current Dropdown.
 */
//Widget *Dropdown::AddChild( Widget *widget ){
	// Check to see if widget is past the bounds.
	//int vbnd = widget->GetY() + widget->GetH();

	//if ( vbnd > this->h ){
		//if ( !this->vscrollbar ){
			//Container::DelChild( this->vscrollbar );
			//this->vscrollbar = new Scrollbar(
				//this->w-SCROLLBAR_THICK-SCROLLBAR_PAD,
				//SCROLLBAR_PAD+bitmaps[1]->GetHeight(),
				//this->h-2*SCROLLBAR_PAD
				//-bitmaps[1]->GetHeight()
				//-SCROLLBAR_THICK, VERTICAL,
				//this);
			//Container::AddChild( this->vscrollbar );
		//}
		//this->vscrollbar->maxpos = vbnd;
	//}

	//return Container::AddChild( widget );
//}

/**\brief Determines focused widget based on scrolled position.*/
Widget *Dropdown::DetermineMouseFocus( int relx, int rely ){
	list<Widget *>::iterator i;

	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->Contains(relx, rely + yoffset) ) {
			return (*i);
		}
	}

	return( NULL );
}

/**\brief Draws the current dropdown.
 */
void Dropdown::Draw( int relx, int rely ) {
	int x, y;
	//static float alpha = 0.95f;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// Draw the window title
	//UI::font->RenderTight(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), name, Font::CENTER,Font::MIDDLE);

	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( (*i) == this->vscrollbar ){
			(*i)->Draw( x, y );
			continue;
		}
		int yscroll = 0;
		if ( this->vscrollbar )
			yscroll = vscrollbar->GetPos();
		(*i)->Draw( x, y - yscroll );
	}
	
	//if ( this->vscrollbar )
		//Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}
