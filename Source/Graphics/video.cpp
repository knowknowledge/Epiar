/**\file			video.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"
#include "Utilities/trig.h"

/**\class Color
 * \var Color::r
 *  \brief Red component
 */
/**\var Color::g
 *  \brief Green component
 */
/** \var Color::b
 *  \brief Blue component
 */

/** \brief Default Color Constructor
 *  \details Defaults to White
 */
Color::Color()
{
	r = g = b = 1.0f;
}

/** \brief Color Assignment Opererator
 */
Color& Color::operator=(Color other)
{
	r = other.r;
	g = other.g;
	b = other.b;
}

/** \brief Color Constructor from Integers
 *  \param[in] red Red percentage as int between 0x00 and 0xFF.
 *  \param[in] green Green percentage as int between 0x00 and 0xFF.
 *  \param[in] blue Blue percentage as int between 0x00 and 0xFF.
 */
Color::Color( int red, int green, int blue )
{
	r = static_cast<float> (red   / 255.0f);
	g = static_cast<float> (green / 255.0f);
	b = static_cast<float> (blue  / 255.0f);
}

/** \brief Color Constructor from Floats
 *  \param[in] red Red percentage as int between 0.0f and 1.0f.
 *  \param[in] green Green percentage as int between 0.0f and 1.0f.
 *  \param[in] blue Blue percentage as int between 0.0f and 1.0f.
 */
Color::Color( float red, float green, float blue )
{
	r = red;
	g = green;
	b = blue;
}

/** \brief Color Constructor from Hex String
 *  \param[in] str A string of the form RRGGBB where RR, GG, and BB are the
 *  hex values of the Red, Green and Blue components of the color.
 *  This can be optionally preceeded by "0x" or "0X".
 *
 *  \example "0xFF0000" is Red
 */
Color::Color( string str )
{
	// string must be in hex format.
	int n;
	stringstream ss;
	ss << std::hex << str;
	ss >> n;

	r = ((n >> 16) & 0xFF ) / 255.0f;
	g = ((n >>  8) & 0xFF ) / 255.0f;
	b = ((n      ) & 0xFF ) / 255.0f;
}

/**\class Rect
 * \brief Wrapper for a rectangle.
 * \fn Rect::Rect()
 *  \brief Creates a rectangle with 0 for x,y, width, and height.
 * \fn Rect::Rect( float x, float y, float w, float h )
 *  \brief Creates a rectangle with specified dimensions.
 * \fn Rect::Rect( int x, int y, int w, int h )
 *  \brief Creates a rectangle with specified dimensions. (float version)
 * \var Rect::x
 *  \brief x coordinate
 */
/**\var Rect::y
 *  \brief y coordinate
 */
/**\var Rect::w
 *  \brief width
 */
/**\var Rect::h
 *  \brief height
 */

/**\class Video
 * \brief Video handling. */

int Video::w = 0;
int Video::h = 0;
int Video::w2 = 0;
int Video::h2 = 0;
stack<Rect> Video::cropRects;

/**\brief Initializes the Video display.
 */
bool Video::Initialize( void ) {
	char buf[32] = {0};
	
	// initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		LogMsg(ERR, "Could not initialize SDL: %s", SDL_GetError() );
		return( false );
	} else {
		LogMsg(INFO, "SDL video initialized using %s driver.", SDL_VideoDriverName( buf, 31 ) );
	}

	atexit( SDL_Quit );
	
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	int w = OPTION( int, "options/video/w" );
	int h = OPTION( int, "options/video/h" );
	bool fullscreen = OPTION( bool, "options/video/fullscreen" );

	// If w/h not set, then 1024x768 is windowed default, native screen resolution is fullscreen default
	if( !w && !h ) {
		if( OPTION( int, "options/video/fullscreen" ) ) {
			// fullscreen set, use native resolution
			const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

			w = videoInfo->current_w;
			h = videoInfo->current_h;
		} else {
			// not fullscreen
			w = 1024;
			h = 768;
		}
	}
	

	Video::SetWindow( w, h, OPTION( int, "options/video/bpp"), fullscreen );
	
	return( true );
}

