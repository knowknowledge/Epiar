/**\file			font.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/font.h"
#include "includes.h"
#include "Utilities/log.h"
#include <FTGL/ftgl.h>
#include "Graphics/video.h"
#include "Utilities/file.h"

/**\class Font
 * \brief Font class takes care of initializing fonts. */

Font::Font() {
	r = 1.;
	g = 1.;
	b = 1.;
	a = 1.;
}

void Font::SetColor( float r, float g, float b, float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

// FreeFont

FreeFont::FreeFont( string filename ) {
	font=NULL;
	SetFont( filename );
}

FreeFont::~FreeFont() {
	delete (FTTextureFont*)this->font;
	Log::Message( "Font '%s' freed.", fontname.c_str() );
}

bool FreeFont::SetFont( string filename ) {
	File fontFile;
	if( fontFile.OpenRead( filename.c_str() ) == false) {
		Log::Error( "Font '%s' could not be loaded.", fontname.c_str() );
		return( false );
	}

	if( this->font != NULL) {
		Log::Error( "Deleting the old font '%s'.\n", fontname.c_str() );
		delete this->font;
	}

	fontname = filename;
	this->font = new FTTextureFont( fontname.c_str() );

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.\n", fontname.c_str() );
		return( false );
	}

	font->FaceSize(12);

	Log::Message( "Font '%s' loaded.\n", fontname.c_str() );

	return( true );
}

Rectangle FreeFont::Render( int x, int y, const char *text ) {
	float llx, lly, llz;
	float urx, ury, urz;

	glColor4f( r, g, b, a );
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1); 
	
	( ( FTTextureFont * ) font )->BBox( text, llx, lly, llz, urx, ury, urz );
	
	FTPoint pt = FTPoint( x, -y, 1);
	( ( FTTextureFont * ) font )->Render( text, -1, pt );
	glPopMatrix(); // restore the previous matrix
	
	return Rectangle( (float)x, (float)y, -(llx - urx), lly - ury );
}

// Renders text centered squarely on (x,y), taking the bounding box into account
Rectangle FreeFont::RenderCentered( int x, int y, const char *text ) {
	float llx, lly, llz;
	float urx, ury, urz;

	( ( FTTextureFont * ) font )->BBox( text, llx, lly, llz, urx, ury, urz );

	Render( x + ( llx - urx) / 2, y - (lly - ury) / 2, text );

	return Rectangle( (float)x, (float)y, -(llx - urx), lly - ury );
}

