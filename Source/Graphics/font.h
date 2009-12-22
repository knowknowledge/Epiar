/**\file			font.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef H_FONT
#define H_FONT

#include "includes.h"
#ifdef USE_FREETYPE
#include <FTGL/ftgl.h>
// Font Rendering technique to use
typedef FTTextureFont FONTRENDERTYPE;
#else
	#warning FreeType Disabled
#endif //USE_FREETYPE
#include "Graphics/afont/afont_gl.h"


class Font {
 public:
	Font();

	virtual bool SetFont( string filename ) = 0;
	virtual void Render( int x, int y, const char *text ) = 0;
	virtual void RenderCentered( int x, int y, const char *text ) = 0;
	void SetColor( float r, float g, float b );


 private:
	string fontname; // filename of the loaded font
	float r, g, b; // color of text
	int height,width,base;

	friend class AFont;
	friend class FreeFont;
};

// Epiar specific afont style
class AFont: public Font {
 public:
	AFont() { font=NULL; }
	AFont( string filename );
	~AFont();
	bool SetFont( string filename );
	void Render( int x, int y, const char *text );
	void RenderCentered( int x, int y, const char *text );

 private:
	afontgl* font;
};

#ifdef USE_FREETYPE
// Uses the FreeType and FTGL libraries
class FreeFont: public Font {
 public:
	FreeFont() { font=NULL; }
	FreeFont( string filename );
	~FreeFont();
	bool SetFont( string filename );
	void Render( int x, int y, const char *text );
	void RenderCentered( int x, int y, const char *text );
 private:
	FONTRENDERTYPE* font;
};
#endif //USE_FREETYPE

#endif // H_FONT
