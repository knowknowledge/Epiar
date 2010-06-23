/**\file			video.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

/**\class Color
 * \brief Wrapper for RGB color.
 * \fn Color::Get( int r, int g, int b)
 *  \brief Creates the Color object.
 * \fn Color::Get( float r, float g, float b )
 *  \brief Creates the Color object (float version)
 * \var Color::r
 *  \brief Red component
 */
/**\var Color::g
 *  \brief Green component
 */
/** \var Color::b
 *  \brief Blue component
 */

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

Video *Video::pInstance = 0; // initialize pointer
int Video::w = 0;
int Video::h = 0;
int Video::w2 = 0;
int Video::h2 = 0;
stack<Rect> Video::cropRects;

/**\brief Creates the singleton or retrieves the current instance.
 */
Video *Video::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Video; // create the sold instance
	}

	return( pInstance );
}

/**\brief Empty constructor.
 */
Video::Video( void ) {
	
}

/**\fn Video::Video( const Video& )
 *  \brief Empty copy constructor.
 * \fn Video::operator= (const Video&)
 *  \brief Empty assignment operator.
 */


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
	
	return( true );
}

/**\brief Shuts down the Video display.
 */
bool Video::Shutdown( void ) {
	
	EnableMouse();

	return( true );
}

/**\brief Releases the pointer.
 */
Video::~Video( void ){
	pInstance = NULL;
}

/**\brief Sets the window properties.
 */
bool Video::SetWindow( int w, int h, int bpp ) {
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
	if( OPTION( int, "options/video/fullscreen" ) )
		videoFlags |= SDL_FULLSCREEN;

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
	SDL_WM_SetCaption("Epiar","Epiar");

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

/**\brief Video updates.
 */
void Video::Update( void ) {
	glFlush();
	SDL_GL_SwapBuffers();
	glFinish();
}

/**\brief Clears screen.
 */
void Video::Erase( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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

void Video::DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a ) {
	glColor4f( r, g, b, a );
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}

/**\brief Draws a rectangle
 */
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b ) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f( r, g, b );
	glRecti( x, y, x + w, y + h );
}

/**\brief Draws a rectangle with transparency
 */
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glRecti( x, y, x + w, y + h );
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
	for(double angle = 0; angle <= 2.0 * M_PI; angle = angle + 0.1)
	{
		glVertex2d(radius * cos(angle) + x, radius * sin(angle) + y);
	}
	// One more point to finish the circle. (ang=0)
	glVertex2d(radius + x, y);
	glEnd();
}

/**\brief Draw a filled circle.
 */
void Video::DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a) {
	glColor4f(r,g,b,a);
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);
	for(double ang = 0; ang <= 2.0 * M_PI; ang = ang + 0.1)
	{
		glVertex2d(x,y);
		glVertex2d(radius * cos(ang) + x, radius * sin(ang) + y);
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
	int xn,yn,wn,hn;
	if (cropRects.empty()){
		glEnable(GL_SCISSOR_TEST);
		xn=x;
		yn=y;
		wn=w;
		hn=h;
	}else{
		// Need to detect which part of crop rectangle is within previous rectangle
		// So we don't miss things that needs to be cropped.
		Rect prevrect = cropRects.top();
		int rightprev = TO_INT(prevrect.x+prevrect.w);
		int right = x+w;
		int botprev = TO_INT(prevrect.y+prevrect.h);
		int bot = y+h;
		xn = prevrect.x > x ? TO_INT(prevrect.x) : x;	// Left
		yn = prevrect.y > y ? TO_INT(prevrect.y) : y;	// Top
		wn = rightprev > right ? right-xn : rightprev-xn;// Right
		hn = botprev > bot ? bot-yn : botprev-yn;		// Bottom
	}

	cropRects.push(Rect( xn, yn, wn, hn ));
	// Need to convert top down y-axis
	glScissor( x, Video::h-(y+h), w, h );
}

/**\brief Unset the previous crop rectangle after use.
 */
void Video::UnsetCropRect( void ){
	if (!cropRects.empty()) // Shouldn't be empty
		cropRects.pop();
	else
		LogMsg(WARN,"You unset the crop rect too many times.");

	if ( cropRects.empty() )
		glDisable(GL_SCISSOR_TEST);
	else{	//Set's the previous crop rectangle.
		Rect prevrect = cropRects.top();
		glScissor( TO_INT(prevrect.x), TO_INT(prevrect.y), 
				TO_INT(prevrect.w), TO_INT(prevrect.h) );
	}
}
