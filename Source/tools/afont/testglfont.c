/* testglfont.c */

#ifdef MACOSX
#include "gl.h"
#include "glu.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "SDL.h"

#include "afont_gl.h"

int main( int argc, char **argv )
{
  afont *a;
  afontgl *ag, *ag2;
  SDL_Event event;
  SDL_Surface *screen;
  Uint32 videoflags;
  int quit = 0;

  if(argc < 2) {
    fprintf(stderr, "Usage: %s [font file]\n", argv[0]);
    return 0;
  }

  /* Init SDL */
  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);

	int bpp = 16;

	const SDL_VideoInfo *videoInfo; // handle to SDL video information
	Uint32 videoFlags = 0; // bitmask to pass to SDL_SetVideoMode()

	// get information about the video card (namely, does it support
	// hardware surfaces?)
	videoInfo = SDL_GetVideoInfo();

	// enable OpenGL and various other options
	videoFlags = SDL_OPENGL;
	videoFlags |= SDL_GL_DOUBLEBUFFER;
	videoFlags |= SDL_HWPALETTE;

	// using SDL given information, use hardware surfaces if supported by
	// the video card
	if( videoInfo->hw_available ) {
		videoFlags |= SDL_HWSURFACE;
	} else {
		videoFlags |= SDL_SWSURFACE;
	}

	// using SDL given information, set hardware acceleration if supported
	if( videoInfo->blit_hw ) {
		videoFlags |= SDL_HWACCEL;
	} else {
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	bpp = SDL_VideoModeOK( 640, 480, bpp, videoFlags );
	if( !bpp ) {
		bpp = 32;
	} else {
	}

	// finally, set the video mode (creating a window)
	if( ( screen = SDL_SetVideoMode( 640, 480, bpp, videoFlags ) ) == NULL ) {
		return( -1 );
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

  //glDepthFunc( GL_LESS );
  //glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set up a pseudo-2D viewpoint
	glViewport( 0, 0, 640, 480 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 640, 480, 0, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

  ag = afont_gl_load(argv[1]);

    ag2 = NULL;

  while(!quit) {
    char buf[128];
    
    /* Draw the screen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity( );

    glColor4f(1.0, 1.0, 0.0, 0.5 + 
            0.5 * sin(((float)SDL_GetTicks()) / 1000.0));
    glRasterPos2f(50., 50.);
    sprintf(buf, "The time is: %d", SDL_GetTicks());
    afont_gl_render_text(ag, buf);

    SDL_GL_SwapBuffers();
    SDL_Delay(20);

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_KEYDOWN)
        quit = 1;
    }
 
  }

  return 0;
}
