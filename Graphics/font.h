/*
 * Filename      : font.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef H_FONT
#define H_FONT

#include "includes.h"
#include "Graphics/afont/afont_gl.h"

class Font {
 public:
	Font();
	Font( const char *filename );
	~Font();

	bool SetFont( const char *filename );
	void Render( int x, int y, const char *text );
	void RenderCentered( int x, int y, const char *text );
	void SetColor( float r, float g, float b );

	afontgl *font;

 private:
	//afontgl *font; // handle to font
	char *filename; // filename of the loaded font
	float r, g, b; // color of text
};

#endif // H_FONT
