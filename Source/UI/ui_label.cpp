/*
 * Filename      : ui_label.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, April 25, 2008
 * Last Modified : Friday, April 25, 2008
 * Purpose       : 
 * Notes         :
 */

#include "common.h"
#include "Graphics/video.h"
#include "Graphics/font.h"
#include "includes.h"
#include "UI/ui.h"
#include "Utilities/log.h"

Label::Label( int x, int y, string label, bool centered) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
	this->centered = centered;
}

Label::~Label() {

}

void Label::Draw(  int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// draw the label
	Vera10->SetColor( 1., 1., 1. );
	if(centered){
		Vera10->RenderCentered( x, y, (char *)label.c_str() );
	} else {
		Vera10->Render( x, y, (char *)label.c_str() );
	}
	
}
