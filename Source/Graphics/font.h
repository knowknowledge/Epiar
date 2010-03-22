/**\file			font.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, February 20, 2010
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
			Font( string filename );
			~Font();

			bool SetFont( string filename );
			Rect Render( int x, int y, const char *text );
			Rect RenderCentered( int x, int y, const char *text );
			void SetColor( float r, float g, float b, float a=1.0f );
			Rect BoundingBox( const char *text );
			Rect BoundingBox( string text );

		private:
			string fontname; // filename of the loaded font
			float r, g, b, a; // color of text
			int height, width, base;

			FTTextureFont* font;
};

#endif // H_FONT
