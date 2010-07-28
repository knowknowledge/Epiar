/**\file			video.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */


#ifndef __H_VIDEO__
#define __H_VIDEO__

#include "includes.h"
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
	static Color Get( string str ) {
		// string must be in hex format.
		int n;
		Color clr;

		stringstream ss;
		ss << std::hex << str;
		ss >> n;

		clr.r = ((n >> 16) & 0xFF ) / 255.0f;
		clr.g = ((n >>  8) & 0xFF ) / 255.0f;
		clr.b = ((n      ) & 0xFF ) / 255.0f;

		return clr;
	}
};

class Rect {
	public:
		float x, y, w, h;
		
		Rect() { x = y = w = h = 0.0f; }
		Rect( float x, float y, float w, float h ) { this->x = x; this->y = y; this->w = w; this->h = h; }
		Rect( int x, int y, int w, int h ) { this->x = TO_FLOAT(x); this->y = TO_FLOAT(y); this->w = TO_FLOAT(w); this->h = TO_FLOAT(h); }
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
		static void DrawLine( int x1, int y1, int b2, int y2, float r, float g, float b, float a = 1.0f );
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b );
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b, float a );
		static void DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, Color col, float a = 1.0f);
		static void DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a = 1.0f);
		static void DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b );

		static void SetCropRect( int x, int y, int w, int h );
		static void UnsetCropRect( void );
 	protected:
 		Video();
  		Video( const Video & );
  		Video& operator= (const Video&);

 	private:
  		static Video *pInstance;
  		static int w, h; // width/height of screen
		static int w2, h2; // width/height divided by 2
		static stack<Rect> cropRects;
};

#endif // __H_VIDEO__

