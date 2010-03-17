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

/**\brief Constructs a new Checkbox.
 */
Checkbox::Checkbox( int x, int y, bool checked, string label ) {
	SetX( x );
	SetY( y );
	
	this->label = label;
	this->checked = checked;
	if(label!=""){
		this->AddChild( new Label(CHECKBOX_W+5,CHECKBOX_H, label, 0) );
	}
}

void Checkbox::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	Video::DrawRect( x, y, CHECKBOX_W, CHECKBOX_H, 0.4f, 0.4f, 0.4f );
	Video::DrawRect( x + 1, y + 1, CHECKBOX_W - 2, CHECKBOX_H - 2, 0.15f, 0.15f, 0.15f );

	if( checked ) Video::DrawRect( x + 3, y + 3, CHECKBOX_W - 6, CHECKBOX_H - 6, 0.4f, 0.4f, 0.4f );

	// draw the label
	Widget::Draw(relx,rely);
}

void Checkbox::MouseLUp( int wx, int wy ) {
	if(checked) checked = false;
	else checked = true;
}
