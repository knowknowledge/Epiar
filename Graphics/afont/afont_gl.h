/* afont_gl.h */

/* See the file LICENSE for copyright and license information */

#ifndef _afont_gl_h
#define _afont_gl_h

#include "afont.h"

void afont_gl_make_bitmaps( afont *a );
  /* Convert the bitmaps loaded by afont_load into a format that can be used
   * by the gl rendering functions.  (Specifically, pads lines to 32-bit
   * boundaries and reverses the order of the lines to a bottom-to-top order
   * in memory.) 
   * This doesn't play nicely with SDL font rendering; should you want to
   * use both, you need to load two copies of the font. */
void afont_gl_render_char( afont *a, char c );
  /* Render a single character at the current raster position.  The raster
   * position will be updated to the appropriate pen position for another
   * character. */
void afont_gl_render_text( afont *a, char *text );
  /* Render a string of text via repeated calls to afont_render_char */

#endif
