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
 * \todo Some calculations are unnecessary here.
 */

/**\brief Constructs slider with given parameters
 */
Slider::Slider( int x, int y, int w, int h, const string& label ):
	minval( 0.0f ), maxval( 1.0f ), val ( 0.5f),
	w ( w ), h ( h ), label ( string(label) ){
	SetX( x );
	SetY( y );
}

/**\brief Calculates the pixel offset from the beginning to marker.
 */
 int Slider::ValToPixel( float value ){
	 
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
	return static_cast<int>((this->w - SLIDER_MW) *
		((this->val - this->minval)/
		(this->maxval - this->minval)) + SLIDER_MW/2);
 }

/**\brief Calculates the value from pixel offset
 */
 float Slider::PixelToVal( int pixels ){
	float value;
	if ( this->maxval < this->minval )
		value = (TO_FLOAT(pixels - SLIDER_MW/2 ) 
			/ TO_FLOAT(this->w - SLIDER_MW))
			* ( minval - maxval) + maxval;
	else
		value = (TO_FLOAT(pixels - SLIDER_MW/2)
			/ TO_FLOAT(this->w - SLIDER_MW))
			* ( maxval - minval) + minval;
	return value;
 }

/**\brief Empty destructor.
 */

Slider::~Slider() {
}

/**\brief Slider drawing function.
 */

void Slider::Draw( int relx, int rely ){
	int x, y;
	char value[20];

	x = GetX() + relx;
	y = GetY() + rely;

	int markerxr = this->ValToPixel( this->val );
	int markerx = x + markerxr;
	
	// Draw slider background
	Video::DrawRect( x, y+(this->h/2)-SLIDER_H/2, this->w, SLIDER_H,
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( markerx+1, y+(this->h/2)-SLIDER_H/2+1, 
			this->w-2 - markerxr, SLIDER_H-2,
			0.15f, 0.15f, 0.15f);

	// Draw marker
	Video::DrawRect( markerx-SLIDER_MW/2, y, SLIDER_MW, this->h,
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( markerx-SLIDER_MW/2+1, y+1, SLIDER_MW-2, this->h-2,
			0.15f, 0.15f, 0.15f);

	// Render the value indicator
	snprintf(value,20,"%.2f",this->val);
	Rect bounds = SansSerif->BoundingBox( value );
	SansSerif->RenderCentered( markerx, y+bounds.h/2 - SLIDER_VALPAD, value );
}

/**\brief Slider mouse drag call back.
 */
void Slider::MouseMotion( int xi, int yi, int dx, int dy ){
	this->val = this->PixelToVal(xi - GetX());
}

/**\brief Slider mouse down call back.
 */
void Slider::MouseDown( int wx, int wy ){
	this->val = this->PixelToVal(wx - GetX());
}
