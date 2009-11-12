/* afont_sdl.h */

/* See the file LICENSE for copyright and license information */

#ifndef _afont_sdl_h
#define _afont_sdl_h

#include "SDL.h"

#include "afont.h"

typedef enum {
  AFONT_RENDER_SOLID,      /* Render solid (no antialiasing) */
  AFONT_RENDER_SHADED,     /* Render with antialiasing, blended into the
                            * given background color */
  AFONT_RENDER_BLENDED,    /* Render with antialiasing, blended into the
                            * surface pixels */
  AFONT_RENDER_ALPHA       /* Render with antialiasing, creating
                            * alpha-valued pixels */
} afont_render_mode;

void afont_render_char( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode, Uint32 *palette,
        SDL_Surface *s, int x, int y,
        char c );
  /* Render one character of text.
   *
   * a == font to use
   * fg == foreground color (in 0xrrggbbaa format)
   * bg == background color (for AFONT_RENDER_SHADED)
   * mode == render mode -- see notes by afont_render_mode definition
   * palette == an array of 256 colors to use as the palette, for the
   *   AFONT_RENDER_SHADED and AFONT_RENDER_ALPHA rendering modes.  Creating
   *   this in advance saves time in computing these values on the fly,
   *   which is done if palette == NULL.  The afont_render_text functions do
   *   this as appropriate.
   * s == surface to render to
   * x == x position of starting pen position
   * y == y position of starting pen position
   * c == character to render
   *
   * Note that the pen position is a starting point on the baseline of the
   * text; the actual location that the character bitmap is copied to
   * depends on the font and the character being rendered. 
   *
   * Also note that the background color only affects pixels that are
   * partially but not fully transparent.  See afont_render_text_bg()
   * for a way to fill all other pixels with the background, or do it 
   * before calling this function. */

void afont_render_text( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_Surface *s, int x, int y,
        char *text );
  /* Render a string of text; passes each character to afont_render_char. */

void afont_render_text_bg( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_Surface *s, int x, int y,
        char *text );
  /* Before rendering the text, fill in a rectangle of color bg around the
   * bounding box of the string. */

SDL_Surface *afont_render_text_surf( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_PixelFormat *fmt,
        char *text );
  /* Create a new surface, and render the text onto that, rather than
   * rendering to a surface that has already been allocated. */

#endif
