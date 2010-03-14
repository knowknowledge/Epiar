/**\file			ui_slider.cpp
 * \author			Maoserr
 * \date			Created: Saturday, March 13, 2010
 * \date			Modified: Saturday, March 13, 2010
 * \brief			Creates a slider widget
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "UI/ui_slider.h"
#include "Utilities/log.h"

/**\class Slider
 * \brief Slider widget.
 */

/**\brief Constructs slider with given parameters
 */
Slider::Slider( int x, int y, int w, int h, const string& label ):
	minval( 0.0f ), maxval( 1.0f ), val ( 0.5f),
	w ( w ), h ( h ), label ( string(label) ){
	SetX( x );
	SetY( y );
}



/**\brief Empty destructor.
 */

Slider::~Slider() {
}

/**\brief Slider drawing function.
 */

void Slider::Draw( int relx, int rely ){
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	// Draw slider background
	Video::DrawRect( x, y+(this->h/2)-SLIDER_H/2, this->w, SLIDER_H,
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( x+1, y+(this->h/2)-SLIDER_H/2+1, 
			this->w-2, SLIDER_H-2,
			0.15f, 0.15f, 0.15f);
	

	// In case the user switched max and min val,
	// we can still go ahead, everything will just be flipped.
	if ( this->maxval < this->minval){
		if ( (this->val < this->maxval))
			this->val = maxval;
		else if (this->val > this->minval)
			this->val = minval;
	}else{
		if ( (this->val > this->maxval))
			this->val = maxval;
		else if (this->val < this->minval)
			this->val = minval;
	}
	// Draw marker
	int markerx = x + this->w *
		((this->val - this->minval)/
		(this->maxval - this->minval));
	Video::DrawRect( markerx-SLIDER_MW/2, y, SLIDER_MW, this->h,
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( markerx-SLIDER_MW/2+1, y+1, SLIDER_MW-2, this->h-2,
			0.15f, 0.15f, 0.15f);
}

/**\brief Slider mouse drag call back.
 */
void Slider::MouseMotion( int xi, int yi, int dx, int dy ){
	this->val = (TO_FLOAT(xi-dx) / TO_FLOAT(this->w))
		* ( maxval - minval) + minval;
	Log::Message("x: %d dx: %d, w:%d, val: %f",xi,dx,this->w,this->val);
}

