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

/**\brief Constructs new font (default color white).
 */
Font::Font() {
	// Solid White
	r = 0.;
	g = 0.;
	b = 0.;
	a = 1.;
	
	font = NULL;
}

Font::~Font() {
	delete (FTTextureFont*)this->font;
	Log::Message( "Font '%s' freed.", fontname.c_str() );
}

/**\brief Sets the new color and alpha value.
 */
void Font::SetColor( float r, float g, float b, float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

/**\class FreeFont
 * \brief Implements FTGL font routines.*/

/**\brief Construct new font based on file.
 * \param filename String containing file.
 */
Font::Font( string filename ) {
	// Solid White
	r = 0.;
	g = 0.;
	b = 0.;
	a = 1.;
	font = NULL;
	SetFont( filename );
}

/**\brief Loads the font (uses FTGL Texture Fonts).
 * \param filename Path to font file.
 */
bool Font::SetFont( string filename ) {
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

/**\brief Renders a string.
 * \param x X coordinate
 * \param y Y coordinate
 * \param text C style string pointer to text.
 */
Rect Font::Render( int x, int y, const char *text ) {
	float llx, lly, llz;
	float urx, ury, urz;

	glColor4f( r, g, b, a );
	glPushMatrix(); // to save the current matrix
	glScalef(1, -1, 1); 
	
	( ( FTTextureFont * ) font )->BBox( text, llx, lly, llz, urx, ury, urz );
	
	FTPoint pt = FTPoint( x, -y, 1);
	( ( FTTextureFont * ) font )->Render( text, -1, pt );
	glPopMatrix(); // restore the previous matrix
	
	return Rect( (float)x, (float)y, -(llx - urx), lly - ury );
}

/**\brief Renders text centered squarely on (x,y).
 * \details
 * Taking the bounding box into account.
 * \sa FreeFont::Render
 */
Rect Font::RenderCentered( int x, int y, const char *text ) {
	float llx, lly, llz;
	float urx, ury, urz;

	( ( FTTextureFont * ) font )->BBox( text, llx, lly, llz, urx, ury, urz );

	Render( x + static_cast<int>( llx - urx) / 2, y - static_cast<int>(lly - ury) / 2, text );

	return Rect( (float)x, (float)y, -(llx - urx), lly - ury );
}

/**\brief Returns the graphical size of text without rendering it.
 * \details
 * 
 * \sa Font::BoundingBox( string text )
 */
Rect Font::BoundingBox( const char *text ) {
	float llx, lly, llz;
	float urx, ury, urz;

	( (FTTextureFont*)font )->BBox( text, llx, lly, llz, urx, ury, urz );

	return Rect( 0., 0., -(llx - urx), lly - ury );	
}

/**\brief Returns the graphical size of the text without rendering it.
 */
Rect Font::BoundingBox( string text ) {
	return BoundingBox( text.c_str() );
}
