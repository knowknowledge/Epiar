/* afont_gl.h */

/* See the file LICENSE for copyright and license information */

#ifndef _afont_gl_h
#define _afont_gl_h

#include <stdio.h>
#include <GL/gl.h>

#include "afont.h"

typedef struct afontgl afontgl;

struct afontgl {
  afont_type type;
  int nchars;
  int base;
  GLuint baselist;
  afont *orig;
};

afontgl *afont_gl_load( char *path );
afontgl *afont_gl_load_fp( FILE *fp );
afontgl *afont_gl_convert( afont *a );
void afont_gl_free( afontgl *ag );
void afont_gl_render_text( afontgl *a, const char *text );
  /* Render a string of text, setting up the pixel transfer options
   * appropriately to display in the current color. */

#endif
