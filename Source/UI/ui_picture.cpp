/**\file			ui_picture.cpp
 * \author			Matt Zweig
 * \date			Created: Tuesday, November 2, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Widget for displaying Images
 * \details
 */

#include "includes.h"
#include "Graphics/image.h"
#include "UI/ui.h"
#include "UI/ui_picture.h"
#include "Utilities/log.h"

/**\class Picture
 * \brief UI picture. */

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
	bitmap->Draw( GetX()+relx, GetY()+rely, static_cast<float>(rotation));
}

void Picture::Set( Image *img ){
	// Potential Memory Leak
	// If the previous bitmap was created from new,
	// then that image is now lost.
	// We can't delete it though, since it could be shared (eg, Ship Model).
	bitmap = img;
}

void Picture::Set( string filename ){
	// Potential Memory Leak
	// If the previous bitmap was created from new,
	// then that image is now lost
	bitmap = new Image(filename);
}
