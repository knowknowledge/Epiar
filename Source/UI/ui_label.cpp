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

Label::Label( int x, int y, string label, bool centered) {
	Rect size;
	
	this->x=x;
	this->y=y;

	// w/h is dependent upon the text given
	this->w = SansSerif->TextWidth( label);
	this->h = SansSerif->TightHeight( );
	
	this->name = label;
	this->centered = centered;
}

void Label::Draw(  int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;

	// draw the label
	SansSerif->SetColor( 1., 1., 1. );
	if(centered){
		SansSerif->Render( x, y, name, Font::CENTER,Font::MIDDLE );
	} else {
		SansSerif->Render( x, y, name );
	}
	
}
