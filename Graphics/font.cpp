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

Font::Font() {
	font = NULL;
	filename = NULL;

	r = 1.0f;
	g = 1.0f;
	b = 1.0f;
}

Font::Font( char *filename ) {
	font = NULL;
	this->filename = NULL;

	r = 1.0f;
	g = 1.0f;
	b = 1.0f;

	SetFont( filename );
}

Font::~Font() {
	afont_free( font );

	//Log::Message( "Font '%s' freed.", filename );

	free( filename );
}

bool Font::SetFont( char *filename ) {
	font = afont_load( filename );

	if( font == NULL ) {
		Log::Error( "Failed to load font '%s'.", filename );
		return( false );
	}

	afont_gl_make_bitmaps( font );

	this->filename = strdup( filename );

	//Log::Message( "Font '%s' loaded.", filename );

	return( true );
}

void Font::Render( int x, int y, char *text ) {
	glColor3f( r, g, b );

	glRasterPos2i( x, y );

	afont_gl_render_text( font, text );
}

// Renders text centered squarely on (x,y), taking the bounding box into account
void Font::RenderCentered( int x, int y, char *text ) {
	int w, h, base;

	// determine size of text
	afont_size_text( font, text, &w, &h, &base );

	Render( x - (w / 2), y + (h / 2) - base, text );
}

void Font::SetColor( float r, float g, float b ) {
	this->r = r;
	this->g = g;
	this->b = b;
}