/**\brief Shuts down the Video display.
 */
bool Video::Shutdown( void ) {
	
	EnableMouse();

	return( true );
}

/**\brief Sets the window properties.
 */
bool Video::SetWindow( int w, int h, int bpp, bool fullscreen ) {
	const SDL_VideoInfo *videoInfo; // handle to SDL video information
	Uint32 videoFlags = 0; // bitmask to pass to SDL_SetVideoMode()
	SDL_Surface *screen = NULL; // pointer to main video surface
	int ret = 0;

	// get information about the video card (namely, does it support
	// hardware surfaces?)
	videoInfo = SDL_GetVideoInfo();
	if(! videoInfo )
		LogMsg(WARN, "SDL_GetVideoInfo() returned NULL" );

	// enable OpenGL and various other options
	videoFlags = SDL_OPENGL;
	videoFlags |= SDL_GL_DOUBLEBUFFER;
	videoFlags |= SDL_HWPALETTE;

	// enable fullscreen if set (see main.cpp, parseOptions)
	if( fullscreen ) videoFlags |= SDL_FULLSCREEN;

	// using SDL given information, use hardware surfaces if supported by
	// the video card
	if( videoInfo->hw_available ) {
		videoFlags |= SDL_HWSURFACE;
		LogMsg(INFO, "Using hardware surfaces." );
	} else {
		videoFlags |= SDL_SWSURFACE;
		LogMsg(INFO, "Not using hardware surfaces." );
	}

	// using SDL given information, set hardware acceleration if supported
	if( videoInfo->blit_hw ) {
		videoFlags |= SDL_HWACCEL;
		LogMsg(INFO, "Using hardware accelerated blitting." );
	} else {
		LogMsg(INFO, "Not using hardware accelerated blitting." );
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync

	ret = SDL_VideoModeOK( w, h, bpp, videoFlags );
	if( !ret ) {
		LogMsg(WARN, "Video mode %dx%dx%d not available.", w, h, bpp );
	} else {
		LogMsg(INFO, "Video mode %dx%dx%d supported.", w, h, bpp );
	}

	// finally, set the video mode (creating a window)
	if( ( screen = SDL_SetVideoMode( w, h, bpp, videoFlags ) ) == NULL ) {
		LogMsg(ERR, "Could not set video mode: %s", SDL_GetError() );
		return( false );
	}

	// set window title
	SDL_WM_SetCaption("Epiar", "Epiar");

	// set up some needed opengl facilities
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// for motion blur
	glClearAccum(0.0, 0.0, 0.0, 1.0);
	glClear(GL_ACCUM_BUFFER_BIT);

	// set up a pseudo-2D viewpoint
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, w, h, 0, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	Video::w = w;
	Video::h = h;

	// compute the half dimensions
	w2 = w / 2;
	h2 = h / 2;

	LogMsg(INFO, "Video mode initialized at %dx%dx%d\n", screen->w, screen->h, screen->format->BitsPerPixel );

	return( true );
}

/**\brief Register Lua functions for Video related operations.
 */
void Video::RegisterVideo(lua_State *L) {
	static const luaL_Reg videoFunctions[] = {
		{"getWidth", &Video::lua_getWidth},
		{"getHeight", &Video::lua_getHeight},
		{NULL, NULL}
	};

	luaL_openlib(L, EPIAR_VIDEO, videoFunctions, 0);

	lua_pop(L,1);
}

/**\brief Same as Video::GetWidth (Lua callable)
 */
int Video::lua_getWidth(lua_State *L) {
	lua_pushinteger(L, GetWidth() );
	return 1;
}

/**\brief Same as Video::GetHeight (Lua callable)
 */
int Video::lua_getHeight(lua_State *L) {
	lua_pushinteger(L, GetHeight() );
	return 1;
}

/**\brief Video updates.
 */
void Video::Update( void ) {
	//Blur();

	glFlush();
	SDL_GL_SwapBuffers();
	glFinish();
}

void Video::Blur( void ) {
	float q = .6f;

	glAccum(GL_MULT, q);
	glAccum(GL_ACCUM, 1.0f - q);
	glAccum(GL_RETURN, 1.0f);
}

/**\brief Clears screen.
 */
void Video::Erase( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT );
	glLoadIdentity();
}

