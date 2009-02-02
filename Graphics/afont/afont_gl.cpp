/* afont_gl.c */

/* See the file LICENSE for copyright and license information */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "afont_gl.h"

static void afont_gl_make_bitmaps( afont *a );
  /* Convert the bitmaps loaded by afont_load into a format that can be used
   * by the gl rendering functions.  (Specifically, pads lines to 32-bit
   * boundaries and reverses the order of the lines to a bottom-to-top order
   * in memory.) 
   * This doesn't play nicely with SDL font rendering; should you want to
   * use both, you need to load two copies of the font. */
static void afont_gl_render_char( afont *a, char c );
  /* Render a single character at the current raster position.  The raster
   * position will be updated to the appropriate pen position for another
   * character. 
   * Used in order to build the display lists. */

afontgl *afont_gl_load( char *path )
{
  afont *a;
  afontgl *ag;

  a = afont_load(path);
  ag = afont_gl_convert(a);
  ag->orig = a;

  return ag;
}

afontgl *afont_gl_load_fp( FILE *fp )
{
  afont *a;
  afontgl *ag;

  a = afont_load_fp(fp);
  ag = afont_gl_convert(a);
  ag->orig = a;

  return ag;
}

afontgl *afont_gl_convert( afont *a )
{
  afontgl *ag;
  int i;

  ag = (afontgl *)malloc(sizeof(afontgl));

  ag->nchars = a->nchars;
  ag->base = a->base;
  ag->baselist = glGenLists(a->nchars);
  afont_gl_make_bitmaps(a);
  ag->type = a->type;

  /* Save the old values of GL_UNPACK_ALIGNMENT and GL_UNPACK_LSB_FIRST */
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

  if(ag->type == AFONT_GL_1BIT) {
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  } else if(ag->type == AFONT_GL_8BIT) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  }

  /* Generate the call lists */
  for(i = 0; i < a->nchars; i++) {
    glNewList(ag->baselist + i, GL_COMPILE);
      afont_gl_render_char(a, i + a->base);
    glEndList();
  }

  /* Restore the saved values */
  glPopClientAttrib();

  return ag;
}

void afont_gl_free( afontgl *ag )
{
  glDeleteLists(ag->baselist, ag->nchars);
  afont_free(ag->orig);
  free(ag);

  return;
}

static void afont_gl_make_bitmaps( afont *a )
{
  if(a->type == AFONT_1BIT) {
    int i, j;
    char *b1, *b2;
    int pitch;
    char tmp;

    for(i = 0; i < a->nchars; i++) {
      pitch = (a->glyph_info[i].bmpx + 7) / 8;
      
      for(b1 = a->glyph_info[i].bmp, 
              b2 = b1 + (a->glyph_info[i].bmpy - 1) * pitch;
              b1 < b2; b1 += pitch, b2 -= pitch) {
        for(j = 0; j < pitch; j++) {
          tmp = b1[j];
          b1[j] = b2[j];
          b2[j] = tmp;
        }
      }
    }

    a->type = AFONT_GL_1BIT;
  } else if(a->type == AFONT_8BIT) {
    int i, j;
    int bufsize;
    char *buf;
    char *fontbufp, *bufp;
    int pitch;

    /* Count how much buffer size will be needed */
    bufsize = 0;
    for(i = 0; i < a->nchars; i++) {
      bufsize += a->glyph_info[i].bmpy * 
              (((a->glyph_info[i].bmpx + 3) / 4) * 4);
    }
    
    /* Create the new bitmap buffer */
    buf = (char *)malloc(bufsize);

    /* Copy the bitmaps into the new buffer */
    for(i = 0, bufp = buf; i < a->nchars; i++) {
      pitch = ((a->glyph_info[i].bmpx + 3) / 4) * 4;
      fontbufp = a->glyph_info[i].bmp + 
              (a->glyph_info[i].bmpy - 1) * a->glyph_info[i].bmpx;

      a->glyph_info[i].bmp = bufp;
    
      /* Copy in reverse order */
      for(j = 0; j < a->glyph_info[i].bmpy; j++) {
        memcpy(bufp, fontbufp, a->glyph_info[i].bmpx);
        fontbufp -= a->glyph_info[i].bmpx;
        bufp += pitch;
      }
    }

    free(a->glyphimg);
    a->glyphimg = buf;

    a->type = AFONT_GL_8BIT;
  }

  return;
}

static void afont_gl_render_char( afont *a, char c )
{
  int ch;

  if(a->type != AFONT_GL_1BIT && a->type != AFONT_GL_8BIT)
    return;

  ch = c - a->base;
  if(ch < 0 || ch >= a->nchars) return;

  switch(a->type) {
    case AFONT_GL_8BIT:
      /* Move to the correct raster position; the bitmap will be positioned
       * with the lower left corner at that position, so the raster position
       * should be pen_x + bitmap_left, pen_y - bitmap_top + bmpy */
      glBitmap(0, 0, 0, 0, a->glyph_info[ch].bitmap_left, 
              a->glyph_info[ch].bitmap_top - a->glyph_info[ch].bmpy,
              NULL);

      /* Draw the pixels */
      glDrawPixels(
              a->glyph_info[ch].bmpx, a->glyph_info[ch].bmpy,
              GL_ALPHA, GL_UNSIGNED_BYTE,
              a->glyph_info[ch].bmp);

      /* Advance the pen for another character */
      glBitmap(0, 0, 0, 0, 
              a->glyph_info[ch].advance - a->glyph_info[ch].bitmap_left, 
              a->glyph_info[ch].bmpy - a->glyph_info[ch].bitmap_top,
              NULL);

      break;

    case AFONT_GL_1BIT:
      glBitmap(a->glyph_info[ch].bmpx, a->glyph_info[ch].bmpy,
              -a->glyph_info[ch].bitmap_left,
              a->glyph_info[ch].bitmap_top - a->glyph_info[ch].bmpy,
              a->glyph_info[ch].advance, 0,
              (const GLubyte *)a->glyph_info[ch].bmp);
      break;

    default:
      break;
  }

  return;
}

void afont_gl_render_text( afontgl *ag, const char *text )
{
  int i;
  float color[4];
  float biaszero;
  char *buf;

  /* Set the scale and bias values appropriately */
  if(ag->type == AFONT_GL_8BIT) {
    glGetFloatv(GL_CURRENT_COLOR, color);
    glPushAttrib(GL_PIXEL_MODE_BIT);

    biaszero = 0.0;

    glPixelTransferf(GL_RED_SCALE, biaszero);
    glPixelTransferf(GL_GREEN_SCALE, biaszero);
    glPixelTransferf(GL_BLUE_SCALE, biaszero);
    glPixelTransferf(GL_ALPHA_SCALE, color[3]);
    glPixelTransferf(GL_RED_BIAS, color[0]);
    glPixelTransferf(GL_GREEN_BIAS, color[1]);
    glPixelTransferf(GL_BLUE_BIAS, color[2]);
    glPixelTransferf(GL_ALPHA_BIAS, biaszero);
  }

  buf = strdup(text);
  for(i = 0; text[i]; i++) {
    buf[i] -= ag->base;
  }
  glListBase(ag->baselist);
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, buf);
  free(buf);

  /* Reset the scale and bias values */
  if(ag->type == AFONT_GL_8BIT)
    glPopAttrib();
  
  return;
}
