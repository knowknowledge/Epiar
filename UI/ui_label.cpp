/*
 * Filename      : ui_label.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, April 25, 2008
 * Last Modified : Friday, April 25, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Graphics/video.h"
#include "includes.h"
#include "UI/ui.h"

Label::Label( int x, int y, string label ) {
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	
	this->label = label;
}

Label::~Label() {

}

void Label::Draw( void ) {
	int x, y;
	
	x = GetX();
	y = GetY();
	
	// Draw the label
	
}