/**\brief Returns the width of the screen.
 */
int Video::GetWidth( void ) {
	return( w );
}

/**\brief Returns the height of the screen.
 */
int Video::GetHeight( void ) {
	return( h );
}

/**\brief draws a point, a single pixel, on the screen
 */
void Video::DrawPoint( int x, int y, float r, float g, float b ) {
	glDisable(GL_TEXTURE_2D);
	glColor3f( r, g, b );
	glRecti( x, y, x + 1, y + 1 );
}

/**\brief Draw a point using Coordinate and Color.
 */
void Video::DrawPoint( Coordinate c, Color col ) {
	DrawPoint( (int)c.GetX(), (int)c.GetY(), col.r, col.g, col.b );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( Coordinate p1, Coordinate p2, Color c, float a ) {
	DrawLine( p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, Color c, float a ) {
	DrawLine( x1, y1, x2, y2, c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a ) {
	glColor4f( r, g, b, a );
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}


/**\brief Draws a filled rectangle
 */
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glRecti( x, y, x + w, y + h );
}

void Video::DrawRect( int x, int y, int w, int h, Color c, float a ) {
	DrawRect( x, y, w, h, c.r, c.g, c.b, a );
}

void Video::DrawRect( Coordinate p, int w, int h, Color c, float a ) {
	DrawRect( p.GetX(), p.GetY(), w, h, c.r, c.g, c.b, a );
}

void Video::DrawBox( int x, int y, int w, int h, Color c, float a ) {
	DrawBox( x, y, w, h, c.r, c.g, c.b, a );
}

/**\brief Draws an unfilled rectangle
 */
void Video::DrawBox( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glBegin(GL_LINE_STRIP);
	glVertex2d(x,y);
	glVertex2d(x+w,y);
	glVertex2d(x+w,y+h);
	glVertex2d(x,y+h);
	glVertex2d(x,y);
	glEnd();

}

/**\brief Draws a circle.
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, r, g, b, a );
}

/**\brief Draws a circle with Color
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, Color col, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, col.r, col.g, col.b, a);
}

/**\brief Draws a circle
 */
void Video::DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a) {
	glDisable(GL_TEXTURE_2D);
	glColor4f( r, g, b, a );
	glLineWidth(line_width);
	glBegin(GL_LINE_STRIP);
	Trig* t = Trig::Instance();
	for(int angle = 0; angle < 360; angle += 5)
	{
		glVertex2d(radius * t->GetCos(angle) + x, radius * t->GetSin(angle) + y);
	}
	// One more point to finish the circle. (ang=0)
	glVertex2d(radius + x, y);
	glEnd();
	// Reset Line Width
	glLineWidth(1);
}

void Video::DrawFilledCircle( Coordinate p, int radius, Color c, float a) {
	DrawFilledCircle( p.GetX(), p.GetY(), radius, c.r, c.g, c.b, a );
}

void Video::DrawFilledCircle( int x, int y, int radius, Color c, float a) {
	DrawFilledCircle( x, y, radius, c.r, c.g, c.b, a );
}

/**\brief Draw a filled circle.
 */
void Video::DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a) {
	glColor4f(r,g,b,a);
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);
	Trig* t = Trig::Instance();
	for(int angle = 0; angle < 360; angle += 5)
	{
		glVertex2d(x,y);
		glVertex2d(radius * t->GetCos(angle) + x, radius * t->GetSin(angle) + y);
	}
	// One more triangle to finish the circle. (ang=0)
	glVertex2d(x,y);
	glVertex2d(radius + x, y);
	glEnd();
}

