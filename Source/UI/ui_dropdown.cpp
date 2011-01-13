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
#include "Utilities/debug.h"
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
	bitmap_normal = Image::Get( "Resources/Skin/ui_button.png" );
	bitmap_open = Image::Get( "Resources/Skin/ui_dropdown_open.png" );
	bitmap_mouseover = Image::Get( "Resources/Skin/ui_dropdown_mouseover.png" );
	bitmap_selected = Image::Get( "Resources/Skin/ui_button_pressed.png" );

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = _baseheight;
	baseheight = _baseheight;

	opened = false;

	hovered = -1;
	selected = -1;
}

Dropdown::~Dropdown() {

}

Dropdown* Dropdown::AddOption( string option ) {
	if( option != "" ) {
		options.push_back( option );
		if( options.size() == 1 ) {
			selected = 0;
		}
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
		UI::font->RenderTight( x + (w / 2), y + (baseheight / 2), options[selected], Font::CENTER,Font::MIDDLE );
	} else {
		unsigned int i;
		x += 6;
		y += 4;
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

bool Dropdown::MouseLDown( int xi, int yi ) {
	return true;
}

bool Dropdown::MouseLUp( int xi, int yi ) {
	if( opened ) {
		selected = (yi - y) / baseheight;
		close();
	} else {
		hovered = (yi - y) / baseheight;
		open();
	}
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Dropdown::MouseLeave( void ){
	hovered = -1;
	if( opened ) {
		close();
	}
	return true;
}

bool Dropdown::MouseMotion( int xi, int yi ){
	if( opened ) {
		hovered = (yi - y) / baseheight;
	}
	return true;
}

void Dropdown::open() {
	h = baseheight * options.size();
	opened = true;
}

void Dropdown::close() {
	h = baseheight;
	opened = false;
}


void Dropdown::SetText(string text){
	for(int i = 0; i < options.size(); i++){
		if(options[i] == text){
			selected = i;
			break;
		}
	}
	LogMsg(WARN, "Tried to set selected dropdown option: '%s', but that option doesn't exist.", text.c_str() );
}


/** @} */
