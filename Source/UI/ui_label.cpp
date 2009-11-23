/**\file			ui_label.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Graphics/font.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/**\class Label
 * \brief UI label. */

Label::Label( int x, int y, string label ) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
}

Label::~Label() {

}

void Label::Draw(  int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// draw the label
	Vera10->SetColor( 1., 1., 1. );
	Vera10->RenderCentered( x, y, (char *)label.c_str() );
	
}
