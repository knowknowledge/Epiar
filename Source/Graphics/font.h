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
			enum XPos{LEFT,	/**< Renders left aligned (default).*/
				CENTER,			/**< Renders centered on the point.*/
				RIGHT			/**< Renders right aligned.*/
				};
			enum YPos{TOP,	/**< Renders top aligned (default).*/
				MIDDLE,			/**< Renders centered on the point.*/
				BOTTOM			/**< Renders bottom aligned.*/
				};

			Font();
			Font( string filename );
			~Font();

			bool SetFont( string filename );
			void SetSize( int size=12 );
			unsigned int GetSize( void );
			void SetColor( float r, float g, float b, float a=1.0f );

			int TextWidth(const string& text);
			int LineHeight( void );
			int TightHeight( void );

			int Render( int x, int y, const string& text,XPos xpos=LEFT, YPos ypos=TOP );
			int RenderTight( int x, int y, const string& text,XPos xpos=LEFT, YPos ypos=TOP );
			int RenderWrapped( int x, int y, const string& text, int w );

		private:
			int RenderInternal( int x, int y, const string& text, int h, XPos xpos, YPos ypos);

			string fontname; // filename of the loaded font
			float r, g, b, a; // color of text
			int height, width, base;

			FTTextureFont* font;
};

#endif // H_FONT
