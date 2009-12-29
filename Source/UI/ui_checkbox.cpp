/**\file			ui_checkbox.cpp
 * \author			Christopher Thielen (chris@luethy.net)
 * \date			Created: Monday, December 28, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"

/**\class Checkbox
 * \brief UI checkbox. */

Checkbox::Checkbox( int x, int y, bool checked, string label ) {
	SetX( x );
	SetY( y );
	
	this->label = label;
	this->checked = checked;
}

Checkbox::~Checkbox() {
	Log::Message( "Deleting Checkbox: '%s'.", (char *)label.c_str() );
}

void Checkbox::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	Video::DrawRect( x, y, CHECKBOX_W, CHECKBOX_H, 0.4, 0.4, 0.4 );
	Video::DrawRect( x + 1, y + 1, CHECKBOX_W - 2, CHECKBOX_H - 2, 0.15, 0.15, 0.15 );

	if( checked ) Video::DrawRect( x + 3, y + 3, CHECKBOX_W - 6, CHECKBOX_H - 6, 0.4, 0.4, 0.4 );

	// draw the label
	//Vera10->SetColor( 1., 1., 1. );
	//Vera10->RenderCentered( x + (w / 2), y + (h / 2), (char *)label.c_str() );
}

void Checkbox::FocusMouse( int x, int y ) {

}

void Checkbox::UnfocusMouse( void ) {

}

void Checkbox::MouseDown( int wx, int wy ) {
	if(checked) checked = false;
	else checked = true;
}
