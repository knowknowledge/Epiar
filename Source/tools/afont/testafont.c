/* testafont.c */

#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "SDL.h"

#include "afont_sdl.h"

enum {
  STATE_TEST_SOLID,
  STATE_TEST_SHADED,
  STATE_TEST_BLENDED,
  STATE_TEST_ALPHA,

  STATE_TEST_BG,

  STATE_TEST_SURFACE,

  STATE_QUIT
};

int main( int argc, char **argv )
{
  SDL_Surface *screen;
  afont *a;
  int i;
  int state = STATE_TEST_SOLID;
  SDL_Event event;
  SDL_Surface *tmps = NULL;
  SDL_PixelFormat fmt;

  if(argc != 2) {
    fprintf(stderr, "Usage: %s [font file]\n", argv[0]);
    return 0;
  }

  /* Init SDL */
  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);

  a = afont_load(argv[1]);

  screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
  assert(screen);

  /* Main loop */
  while(state != STATE_QUIT) {
    char buf[128];
    Uint8 r, g, b;
    SDL_Rect dstrect;
    
    /* Draw the screen */
    switch(state) {
      case STATE_TEST_SOLID:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered using AFONT_RENDER_SOLID");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        afont_render_text(a, 0x55b0ffff, 0x000000ff, AFONT_RENDER_SOLID,
                screen, 200, 200, buf);
        break;

      case STATE_TEST_SHADED:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered using AFONT_RENDER_SHADED");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        afont_render_text(a, 0x55b0ffff, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 200, 200, buf);
        break;
      
      case STATE_TEST_BLENDED:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered using AFONT_RENDER_BLENDED");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        afont_render_text(a, 0x55b0ffff, 0x000000ff, AFONT_RENDER_BLENDED,
                screen, 200, 200, buf);
        break;

      case STATE_TEST_ALPHA:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered using AFONT_RENDER_ALPHA");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        fmt.Rmask = 0xff000000;
        fmt.Gmask = 0x00ff0000;
        fmt.Bmask = 0x0000ff00;
        fmt.Amask = 0x000000ff;
        fmt.BitsPerPixel=32;
        tmps = afont_render_text_surf(a, 0x55b0ffff, 0x000000ff, 
                AFONT_RENDER_ALPHA, &fmt, buf);
        dstrect.x = 200; dstrect.y = 200;
        SDL_BlitSurface(tmps, NULL, screen, &dstrect);
        break;

      case STATE_TEST_BG:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered with a background");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        r = (Uint8)(128 + 127 * sin((float)SDL_GetTicks() / 2000.0));
        g = (Uint8)(128 + 127 * sin((float)SDL_GetTicks() / 2000.0 + 1.0));
        b = (Uint8)(128 + 127 * sin((float)SDL_GetTicks() / 2000.0 + 2.0));
        afont_render_text_bg(a, 0x55b0ffff, 
                (r << 24) | (g << 16) | (b << 8) | 0xff,
                AFONT_RENDER_SHADED,
                screen, 200, 200, buf);
        break;
     
      case STATE_TEST_SURFACE:
        SDL_FillRect(screen, NULL, 0);
        afont_render_text(a, 0x50ff50, 0x000000ff, AFONT_RENDER_SHADED,
                screen, 0, 50,
                "Text rendered onto a surface");
        sprintf(buf, "The time is: %d", SDL_GetTicks());
        tmps = afont_render_text_surf(a, 0x55b0ffff, 0x000000ff, 
                AFONT_RENDER_SHADED, screen->format, buf);
        dstrect.x = 200; dstrect.y = 200;
        SDL_BlitSurface(tmps, NULL, screen, &dstrect);
        break;
    }

    SDL_Flip(screen);
    SDL_Delay(200);

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_KEYDOWN)
        state++;
    }
  }

  afont_free(a);
  
  return 0;
}
