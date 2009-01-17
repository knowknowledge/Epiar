/* afont_gl.c */

/* See the file LICENSE for copyright and license information */

#include "afont_gl.h"
#include "includes.h"

void afont_gl_make_bitmaps( afont *a )
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

void afont_gl_render_char( afont *a, char c )
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
	      GL_LUMINANCE, GL_UNSIGNED_BYTE,
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

  case AFONT_1BIT:
  case AFONT_8BIT:
	  break;
  }

  return;
}

void afont_gl_render_text( afont *a, char *text )
{
  int i;
  float color[4];
  float biaszero;
  float scale[4];
  float bias[4];

  if(a->type == AFONT_GL_1BIT) {
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  } else if(a->type == AFONT_GL_8BIT) {
    /* This _can't_ be the best way to do this... */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGetFloatv(GL_CURRENT_COLOR, color);
    glGetFloatv(GL_RED_SCALE, &scale[0]);
    glGetFloatv(GL_GREEN_SCALE, &scale[1]);
    glGetFloatv(GL_BLUE_SCALE, &scale[2]);
    glGetFloatv(GL_ALPHA_SCALE, &scale[3]);
    glGetFloatv(GL_RED_BIAS, &bias[0]);
    glGetFloatv(GL_GREEN_BIAS, &bias[1]);
    glGetFloatv(GL_BLUE_BIAS, &bias[2]);
    glGetFloatv(GL_ALPHA_BIAS, &bias[3]);
    biaszero = 0.0;

    glPixelTransferf(GL_RED_SCALE, color[0]);
    glPixelTransferf(GL_GREEN_SCALE, color[1]);
    glPixelTransferf(GL_BLUE_SCALE, color[2]);
    glPixelTransferf(GL_ALPHA_SCALE, color[3]);
    glPixelTransferf(GL_RED_BIAS, biaszero);
    glPixelTransferf(GL_GREEN_BIAS, biaszero);
    glPixelTransferf(GL_BLUE_BIAS, biaszero);
    glPixelTransferf(GL_ALPHA_BIAS, biaszero);
  }

  for(i = 0; text[i]; i++)
    afont_gl_render_char(a, text[i]);

  if(a->type == AFONT_GL_8BIT) {
    glPixelTransferf(GL_RED_SCALE, scale[0]);
    glPixelTransferf(GL_GREEN_SCALE, scale[1]);
    glPixelTransferf(GL_BLUE_SCALE, scale[2]);
    glPixelTransferf(GL_ALPHA_SCALE, scale[3]);
    glPixelTransferf(GL_RED_BIAS, bias[0]);
    glPixelTransferf(GL_GREEN_BIAS, bias[1]);
    glPixelTransferf(GL_BLUE_BIAS, bias[2]);
    glPixelTransferf(GL_ALPHA_BIAS, bias[3]);
  }
  
  return;
}
