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
#include <FTGL/ftgl.h>
#include "Graphics/afont/afont_gl.h"

// Font Rendering technique to use
typedef FTTextureFont FONTRENDERTYPE;

class Font {
 public:
	Font();

	virtual bool SetFont( const char *filename ) = 0;
	virtual void Render( int x, int y, const char *text ) = 0;
	virtual void RenderCentered( int x, int y, const char *text ) = 0;
	void SetColor( float r, float g, float b );


 private:
	char *filename; // filename of the loaded font
	float r, g, b; // color of text
	int height,width,base;

	friend class AFont;
	friend class FreeFont;
};

// Epiar specific afont style
class AFont: public Font {
 public:
	AFont() { font=NULL; }
	AFont( const char *filename );
	~AFont();
	bool SetFont( const char *filename );
	void Render( int x, int y, const char *text );
	void RenderCentered( int x, int y, const char *text );

 private:
	afontgl* font;
};

// Uses the FreeType and FTGL libraries
class FreeFont: public Font {
 public:
	FreeFont() { font=NULL; }
	FreeFont( const char *filename );
	~FreeFont();
	bool SetFont( const char *filename );
	void Render( int x, int y, const char *text );
	void RenderCentered( int x, int y, const char *text );
 private:
	FONTRENDERTYPE* font;
};

#endif // H_FONT
