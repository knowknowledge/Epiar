/**\file			ui_dropdown.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, November 18, 2010
 * \date			Modified: Friday, December 31, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Dropdown
 * \brief Dropdown handling. */

/**\brief Creates a new dropdown with specified parameters.
 */
Dropdown::Dropdown( int x, int y, int w, int _baseheight )
{
	// TODO The Dropdowns really need their own Art.
	bitmap_normal = Image::Get( "Resources/Skin/ui_dropdown_closed.png" );
	bitmap_open = Image::Get( "Resources/Skin/ui_dropdown_open.png" );
	bitmap_mouseover = Image::Get( "Resources/Skin/ui_dropdown_mouseover.png" );
	bitmap_selected = Image::Get( "Resources/Skin/ui_dropdown_selected.png" );

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = _baseheight;

	baseheight = _baseheight;
	xoffset = convertTo<int>( SKIN("Skin/UI/Dropdown/XOffset"));
	yoffset = convertTo<int>( SKIN("Skin/UI/Dropdown/YOffset"));

	opened = false;

	hovered = -1;
	selected = -1;
}

/**\brief Dropdown Desctructor
 */
Dropdown::~Dropdown() {

}

/**\brief Add a single option
 *  If there were no options before, this option is the new default.
 *  \param[in] option The new option.
 */
Dropdown* Dropdown::AddOption( string option ) {
	if( option != "" ) {
		options.push_back( option );
		if( options.size() == 1 ) {
			selected = 0;
		}
	}
	return this;
}

/**\brief Add multiple options
 *  If there were no options before, the first option is the new default.
 *  \param[in] options The list of new options.
 */
Dropdown* Dropdown::AddOptions( list<string> options ) {
	list<string>::iterator iter;
	for( iter = options.begin(); iter != options.end(); ++iter ) {
		AddOption( *iter );
	}
	return this;
}

/**\brief Draws the current dropdown.
 */
void Dropdown::Draw( int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;
	
	if( !opened ) {
		bitmap_normal->DrawStretch( x, y, w, h);
		if( (options.size() >= 1) ) {
			UI::font->RenderTight( x + (w / 2), y + (baseheight / 2), options[selected], Font::CENTER,Font::MIDDLE );
		}
	} else if( UI::GetZLayer() == 0 ) {
		UI::Defer( this, relx, rely );
	} else {
		unsigned int i;
		for( i = 0; i < options.size(); ++i ) {
			if( i == hovered ) {
				bitmap_mouseover->DrawStretch( x, y, w, baseheight);
			} else if( i == selected ) {
				bitmap_selected->DrawStretch( x, y, w, baseheight);
			}
			else {
				bitmap_open->DrawStretch( x, y, w, baseheight);
			}
			UI::font->RenderTight( x + (w / 2), y + (baseheight / 2), options[i], Font::CENTER,Font::MIDDLE );
			y += baseheight;
		}
	}
	
	Widget::Draw(relx,rely);
}

/**\brief Accept Left mouse clicks
 */
bool Dropdown::MouseLDown( int xi, int yi ) {
	return true;
}

/**\brief Toggle between open and closed.
 */
bool Dropdown::MouseLUp( int xi, int yi ) {
	if( opened ) {
		selected = (yi - y) / baseheight;
		assert( selected >= 0 );
		assert( selected < options.size() );
		close();
	} else {
		hovered = (yi - y) / baseheight;
		assert( hovered >= 0 );
		assert( hovered < options.size() );
		open();
	}
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Dropdown::MouseLeave( void ){
	//hovered = -1;
	//if( opened ) {
	//	close();
	//}
	return true;
}

/**\brief When opened, pick a new hovered option
 */
bool Dropdown::MouseMotion( int xi, int yi ){
	if( opened ) {
		hovered = (yi - y) / baseheight;
	}
	return true;
}

/**\brief Open the Dropdown to display the available options
 */
void Dropdown::open() {
	assert( !opened );
	h = baseheight * options.size();
	x += xoffset;
	y += yoffset;
	opened = true;
}

/**\brief Close the Dropdown to display the selected option
 */
void Dropdown::close() {
	assert( opened );
	h = baseheight;
	x -= xoffset;
	y -= yoffset;
	opened = false;
}

/**\brief Close the Dropdown to display the selected option
 */
bool Dropdown::SetText(string text){
	unsigned int i;
	for(i = 0; i < options.size(); i++){
		if(options[i] == text){
			selected = i;
			return true;
		}
	}
	LogMsg(WARN, "Tried to set selected dropdown option: '%s', but that option doesn't exist.", text.c_str() );
	return false;
}


/** @} */
