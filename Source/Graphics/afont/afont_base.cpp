/* afont_base.c */

/* See the file LICENSE for copyright and license information */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afont.h"

#ifndef SYS_IS_BRAINDEAD
#include <netinet/in.h>
#else
#define ntohl(n) afont_ntohl(n)

static int afont_ntohl( int n )
{
  int buf = 1;
  if(*(char *)(&buf) == 0x01) {
    /* Little-endian */
    ((char *)(&buf))[0] = ((char *)(&n))[3];
    ((char *)(&buf))[1] = ((char *)(&n))[2];
    ((char *)(&buf))[2] = ((char *)(&n))[1];
    ((char *)(&buf))[3] = ((char *)(&n))[0];
    return buf;
  } else {
    /* Big-endian */
    return n;
  }
}

#endif


afont *afont_load( const char *path )
{
  afont *a;
  FILE *fp;

  fp = fopen(path, "rb");
  a = afont_load_fp(fp);
  fclose(fp);

  return a;
}

afont *afont_load_fp( FILE *fp )
{
  afont *a;

  unsigned int tmpi;
  unsigned char tmps[AFONT_HEADER_SIZE];
  int i;

  int total_size;
  char *gip;

  fread(tmps, AFONT_HEADER_SIZE, 1, fp);
  if(strncmp((const char *)tmps, AFONT_HEADER, AFONT_HEADER_SIZE))
    return NULL;

  a = (afont *)malloc(sizeof(afont));
  
  fread(&tmpi, 4, 1, fp);
  tmpi = (int)ntohl(tmpi);
  switch(tmpi) {
    case 0x00:
      a->type = AFONT_1BIT;
      break;
    case 0x01:
      a->type = AFONT_8BIT;
      break;
    default:
      free(a);
      return NULL;
  }
      
  fread(&tmpi, 4, 1, fp);
  a->nchars = (int)ntohl(tmpi);

  fread(&tmpi, 4, 1, fp);
  a->base = (int)ntohl(tmpi);

  a->glyph_info = (_afont_glyph *)malloc(sizeof(struct _afont_glyph) * a->nchars);

  a->bbox_x = 0;
  a->bbox_y = 0;

  total_size = 0;
  for(i = 0; i < a->nchars; i++) {
    fread(&tmpi, 4, 1, fp);
    a->glyph_info[i].bmpx = (int)ntohl(tmpi);
    
    if(a->glyph_info[i].bmpx > a->bbox_x)
      a->bbox_x = a->glyph_info[i].bmpx;

    fread(&tmpi, 4, 1, fp);
    a->glyph_info[i].bmpy = (int)ntohl(tmpi);

    if(a->glyph_info[i].bmpy > a->bbox_y)
      a->bbox_y = a->glyph_info[i].bmpy;

    fread(&tmpi, 4, 1, fp);
    a->glyph_info[i].bitmap_left = (int)ntohl(tmpi);

    fread(&tmpi, 4, 1, fp);
    a->glyph_info[i].bitmap_top = (int)ntohl(tmpi);

    fread(&tmpi, 4, 1, fp);
    a->glyph_info[i].advance = (int)ntohl(tmpi);

    if(a->type == AFONT_1BIT)
      total_size += ((a->glyph_info[i].bmpx + 7) / 8) * 
              a->glyph_info[i].bmpy;
    else if(a->type == AFONT_8BIT)
      total_size += a->glyph_info[i].bmpx * a->glyph_info[i].bmpy;
  }

  a->glyphimg = (char *)malloc(total_size);

  /* Read all the glyph images */
  fread(a->glyphimg, total_size, 1, fp);

  for(i = 0, gip = a->glyphimg; i < a->nchars; i++) {
    a->glyph_info[i].bmp = gip;
    if(a->type == AFONT_1BIT)
      gip += ((a->glyph_info[i].bmpx + 7) / 8) * a->glyph_info[i].bmpy;
    else if(a->type == AFONT_8BIT)
      gip += a->glyph_info[i].bmpx * a->glyph_info[i].bmpy;
  }

  return a;
}

void afont_free( afont *a )
{
  free(a->glyph_info);
  free(a->glyphimg);

  return;
}

void afont_size_text( afont *a, const char *text, int *w, int *h, int *base )
{
  int maxx, maxy, miny;
  int pen_x;
  
  int i;
  int ch;

  pen_x = 0;
  maxx = 0;
  maxy = 0;
  miny = 0;

  for(i = 0; text[i]; i++) {
    ch = text[i] - a->base;
    //aotf_assert(ERR_CHECK, ch >= 0 && ch < a->nchars,
    //        "Illegal character [%c] in string", text[i]);
    if(ch < 0 || ch >= a->nchars) ch = 0;

    if(pen_x + a->glyph_info[ch].bitmap_left + a->glyph_info[ch].bmpx >
            maxx)
      maxx = pen_x + a->glyph_info[ch].bitmap_left + a->glyph_info[ch].bmpx;

    if(a->glyph_info[ch].bitmap_top > maxy || i == 0)
      maxy = a->glyph_info[ch].bitmap_top;
    
    if(a->glyph_info[ch].bitmap_top - a->glyph_info[ch].bmpy < miny || 
            i == 0)
      miny = a->glyph_info[ch].bitmap_top - a->glyph_info[ch].bmpy;

    pen_x += a->glyph_info[ch].advance;
  }

  if(w) *w = maxx + 1;
  if(h) *h = maxy - miny + 1;
  if(base) *base = maxy;

  return;
}

