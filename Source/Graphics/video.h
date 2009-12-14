/**\file			video.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */


#ifndef __H_VIDEO__
#define __H_VIDEO__

#include "Utilities/coordinate.h"

class Color {
	public:
	float r, g, b;

	static Color Get( int r, int g, int b ) {
		Color clr;

		clr.r = static_cast<float> (r / 255.);
		clr.g = static_cast<float> (g / 255.);
		clr.b = static_cast<float> (b / 255.);

		return clr;
	}
	static Color Get( float r, float g, float b ) {
		Color clr;

		clr.r = r;
		clr.g = g;
		clr.b = b;

		return clr;
	}
};

class Video {
 	public:
		~Video();
  		static Video *Instance();

		static bool Initialize( void );
		static bool Shutdown( void );
		
  		static bool SetWindow( int w, int h, int bpp );

  		static void Update( void );
  		static void Erase( void );

  		static void EnableMouse( void );
  		static void DisableMouse( void );

		static int GetWidth( void );
		static int GetHalfWidth( void );
		static int GetHeight( void );
		static int GetHalfHeight( void );

		static void DrawPoint( int x, int y, float r, float g, float b );
		static void DrawPoint( Coordinate c, Color col );
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b );
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b, float a );
		static void DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b);
		static void DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b);
		static void DrawCircle( Coordinate c, int radius, float line_width, Color col);
		static void DrawFilledCircle( int x, int y, int radius, float r, float g, float b);

 	protected:
 		Video();
  		Video( const Video & );
  		Video& operator= (const Video&);

 	private:
  		static Video *pInstance;
  		static int w, h; // width/height of screen
		static int w2, h2; // width/height divided by 2
};

#endif // __H_VIDEO__

