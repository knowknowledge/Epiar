/**\file			video.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */


#ifndef __H_VIDEO__
#define __H_VIDEO__

#include "Graphics/image.h"
#include "includes.h"
#include "Utilities/coordinate.h"
#include "Utilities/lua.h"

#define EPIAR_VIDEO "Video"

#define BLACK     ( Color(0x00,0x00,0x00) )
#define WHITE     ( Color(0xFF,0xFF,0xFF) )
#define RED       ( Color(0xFF,0x00,0x00) )
#define ORANGE    ( Color(0xFF,0x7F,0x00) )
#define YELLOW    ( Color(0xFF,0xFF,0x00) )
#define GREEN     ( Color(0x00,0xFF,0x00) )
#define BLUE      ( Color(0x00,0x00,0xFF) )
#define PURPLE    ( Color(0x80,0x00,0x80) )
#define GREY      ( Color(0x80,0x80,0x80) )
#define GRAY      GREY
#define GOLD      ( Color(0xFF,0xD7,0x80) )

class Color {
	public:
	float r, g, b;

	Color();
	Color& operator=(Color other);

	Color( int r, int g, int b );
	Color( float r, float g, float b );
	Color( string str );

	Color operator*(float delta);
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
		static bool Initialize( void );
		static bool Shutdown( void );
		
  		static bool SetWindow( int w, int h, int bpp, bool fullscreen );

  		static void RegisterVideo(lua_State *L);

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
		static void DrawLine( int x1, int y1, int x2, int y2, Color c, float a = 1.0f);
		static void DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a = 1.0f);
		static void DrawLine( Coordinate p1, Coordinate p2, Color c, float a = 1.0f);
		static void DrawRect( int x, int y, int w, int h, Color c, float a = 1.0f);
		static void DrawRect( int x, int y, int w, int h, float r, float g, float b, float a = 1.0f);
		static void DrawRect( Coordinate p, int w, int h, Color c, float a = 1.0f);
		static void DrawBox( int x, int y, int w, int h, Color c, float a = 1.0f);
		static void DrawBox( int x, int y, int w, int h, float r, float g, float b, float a );
		static void DrawBox( Coordinate p, int w, int h, Color c, float a = 1.0f);
		static void DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a = 1.0f);
		static void DrawCircle( Coordinate c, int radius, float line_width, Color col, float a = 1.0f);
		static void DrawFilledCircle( int x, int y, int radius, Color c, float a = 1.0f);
		static void DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a = 1.0f);
		static void DrawFilledCircle( Coordinate, int radius, Color c, float a = 1.0f);
		static void DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b );

		static void SetCropRect( int x, int y, int w, int h );
		static void UnsetCropRect( void );
		
		static void Blur( void );

		static Image *CaptureScreen( void );
		static void SaveScreenshot( const string filename );

		// Lua functions
		static int lua_getWidth(lua_State *L);
		static int lua_getHeight(lua_State *L);

 	private:
  		static int w, h; // width/height of screen
		static int w2, h2; // width/height divided by 2
		static stack<Rect> cropRects;
		static SDL_Surface *screen; // pointer to main video surface
};

#endif // __H_VIDEO__