/**\brief Draws a targeting overlay.
 */
void Video::DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b ) {
	// d is for 'depth' and is the number of crosshair pixels
	glColor3f(r,g,b);
	glBegin(GL_LINES);
		// Upper Left Corner
		glVertex2d(x-w/2,y-h/2); glVertex2d(x-w/2,y-h/2+d);
		glVertex2d(x-w/2,y-h/2); glVertex2d(x-w/2+d,y-h/2);
		// Upper Right Corner
		glVertex2d(x+w/2,y-h/2); glVertex2d(x+w/2,y-h/2+d);
		glVertex2d(x+w/2,y-h/2); glVertex2d(x+w/2-d,y-h/2);
		// Lower Left Corner
		glVertex2d(x-w/2,y+h/2); glVertex2d(x-w/2,y+h/2-d);
		glVertex2d(x-w/2,y+h/2); glVertex2d(x-w/2+d,y+h/2);
		// Lower Right Corner
		glVertex2d(x+w/2,y+h/2); glVertex2d(x+w/2,y+h/2-d);
		glVertex2d(x+w/2,y+h/2); glVertex2d(x+w/2-d,y+h/2);
	glEnd();
}

/**\brief Enables the mouse
 */
void Video::EnableMouse( void ) {
	SDL_ShowCursor( 1 );
}

/**\brief Disables the mouse
 */
void Video::DisableMouse( void ) {
	SDL_ShowCursor( 0 );
}

/**\brief Returns the half width.
 */
int Video::GetHalfWidth( void ) {
	return( w2 );
}

/**\brief Returns the half height.
 */
int Video::GetHalfHeight( void ) {
	return( h2 );
}

/**\brief Set crop rectangle.
 */
void Video::SetCropRect( int x, int y, int w, int h ){
	int xn, yn, wn, hn;

	if (cropRects.empty()) {
		glEnable(GL_SCISSOR_TEST);

		xn = x;
		yn = y;
		wn = w;
		hn = h;
	} else {
		// Need to detect which part of crop rectangle is within previous rectangle
		// So we don't miss things that needs to be cropped.
		Rect prevrect = cropRects.top();

		int rightprev = TO_INT(prevrect.x + prevrect.w);
		int right = x + w;
		int botprev = TO_INT(prevrect.y + prevrect.h);
		int bot = y + h;

		xn = prevrect.x > x ? TO_INT(prevrect.x) : x;	       // Left
		yn = prevrect.y > y ? TO_INT(prevrect.y) : y;	       // Top
		wn = rightprev > right ? right - xn : rightprev - xn;  // Right
		hn = botprev > bot ? bot - yn : botprev - yn;		   // Bottom

		if(wn < 0) wn = 0; // crops don't overlap. we still record this as they're going to call unsetcrop, unaware of the issue
		if(hn < 0) hn = 0; // same reason as line above
	}

	cropRects.push(Rect( xn, yn, wn, hn ));

	// Need to convert top down y-axis
	glScissor( xn, Video::h - (yn + hn), wn, hn );
}

/**\brief Unset the previous crop rectangle after use.
 */
void Video::UnsetCropRect( void ) {
	if (!cropRects.empty()) // Shouldn't be empty
		cropRects.pop();
	else
		LogMsg(WARN,"You unset the crop rect too many times.");

	if ( cropRects.empty() ) {
		glDisable(GL_SCISSOR_TEST);
	} else {
		// Set's the previous crop rectangle.
		Rect prevrect = cropRects.top();

		glScissor( TO_INT(prevrect.x), Video::h - (TO_INT(prevrect.y) + TO_INT(prevrect.h)), TO_INT(prevrect.w), TO_INT(prevrect.h) );
	}
}
