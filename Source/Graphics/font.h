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
#include <FTGL/ftgl.h>
#include "Graphics/video.h"

class Font {
		public:
			Font();

			virtual bool SetFont( string filename ) = 0;
			virtual Rectangle Render( int x, int y, const char *text ) = 0;
			virtual Rectangle RenderCentered( int x, int y, const char *text ) = 0;
			void SetColor( float r, float g, float b, float a=1.0f );


		private:
			string fontname; // filename of the loaded font
			float r, g, b, a; // color of text
			int height,width,base;

			friend class FreeFont;
};


// Uses the FreeType and FTGL libraries
class FreeFont: public Font {
		public:
			FreeFont() { font=NULL; }
			FreeFont( string filename );
			~FreeFont();
			bool SetFont( string filename );
			Rectangle Render( int x, int y, const char *text );
			Rectangle RenderCentered( int x, int y, const char *text );
		private:
			FTTextureFont* font;
};

#endif // H_FONT
