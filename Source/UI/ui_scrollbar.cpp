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
	SetSize(length);
}

/**\brief Set the Length and Height of a Scrollbar.
 */

void Scrollbar::SetSize(int length) {
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
		Video::DrawRect( x+SCROLLBAR_BTN-2, y+1,
			this->w-(2*SCROLLBAR_BTN)+4,this->h-2,
			0.15f, 0.15f, 0.15f);
		Video::DrawRect( x+1,y+1,
				SCROLLBAR_BTN-4,SCROLLBAR_THICK-2,
				0.15f, 0.15f, 0.15f );
		Video::DrawRect( x+this->w-SCROLLBAR_BTN+3,
				y+1,SCROLLBAR_BTN-4,SCROLLBAR_THICK-2,
				0.15f, 0.15f, 0.15f );
		Video::DrawRect( x+this->MarkerPosToPixel(), y+3,
			this->GetMarkerSize(), SCROLLBAR_THICK-6,
			0.4f, 0.4f, 0.4f);
	}else{
		Video::DrawRect( x+1, y+SCROLLBAR_BTN-2,
			this->w-2,this->h-(2*SCROLLBAR_BTN)+4,
			0.15f, 0.15f, 0.15f);
		Video::DrawRect( x+1,y+1,
				SCROLLBAR_THICK-2,SCROLLBAR_BTN-4,
				0.15f, 0.15f, 0.15f );
		Video::DrawRect( x+1,y+this->h-SCROLLBAR_BTN+3,
				SCROLLBAR_THICK-2,SCROLLBAR_BTN-4,
				0.15f, 0.15f, 0.15f );
		Video::DrawRect( x+3, y+this->MarkerPosToPixel(),
			SCROLLBAR_THICK-6, this->GetMarkerSize(),
			0.4f, 0.4f, 0.4f);
	}
}

/**\brief Scroll to position on mouse down.
 */
void Scrollbar::MouseLDown( int x, int y ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	int newpos;
	if ( type == HORIZONTAL ){
		if( xr<SCROLLBAR_BTN )
			newpos = pos-SCROLLBAR_SCROLL;
		else if ( xr>(this->w-SCROLLBAR_BTN) )
			newpos = pos+SCROLLBAR_SCROLL;
		else
			newpos = this->MarkerPixelToPos( xr, yr );
	}else{
		if( yr<SCROLLBAR_BTN )
			newpos = pos-SCROLLBAR_SCROLL;
		else if ( yr>(this->h-SCROLLBAR_BTN) )
			newpos = pos+SCROLLBAR_SCROLL;
		else
			newpos = this->MarkerPixelToPos( xr, yr );
	}

	this->pos = this->CheckPos( newpos );
}

/**\brief Scroll to position on mouse motion.
 */
void Scrollbar::MouseMotion( int xi, int yi, int dx, int dy ){
	// Relative coordinate - to current widget
	int xr = xi - GetX();
	int yr = yi - GetY();

	this->pos = this->CheckPos(this->MarkerPixelToPos( xr, yr ));
}

/**\brief Calculates marker size based on current dimensions.
 */
int Scrollbar::GetMarkerSize( void ){
	int visiblelen;
	if ( type == HORIZONTAL )
		visiblelen = this->w+2*SCROLLBAR_PAD;
	else
		visiblelen = this->h+2*SCROLLBAR_PAD;

	// Calculate the size of the marker
	int markersize = this->h * visiblelen / maxpos;
	// Don't have super small marker size
	if ( markersize < SCROLLBAR_MINMARK )
		markersize = SCROLLBAR_MINMARK;
	return markersize;
}

/**\brief Calculates the marker position in pixel offset.
 */
int Scrollbar::MarkerPosToPixel( void ){
	int markerpos;
	int effectivelen;
	float posratio;		// 0 - 1 ratio of marker position
	if ( type == HORIZONTAL ){
		effectivelen = this->w-2*SCROLLBAR_BTN
			-this->GetMarkerSize();
		posratio = TO_FLOAT(pos)
			/TO_FLOAT(maxpos-(this->w+2*SCROLLBAR_PAD));
	}else{
		effectivelen = this->h-2*SCROLLBAR_BTN
			-this->GetMarkerSize();
		posratio = TO_FLOAT(pos)
			/TO_FLOAT(maxpos-(this->h+2*SCROLLBAR_PAD));
	}
	markerpos = SCROLLBAR_BTN+static_cast<int>(effectivelen*posratio);
	return markerpos;
}

/**\brief Calculates the marker position from pixel offset.
 */
int Scrollbar::MarkerPixelToPos( int xr, int yr ){
	int effectivelen;
	int screenlen;
	int newpos;
	int marksize = this->GetMarkerSize();
	int effectivestart = SCROLLBAR_BTN+marksize/2;
	if ( type == HORIZONTAL ){
		effectivelen = this->w-2*SCROLLBAR_BTN-marksize;
		screenlen = this->w+2*SCROLLBAR_PAD;
		newpos = TO_INT(TO_FLOAT(xr - effectivestart)
			/TO_FLOAT(effectivelen)
			*(maxpos-screenlen));
	}else{
		effectivelen = this->h-2*SCROLLBAR_BTN-marksize;
		screenlen = this->h+2*SCROLLBAR_PAD;
		newpos = TO_INT(TO_FLOAT(yr - effectivestart)
			/TO_FLOAT(effectivelen)
			*(maxpos-screenlen));
	}
	return newpos;
}


/**\brief Checks that the pos value is valid.
 */
int Scrollbar::CheckPos( int newpos ){
	int screenlen;
	if ( type == HORIZONTAL )
		screenlen = this->w+2*SCROLLBAR_PAD;
	else
		screenlen = this->h+2*SCROLLBAR_PAD;
	
	if ( newpos < 0 )
		newpos = 0;
	else if ( newpos > (maxpos - screenlen) )
		newpos = (maxpos - screenlen);

	return newpos;
}
