/*
 * Filename      : video.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "common.h"
#include "Graphics/video.h"
#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

Video *Video::pInstance = 0; // initialize pointer
int Video::w = 0;
int Video::h = 0;
int Video::w2 = 0;
int Video::h2 = 0;
GLuint	Video::motionBlur_Texture;				// The Texture
int Video::motionBlur_Texture_X=0;					// X Coordonate
int	Video::motionBlur_Texture_Y=0;					// Y Coordonate
int	Video::motionBlur_Texture_channels=0;			// Number of Chanels
int	Video::motionBlur_Texture_type=0;				// The Type



Video *Video::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Video; // create the sold instance
	}

	return( pInstance );
}

Video::Video( void ) {
	
}

bool Video::Initialize( void ) {
	char buf[32] = {0};
	
	// initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		Log::Error( "Could not initialize SDL: %s", SDL_GetError() );
		return( false );
	} else {
		Log::Message( "SDL video initialized using %s driver.", SDL_VideoDriverName( buf, 31 ) );
	}

	atexit( SDL_Quit );
	
	DisableMouse();
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	return( true );
}

bool Video::Shutdown( void ) {
	
	EnableMouse();

	return( true );
}

Video::~Video( void ){
	pInstance = NULL;
}

bool Video::SetWindow( int w, int h, int bpp ) {
	const SDL_VideoInfo *videoInfo; // handle to SDL video information
	Uint32 videoFlags = 0; // bitmask to pass to SDL_SetVideoMode()
	SDL_Surface *screen = NULL; // pointer to main video surface
	int ret = 0;

	// get information about the video card (namely, does it support
	// hardware surfaces?)
	videoInfo = SDL_GetVideoInfo();
	if(! videoInfo )
		Log::Warning( "SDL_GetVideoInfo() returned NULL" );

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
		Log::Message( "Using hardware surfaces." );
	} else {
		videoFlags |= SDL_SWSURFACE;
		Log::Message( "Not using hardware surfaces." );
	}

	// using SDL given information, set hardware acceleration if supported
	if( videoInfo->blit_hw ) {
		videoFlags |= SDL_HWACCEL;
		Log::Message( "Using hardware accelerated blitting." );
	} else {
		Log::Message( "Not using hardware accelerated blitting." );
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync

	ret = SDL_VideoModeOK( w, h, bpp, videoFlags );
	if( !ret ) {
		Log::Warning( "Video mode %dx%dx%d not available.", w, h, bpp );
	} else {
		Log::Message( "Video mode %dx%dx%d supported.", w, h, bpp );
	}

	// finally, set the video mode (creating a window)
	if( ( screen = SDL_SetVideoMode( w, h, bpp, videoFlags ) ) == NULL ) {
		Log::Error( "Could not set video mode: %s", SDL_GetError() );
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

	Log::Message( "Video mode initialized at %dx%dx%d\n", screen->w, screen->h, screen->format->BitsPerPixel );

	return( true );
}

void Video::Update( void ) {
	glFlush();
	SDL_GL_SwapBuffers();
	glFinish();
}

void Video::Erase( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
}

int Video::GetWidth( void ) {
	return( w );
}

int Video::GetHeight( void ) {
	return( h );
}

// draws a point, a single pixel, on the screen
void Video::DrawPoint( int x, int y, float r, float g, float b ) {
	glDisable(GL_TEXTURE_2D);
	glColor3f( r, g, b );
	glRecti( x, y, x + 1, y + 1 );
}

// draws a rectangle
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b ) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f( r, g, b );
	glRecti( x, y, x + w, y + h );
}

// same as above but takes alpha
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glRecti( x, y, x + w, y + h );
}

void Video::DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, r, g, b );
}

void Video::DrawCircle( Coordinate c, int radius, float line_width, Color col) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, col.r, col.g, col.b );
}

void Video::DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b) {
	glDisable(GL_TEXTURE_2D);
	glColor3f(r,g,b);
	glLineWidth(line_width);
	glBegin(GL_LINE_STRIP);
	for(double angle = 0; angle <= 2.5 * M_PI; angle = angle + 0.1)
	{
		glVertex2d(radius * cos(angle) + x, radius * sin(angle) + y);
	}
	glEnd();
}

void Video::DrawFilledCircle( int x, int y, int radius, float r, float g, float b) {
	//TODO: Make this draw a filled circle.
	glColor3f(r,g,b);
	glBegin(GL_LINE_STRIP);
	for(double ang = 0; ang <= 2.5 * M_PI; ang = ang + 0.1)
	{
		glVertex2d(radius * cos(ang) + x, radius * sin(ang) + y);
	}
	glEnd();
}

void Video::EnableMouse( void ) {
	SDL_ShowCursor( 1 );
}

void Video::DisableMouse( void ) {
	SDL_ShowCursor( 0 );
}

int Video::GetHalfWidth( void ) {
	return( w2 );
}

int Video::GetHalfHeight( void ) {
	return( h2 );
}

void Video::MotionBlurTexture ( void ) {
	glLoadIdentity();

	// render the old texture
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, 
			GL_ONE_MINUS_SRC_ALPHA);
			// For experimentation you can use these values too ;)
			//GL_DST_ALPHA);
			//GL_ONE_MINUS_SRC_COLOR );

		// Set the textures customizable color
		glColor4f(1.0f,1.0f,1.0f,0.7f);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

			// put the texture on the screen
			glBindTexture(GL_TEXTURE_2D, Video::motionBlur_Texture);

			// go to orthogonal view
			glLoadIdentity();
			glOrtho( 0, Video::motionBlur_Texture_X, Video::motionBlur_Texture_Y, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_QUADS);
				
				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)Video::motionBlur_Texture_Y);
				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)Video::motionBlur_Texture_X, (GLfloat)Video::motionBlur_Texture_Y);
				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)Video::motionBlur_Texture_X, 0);

			glEnd();

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

	glPopMatrix();

	// copy the whole viewport to the teture
	glBindTexture(GL_TEXTURE_2D, Video::motionBlur_Texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, Video::motionBlur_Texture_type, 0, 0, 
		Video::motionBlur_Texture_X, Video::motionBlur_Texture_Y, 0);

	// End render texture code
//	glClearColor(0.0,0.0,0.0,0.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
//	glLoadIdentity();

//	// put the texture on the screen
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//
//		glMatrixMode(GL_PROJECTION);
//		glPushMatrix();
//			// put the result onto the screen
//			glBindTexture(GL_TEXTURE_2D, Video::motionBlur_Texture);
//
//			// go to orthogonal view
//			glLoadIdentity();
//			glOrtho(0, w, h, 0, 0, 1);
//			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
//
//			glBegin(GL_QUADS);
//				glColor3f(1.0, 1.0, 1.0);
//
//				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
//				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)h);
//				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)w, (GLfloat)h);
//				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)w, 0);
//
//			glEnd();
//
//			glMatrixMode(GL_PROJECTION);
//		glPopMatrix();
//
//		glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
}

#define Texture_Interpolation	1
#define Texture_Near			0
int Video::CreateMotionBlurTexture(int quality, int interpolation) {	// Creates the motion blur texture
	int filter;

			Video::motionBlur_Texture_X = Video::w;
			Video::motionBlur_Texture_Y = Video::h;
			
			Video::motionBlur_Texture_channels = 3;
			Video::motionBlur_Texture_type = GL_RGB;

	switch(interpolation) {
		case Texture_Interpolation :
			filter = GL_LINEAR;
			break;
		case Texture_Near:
			filter = GL_NEAREST;
			break;
	}

	return GenerateTexture(Video::motionBlur_Texture, 
		Video::motionBlur_Texture_X, 
		Video::motionBlur_Texture_Y,			
		Video::motionBlur_Texture_channels, 
		Video::motionBlur_Texture_type, 
		filter, filter);
}

int Video::GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter) {
	int Status = FALSE;
	unsigned int memoryRequiredSize = SizeX * SizeY * Channels;

	unsigned int *memoryBlock = new unsigned int[memoryRequiredSize];
	if(memoryBlock == NULL)
		return Status;

	ZeroMemory(memoryBlock, memoryRequiredSize);

	Status = GenerateTexture(Texture, SizeX, SizeY, Channels, Format, Min_Filter, Mag_Filter, memoryBlock);
	/* -- deprecated, moved into a other procedure
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, Channels, SizeX, SizeY, Format, GL_UNSIGNED_INT, memoryBlock);
	glTexImage2D(GL_TEXTURE_2D, 0, Channels, SizeX, SizeY, 0, Format, GL_UNSIGNED_INT, memoryBlock);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter); 
*/
	delete [] memoryBlock;

	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}

int Video::GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter, unsigned int *memoryArea) {
	int Status = FALSE;

	if(memoryArea == NULL)
		return Status;

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, Channels, SizeX, SizeY, Format, GL_UNSIGNED_INT, memoryBlock);
	glTexImage2D(GL_TEXTURE_2D, 0, Channels, SizeX, SizeY, 0, Format, GL_UNSIGNED_INT, memoryArea );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter); 


	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}
