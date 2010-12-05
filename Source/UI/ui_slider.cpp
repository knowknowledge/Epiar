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

/**\brief Constructs a slider with given parameters and a Lua callback
 */
Slider::Slider( int x, int y, int w, int h, const string& label,
		float value,
		const string& callback):
		minval( 0.000f ), maxval( 1.000f ), val( value ),
		lua_callback( string(callback) )
{
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	this->name=label;
}

/**\fn Slider::GetVal( )
 *  \brief Returns the current value.
 * \fn Slider::GetMin( )
 *  \brief Returns the current minimum value.
 * \fn Slider::GetMax( )
 *  \brief Returns the current maximum value.
 */

/**\brief Slider drawing function.
 */

void Slider::Draw( int relx, int rely ){
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	int markerxr = this->ValToPixel( this->val );
	int markerx = x + markerxr;
	
	// Draw slider background
	Video::DrawRect( x, y+(GetH()/2)-SLIDER_H/2, GetW(), SLIDER_H,
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( x+1, y+(GetH()/2)-SLIDER_H/2+1, GetW()-2, SLIDER_H-2,
			0.15f, 0.15f, 0.15f);
	Video::DrawRect( x+3, y+(GetH()/2)-SLIDER_H/2+3, 
			markerxr-3, SLIDER_H-6,
			0.4f, 0.4f, 0.4f);

	// Draw marker
	Video::DrawRect( markerx-SLIDER_MW/2, y, SLIDER_MW, GetH(),
			0.4f, 0.4f, 0.4f);
	Video::DrawRect( markerx-SLIDER_MW/2+1, y+1, SLIDER_MW-2, GetH()-2,
			0.15f, 0.15f, 0.15f);

	// Render the value indicator
	char value[20];
	snprintf(value,20,"%.2f",this->val);
	Font::GetSkin( "Font/UI/Slider" )->Render( markerx, y, value,Font::CENTER,Font::BOTTOM );
}

/**\brief Slider mouse drag call back.
 */
bool Slider::MouseDrag( int xi, int yi ){
	this->SetVal(this->PixelToVal(xi - GetX()));
	if( "" != this->lua_callback){
		/// \todo This will be replaced with Lua::Call
		std::ostringstream fullcmd;
		fullcmd << lua_callback << "(" << this->GetVal() << ")";
		if ( !fullcmd )
			LogMsg(ERR,"Bad conversion of float to string");
		Lua::Run(fullcmd.str());
	}
	return true;
}

/**\brief Slider mouse down call back.
 */
bool Slider::MouseLDown( int wx, int wy ){
	this->SetVal(this->PixelToVal(wx - GetX()));
	if( "" != this->lua_callback){
		/// \todo This will be replaced with Lua::Call
		std::ostringstream fullcmd;
		fullcmd << lua_callback << "(" << this->GetVal() << ")";
		if ( !fullcmd )
			LogMsg(ERR,"Bad conversion of float to string");
		Lua::Run(fullcmd.str());
	}
	return true;
}

/**\brief Never set the this->val directly, use this function.
 * \details
 * Bounds checking are performed.
 */
void Slider::SetVal( float value ){
	float checkedval = value;
	
	// In case the user switched max and min val,
	// we can still go ahead, everything will just be flipped.
	if ( this->maxval < this->minval){
		if ( value < this->maxval )
			checkedval = maxval;
		else if ( value > this->minval )
			checkedval = minval;
	}else{
		if ( value > this->maxval)
			checkedval = maxval;
		else if ( value < this->minval)
			checkedval = minval;
	}
	this->val = checkedval;
}

// Private functions
/**\brief Calculates the pixel offset from the beginning to marker.
 */
 int Slider::ValToPixel( float value ){
	return static_cast<int>((GetW() - SLIDER_MW) *
		((this->val - this->minval)/
		(this->maxval - this->minval)) + SLIDER_MW/2);
 }

/**\brief Calculates the value from pixel offset
 */
 float Slider::PixelToVal( int pixels ){
	float value;
	if ( this->maxval < this->minval )
		value = (TO_FLOAT(pixels - SLIDER_MW/2 ) 
			/ TO_FLOAT(GetW() - SLIDER_MW))
			* ( minval - maxval) + maxval;
	else
		value = (TO_FLOAT(pixels - SLIDER_MW/2)
			/ TO_FLOAT(GetW() - SLIDER_MW))
			* ( maxval - minval) + minval;
	return value;
 }
	
