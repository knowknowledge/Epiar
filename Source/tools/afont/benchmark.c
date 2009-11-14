/* benchmark.c */

/* Note: afont's blended mode is not really analogous to SDL_ttf's blended
 * mode -- SDL_ttf's is actually closer to afont's AFONT_RENDER_ALPHA mode.
 * SDL_ttf doesn't have anything that works like AFONT_RENDER_BLENDED */

#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "afont_sdl.h"

#define AFONT "verabi24.af"
#define TTFFONT "VeraBI.ttf"
#define TTFSIZE 24

int main( int argc, char **argv )
{
  SDL_Surface *screen;
  afont *a;
  TTF_Font *font;
  int i;
  SDL_Surface *tmps;
  Uint32 start, end;
  Uint32 ttf_solid, ttf_shaded, ttf_blended;
  Uint32 afont_solid, afont_shaded, afont_blended, afont_surf;
  char *buf = "The quick brown fox jumped over the lazy dog.";
  SDL_Color ttffg = { 0xff, 0xff, 0xff, 0xff };
  SDL_Color ttfbg = { 0xff, 0xff, 0xff, 0xff };
  Uint32 afontfg = 0xffffffff;
  SDL_Rect dst;

  dst.x = dst.y = 0;

  /* Init SDL */
  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);

  TTF_Init();
  font = TTF_OpenFont(TTFFONT, TTFSIZE);

  a = afont_load(AFONT);

  screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
  assert(screen);

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    tmps = TTF_RenderText_Solid(font, buf, ttffg);
    SDL_BlitSurface(tmps, NULL, screen, &dst);
    SDL_FreeSurface(tmps);
  }
  end = SDL_GetTicks();
  ttf_solid = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    tmps = TTF_RenderText_Shaded(font, buf, ttffg, ttfbg);
    SDL_BlitSurface(tmps, NULL, screen, &dst);
    SDL_FreeSurface(tmps);
  }
  end = SDL_GetTicks();
  ttf_shaded = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    tmps = TTF_RenderText_Blended(font, buf, ttffg);
    SDL_BlitSurface(tmps, NULL, screen, &dst);
    SDL_FreeSurface(tmps);
  }
  end = SDL_GetTicks();
  ttf_blended = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    afont_render_text(a, afontfg, 0xff, AFONT_RENDER_SOLID,
            screen, 20, 50, buf);
  }
  end = SDL_GetTicks();
  afont_solid = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    afont_render_text(a, afontfg, 0xff, AFONT_RENDER_SHADED,
            screen, 20, 50, buf);
  }
  end = SDL_GetTicks();
  afont_shaded = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    afont_render_text(a, afontfg, 0xff, AFONT_RENDER_BLENDED,
            screen, 20, 50, buf);
  }
  end = SDL_GetTicks();
  afont_blended = end - start;

  start = SDL_GetTicks();
  for(i = 0; i < 1000; i++) {
    tmps = afont_render_text_surf(a, afontfg, 0xff, AFONT_RENDER_SHADED,
            screen->format, buf);
    SDL_BlitSurface(tmps, NULL, screen, &dst);
    SDL_FreeSurface(tmps);
  }
  end = SDL_GetTicks();
  afont_surf = end - start;

  TTF_CloseFont(font);
  TTF_Quit();
  afont_free(a);

  printf("Results:\n");
  printf("SDL_ttf, solid: %d ticks\n", ttf_solid);
  printf("SDL_ttf, shaded: %d ticks\n", ttf_shaded);
  printf("SDL_ttf, blended: %d ticks\n", ttf_blended);
  printf("afont, solid: %d ticks\n", afont_solid);
  printf("afont, shaded: %d ticks\n", afont_shaded);
  printf("afont, blended: %d ticks\n", afont_blended);
  printf("afont, shaded, new surface: %d ticks\n", afont_surf);
  
  return 0;
}
