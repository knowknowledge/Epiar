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
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

class Font {
 public:
	Font();
	Font( char *filename, float h );
	~Font();

	bool SetFont( char *filename, float h );
	void Render( int x, int y, char *text );
	void RenderCentered( int x, int y, char *text );
	void SetColor( float r, float g, float b );

 private:
	// generate display list corresponding to the given character
	void make_dlist(FT_Face face, char ch, GLuint list_base, GLuint *tex_base);
	int next_p2(int a); // returns the next power of 2

	char *filename; // filename of the loaded font
	float r, g, b; // color of text
	float h; // height of font
	GLuint *textures; // holds texture IDs
	GLuint list_base; // holds the first display list id
};

#endif // H_FONT
