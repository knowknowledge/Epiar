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

/** \addtogroup UI
 * @{
 */

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
	left = Image::Get( "Resources/Graphics/ui_slider_left.png" );
	right = Image::Get( "Resources/Graphics/ui_slider_right.png" );
	background = Image::Get( "Resources/Graphics/ui_slider_background.png" );
	bar = Image::Get( "Resources/Graphics/ui_slider_bar.png" );
	handle = Image::Get( "Resources/Graphics/ui_slider_handle.png" );

	LogMsg(INFO, "New Slider '%s' with callback = %s\n", label.c_str(), callback.c_str() );

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = ( handle->GetHeight() > background->GetHeight() )
	          ? handle->GetHeight()
	          : background->GetHeight();
	this->name = label;
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

	int markerx_pix = ValToPixel( val );
	
	// Draw slider background
	left->Draw( x, y );
	background->DrawTiled( x + left->GetWidth(), y, w - left->GetWidth() - right->GetWidth(), background->GetHeight() );
	right->Draw( x + w - right->GetWidth(), y );

	// Draw the Bar
	bar->DrawTiled( x + left->GetWidth(),
	                y + background->GetHalfHeight() - bar->GetHalfHeight(),
	                markerx_pix, bar->GetHeight() );

	// Draw marker
	handle->Draw( x + markerx_pix - handle->GetHalfWidth(),
	              y + background->GetHalfHeight() - handle->GetHalfHeight() );

	// Render the value indicator
	char value[20];
	snprintf(value,20,"%.2f",this->val);
	UI::font->Render( x + markerx_pix, y, value,Font::CENTER,Font::BOTTOM );

	Widget::Draw(relx, rely);
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
	return static_cast<int>( w * ((this->val - this->minval) / (this->maxval - this->minval)) );
}

/**\brief Calculates the value from pixel offset
 */
float Slider::PixelToVal( int pixels ){
	float value;
	value = (TO_FLOAT(pixels) / TO_FLOAT(w)); // Ratio of the pixels
	value *= (maxval - minval); // Multiply by range
	value += minval; // Add Baseline
	return value;
}

/** @} */
