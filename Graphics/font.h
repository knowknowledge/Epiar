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

#include "Graphics/afont/afont_gl.h"

class Font {
 public:
	Font();
	Font( char *filename );
	~Font();

	bool SetFont( char *filename );
	void Render( int x, int y, char *text );
	void RenderCentered( int x, int y, char *text );
	void SetColor( float r, float g, float b );

 private:
	afont *font; // handle to font
	char *filename; // filename of the loaded font
	float r, g, b; // color of text
};

#endif // H_FONT
