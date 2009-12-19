/*
 * Filename      : font.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Graphics/font.h"
#include "Graphics/afont/afont_gl.h"
#include "includes.h"
#include "Utilities/log.h"
#include <FTGL/ftgl.h>
#include "Graphics/video.h"

Font::Font() {
	filename = NULL;

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

AFont::AFont( const char *filename ) {
	SetFont( filename );
}

AFont::~AFont() {
	afont_gl_free( (afontgl*)font );
	delete (afontgl*)this->font;
	Log::Message( "Font '%s' freed.", filename );
	free( filename );
}

bool AFont::SetFont( const char *filename ) {
	printf( "loading font '%s'...\n", filename );
	font = afont_gl_load( filename );

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.\n", filename );
		return( false );
	}

	this->filename = strdup( filename );

	afont_size_text( ((afontgl*)font)->orig, "A", &(this->width), &(this->height), &(this->base));

	Log::Message( "Font '%s' loaded.\n", filename );

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

// FreeFont

FreeFont::FreeFont( const char *filename ) {
	SetFont( filename );
}

FreeFont::~FreeFont() {
	delete (FONTRENDERTYPE*)this->font;
	Log::Message( "Font '%s' freed.", filename );
	free( filename );
}

bool FreeFont::SetFont( const char *filename ) {
	printf( "loading font '%s'...\n", filename );
	this->font = new FONTRENDERTYPE(filename);

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.\n", filename );
		return( false );
	}

	this->filename = strdup( filename );

	font->FaceSize(12);

	Log::Message( "Font '%s' loaded.\n", filename );

	return( true );
}

void FreeFont::Render( int x, int y, const char *text ) {
	glColor4f( r, g, b, 1. );
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1); 
	
	FTPoint pt = FTPoint( x, -y, 1);
	((FONTRENDERTYPE*)font)->Render(text,-1,pt);
	glPopMatrix(); // restore the previous matrix
}

// Renders text centered squarely on (x,y), taking the bounding box into account
void FreeFont::RenderCentered( int x, int y, const char *text ) {
	glColor4f( r, g, b, 1. );
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1); 

	float llx, lly, llz;
    float urx, ury, urz;

	((FONTRENDERTYPE*)font)->BBox(text, llx, lly, llz, urx, ury, urz);

	FTPoint pt = FTPoint( x + (llx - urx)/2, -(y - (lly-ury)/2), 1);
	((FONTRENDERTYPE*)font)->Render(text,-1,pt);
	glPopMatrix(); // restore the previous matrix
}

