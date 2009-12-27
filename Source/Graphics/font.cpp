/**\file			font.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/font.h"
#include "Graphics/afont/afont_gl.h"
#include "includes.h"
#include "Utilities/log.h"
#ifdef USE_FREETYPE
#include <FTGL/ftgl.h>
#endif //USE_FREETYPE
#include "Graphics/video.h"

/**\class Font
 * \brief Font class takes care of initializing fonts. */

Font::Font() {
	r = 1.;
	g = 1.;
	b = 1.;
}

void Font::SetColor( float r, float g, float b ) {
	this->r = r;
	this->g = g;
	this->b = b;
}

// AFONT

AFont::AFont( string filename ) {
	SetFont( filename );
}

AFont::~AFont() {
	afont_gl_free( (afontgl*)font );
	delete (afontgl*)this->font;
	Log::Message( "Font '%s' freed.", fontname.c_str() );
}

bool AFont::SetFont( string filename ) {
	fontname = filename;
	font = afont_gl_load( fontname.c_str() );

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.\n", fontname.c_str() );
		return( false );
	}

	afont_size_text( ((afontgl*)font)->orig, "A", &(this->width), &(this->height), &(this->base));

	Log::Message( "Font '%s' loaded.\n", fontname.c_str() );

	return( true );
}

void AFont::Render( int x, int y, const char *text ) {
	glEnable( GL_TEXTURE_2D );
	glEnable(GL_BLEND);

	glColor4f( r, g, b, 1. );
	glRasterPos2i( x, y + height); // + height so that the top corner is at (x,y) like everything else.

	afont_gl_render_text( (afontgl*)font, text );
}

// Renders text centered squarely on (x,y), taking the bounding box into account
void AFont::RenderCentered( int x, int y, const char *text ) {
	glColor4f( r, g, b, 1. );

	int w, h, base;

	// determine size of text
	afont_size_text( ((afontgl*)font)->orig, text, &w, &h, &base );

	Render( x - (w / 2), y - (h / 2) , text ); // -1 because it just kinda looks better
}

#ifdef USE_FREETYPE
// FreeFont

FreeFont::FreeFont( string filename ) {
	SetFont( filename );
}

FreeFont::~FreeFont() {
	delete (FONTRENDERTYPE*)this->font;
	Log::Message( "Font '%s' freed.", fontname.c_str() );
}

bool FreeFont::SetFont( string filename ) {
	fontname = filename;
	this->font = new FONTRENDERTYPE( fontname.c_str() );

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.\n", fontname.c_str() );
		return( false );
	}

	font->FaceSize(12);

	Log::Message( "Font '%s' loaded.\n", fontname.c_str() );

	return( true );
}

void FreeFont::Render( int x, int y, const char *text ) {
	glColor4f( r, g, b, 1. );
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1); 
	
	FTPoint pt = FTPoint( x, -y, 1);
	( ( FONTRENDERTYPE * ) font )->Render( text, -1, pt );
	glPopMatrix(); // restore the previous matrix
}

// Renders text centered squarely on (x,y), taking the bounding box into account
void FreeFont::RenderCentered( int x, int y, const char *text ) {
	float llx, lly, llz;
    float urx, ury, urz;

	( ( FONTRENDERTYPE * ) font )->BBox( text, llx, lly, llz, urx, ury, urz );

	Render( x + ( llx - urx) / 2, y - (lly - ury) / 2, text );
}

#endif //USE_FREETYPE
