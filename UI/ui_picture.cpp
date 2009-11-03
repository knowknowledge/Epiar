/*
 * Filename      : ui_picture.h
 * Author(s)     : Matt Zweig
 * Date Created  : November 2, 2009
 * Last Modified : November 2, 2009
 * Purpose       : 
 * Notes         : 
 */

#include "Graphics/image.h"
#include "UI/ui.h"
#include "UI/ui_picture.h"
#include "Utilities/log.h"


Picture::Picture( int x, int y, int w, int h, string filename ){
	SetX( x );
	SetY( y );

	this->w = w;
	this->h = h;
	rotation=0.;
	bitmap = new Image(filename);
}

Picture::~Picture(){
	delete bitmap;
}

void Picture::Rotate(double angle){
	rotation=angle;
}

void Picture::Draw( int relx, int rely ){
	bitmap->Draw( GetX()+relx, GetY()+rely, rotation);
}
