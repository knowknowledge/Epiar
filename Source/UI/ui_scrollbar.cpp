/**\file			ui_scrollbar.cpp
 * \author			Maoserr
 * \date			Created: Tuesday, March 16, 2010
 * \date			Modified: Tuesday, March 16, 2010
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/**\class Scrollbar
 * \brief Implements a scrollbar that can be attached to certain widgets.
 */

/**\brief Attaches Scrollbar to the widget specified.
 */
Scrollbar::Scrollbar( int x, int y, int length,
	scrollbarType type,Widget *parent):
		pos( 0 ),maxpos( 0 ),
		type( type ),parent( parent ){

	SetX( x );
	SetY( y );

	if ( type == HORIZONTAL ){
		this->w = length;
		this->h = SCROLLBAR_THICK;
	}else{
		this->w = SCROLLBAR_THICK;
		this->h = length;
	}
}

/**\brief Draws the scrollbar.
 */
void Scrollbar::Draw( int relx, int rely ){
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	Video::DrawRect( x, y, this->w,this->h,
		0.4f, 0.4f, 0.4f);
	if (this->type == HORIZONTAL){

	}else{
		// Calculate the size of the marker
		float markersize = this->h *
			(parent->GetHeight()/maxpos);
		// Don't have super small marker size
		if ( markersize < 15 )
			markersize = 15;
		float markerpos = TO_FLOAT(this->h-markersize)
			*(TO_FLOAT(pos)/TO_FLOAT(maxpos-this->h));
		Video::DrawRect( x, y+markerpos,
			SCROLLBAR_THICK, markersize,
			0.15f, 0.15f, 0.15f);
	}
}

/**\brief Scroll to position on mouse down.
 */
void Scrollbar::MouseLDown( int x, int y ){
	if (pos<(maxpos-this->h))
		pos+=20;
	Log::Message("Position incremented");
}