char *afont_build_bitmap( afont *a, afont_bitmap_type type,
        int *x, int *y )
{
  char *buf;
  int bmpx, bmpy, bmpsize;
  int i;
  int pitch = 0;

  /* Determine bitmap size */
  bmpx = a->bbox_x * 12;
  bmpy = a->bbox_y * ((a->nchars + 11) / 12);
  bmpsize = bmpx * bmpy;

  switch(type) {
    case AFONT_BITMAP_1:
      pitch = (bmpx + 7) / 8;
      break;

    case AFONT_BITMAP_8:
      pitch = bmpx;
      break;

    case AFONT_BITMAP_24:
      pitch = bmpx * 3;
      break;
  }

  buf = (char *)calloc(pitch * bmpy, 1);

  for(i = 0; i < a->nchars; i++) {
    int x, y;

    char *glyphp;
    int gx, gy;

    char *bmpp, *bmpp0 = 0;
    unsigned char mask = 0, mask0 = 0;
    
    x = i % 12;
    y = i / 12;

    /* Set up bmpp0 and mask0 */
    switch(type) {
      case AFONT_BITMAP_1:
        bmpp0 = buf + y * a->bbox_y * pitch + (x * a->bbox_x + 7) / 8;
        mask0 = 1 << ((x * a->bbox_x) % 8);
        break;
      case AFONT_BITMAP_8:
        bmpp0 = buf + y * a->bbox_y * pitch + x * a->bbox_x;
        break;
      case AFONT_BITMAP_24:
        bmpp0 = buf + y * a->bbox_y * pitch + x * a->bbox_x * 3;
        break;
    }

    glyphp = a->glyph_info[i].bmp;
    for(gy = 0; gy < a->glyph_info[i].bmpy; gy++) {
      bmpp = bmpp0 + gy * pitch;
      mask = mask0;

      for(gx = 0; gx < a->glyph_info[i].bmpx; gx++, glyphp++) {
      
        switch(type) {
          case AFONT_BITMAP_1:
            if(*(unsigned char *)glyphp > AFONT_SOLID_CUTOFF)
              *bmpp |= mask;

            mask <<= 1;
            if(!mask) {
              mask = 1;
              bmpp++;
            }
            
            break;
          case AFONT_BITMAP_8:
            *bmpp = *glyphp;
            
            bmpp++;
            
            break;
          case AFONT_BITMAP_24:
            bmpp[0] = *glyphp;
            bmpp[1] = *glyphp;
            bmpp[2] = *glyphp;
            
            bmpp += 3;
            
            break;
        }

      }
    }
    
  }

  if(x) *x = bmpx;
  if(y) *y = bmpy;
  return buf;
}

void afont_dump_char( afont *a, char c )
{
  int ch;
  int y;
  int i;

  char *buf;
  
  ch = c - a->base;
  if(ch < 0 || ch >= a->nchars) return;

  buf = (char *)malloc(a->glyph_info[ch].bmpx + 1);

  switch(a->type) {
    default:
      printf("Unhandled font bitmap type: %d\n", a->type);
      break;

    case AFONT_1BIT:
    case AFONT_GL_1BIT:
      for(y = 0; y < a->glyph_info[ch].bmpy; y++) {
        unsigned char *bp;
        unsigned char mask;
        for(i = 0, bp = (unsigned char *)(a->glyph_info[ch].bmp + 
                y * ((a->glyph_info[ch].bmpx + 7) / 8)), mask = 1; 
                i < a->glyph_info[ch].bmpx; i++) {
          if(*bp & mask) buf[i] = '*'; else buf[i] = ' ';
          mask <<= 1;
          if(!mask) {
            mask = 1;
            bp++;
          }
        }
        buf[i] = '\0';

        printf("%s\n",buf);
      }
      break;

    case AFONT_8BIT:
      for(y = 0; y < a->glyph_info[ch].bmpy; y++) {
        for(i = 0; i < a->glyph_info[ch].bmpx; i++) {
          if((unsigned char)a->glyph_info[ch].bmp[y * a->glyph_info[ch].bmpx + i]
                  > AFONT_SOLID_CUTOFF)
            buf[i] = '*';
          else
            buf[i] = ' ';
        }

        buf[i] = '\0';

        printf("%s\n",buf);
      }
      break;
  }

  free(buf);

  return;
}
