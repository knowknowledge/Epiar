/* afont.h */

/* See the file LICENSE for copyright and license information */

#ifndef _afont_h
#define _afont_h

#include <stdio.h>

#define AFONT_HEADER "AFNT0004"
#define AFONT_HEADER_SIZE 8

#define AFONT_SOLID_CUTOFF 128
  /* Solid rendering, for now, is simply done by rendering all pixels with
   * alpha >= AFONT_SOLID_CUTOFF as full intensity */

typedef enum {
  AFONT_BITMAP_1,          /* 1 bit per pixel.  Bitmap pitch is a multiple
                            * of 8 bits */
  AFONT_BITMAP_8,          /* 8 bits per pixel */
  AFONT_BITMAP_24          /* 24 bits per pixel; three sets of identical
                            * 8-bit values (creating a grayscale RGB 
                            * image */
} afont_bitmap_type;

typedef enum {
  AFONT_1BIT,              /* 1 bit per pixel, standard ordering.  Padded to
                            * one byte. */
  AFONT_8BIT,              /* 8 bits per pixel, standard ordering, no
                            * padding */
  AFONT_GL_1BIT,           /* 1 bit per pixel, reversed rows.  Padded to one
                            * byte. */
  AFONT_GL_8BIT,           /* 8 bits per pixel, reversed rows.  Padded to
                            * four bytes. */
} afont_type;

typedef struct afont afont;

struct _afont_glyph {
  int bmpx, bmpy;                /* Size of glyph bitmap */
  int bitmap_left, bitmap_top;   /* the location of the bitmap, relative to
                                  * the current pen position */
  int advance;                   /* Number of pixels to advance after
                                  * drawing this character */
  char *bmp;                     /* Pointer to the bitmap (This is a pointer
                                  * into the afont->glyphimg array) */
};

struct afont {
  int nchars;  /* Number of characters in font */
  int base;    /* ASCII character of base of font (in general, 32) */

  struct _afont_glyph *glyph_info;
  char *glyphimg;  /* Glyph bitmap buffer */

  int bbox_x, bbox_y;  /* Smallest box that will enclose any glyph bitmap */
  afont_type type;
};

afont *afont_load( char *path );
  /* Load a font from a file */
afont *afont_load_fp( FILE *fp );
  /* Load a font, given a stream that points to the beginning of a afont
   * file. */
void afont_free( afont *a );
  /* Free an afont structure and all associated data. */

void afont_size_text( afont *a, char *text, int *w, int *h, int *base );
  /* Find the size of a string of text.  *w and *h will be set to the width
   * and height of a bounding box that will accomodate the rendered text,
   * and base is the location of the baseline, relative to the top of the
   * bounding box. */

char *afont_build_bitmap( afont *a, afont_bitmap_type type,
        int *x, int *y );
  /* Create a large bitmap with all of the characters evenly spaced on it.
   * The bitmap size is returned in x and y. 
   * This is unused and more than likely does not work. */
  
void afont_dump_char( afont *a, char c );
  /* Dump an ASCII representation of the given character to stdout.
   * Primarily for debugging purposes. */

#endif
