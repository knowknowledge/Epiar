/* afont_gl.h */

/* See the file LICENSE for copyright and license information */

#ifndef _afont_gl_h
#define _afont_gl_h

#include "includes.h"
#include "Utilities/file.h"
#include "afont.h"

typedef struct afontgl afontgl;

struct afontgl {
  afont_type type;
  int nchars;
  int base;
  GLuint baselist;
  afont *orig;
};

afontgl *afont_gl_load( const char *path );
afontgl *afont_gl_load_fp( File& file );
afontgl *afont_gl_convert( afont *a );
void afont_gl_free( afontgl *ag );
void afont_gl_render_text( afontgl *a, const char *text );
  /* Render a string of text, setting up the pixel transfer options
   * appropriately to display in the current color. */

#endif
