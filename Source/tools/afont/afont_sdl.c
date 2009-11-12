/* afont_sdl.c */

/* See the file LICENSE for copyright and license information */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "afont_sdl.h"

#ifndef ALLOCA_ALLOC
#define ALLOCA_ALLOC(n) alloca(n)
#endif

#ifndef ALLOCA_FREE
#define ALLOCA_FREE(n)
#endif

static Uint32 blend_alpha( SDL_PixelFormat *fmt, Uint32 src, Uint32 dst,
        Uint8 alpha );

/* TODO: Handle alpha values in fg, bg
 * (Maybe done -- do some checks to make sure that it works correctly
 *  in all cases.) */
void afont_render_char( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode, Uint32 *palette,
        SDL_Surface *s, int x, int y,
        char c )
{
  int ch;
  int sx, sy;
  int xtmp, ytmp;
  int ymax;

  Uint8 *linep, *endp;
  Uint8 *pp;

  Uint32 color;
  Uint32 pixel;

  Uint32 bgpixel;

  Uint8 *bmpp;
  Uint8 mask;

  Uint8 r, g, b, sa;

  if(a->type != AFONT_8BIT && a->type != AFONT_1BIT)
    return;

  r = (fg >> 24) & 0xff;
  g = (fg >> 16) & 0xff;
  b = (fg >> 8) & 0xff;
  sa = fg & 0xff;

  pixel = color = SDL_MapRGBA(s->format, r, g, b, sa);
  bgpixel = SDL_MapRGBA(s->format, (bg >> 24) & 0xff, 
          (bg >> 16) & 0xff, (bg >> 8) & 0xff, bg & 0xff);

  ch = c - a->base;
  if(ch < 0 || ch >= a->nchars) return;

  sx = x + a->glyph_info[ch].bitmap_left;
  sy = y - a->glyph_info[ch].bitmap_top;

  SDL_LockSurface(s);
  
  ymax = sy + a->glyph_info[ch].bmpy;
  linep = (Uint8 *)s->pixels + sx * s->format->BytesPerPixel +
          sy * s->pitch;
  bmpp = a->glyph_info[ch].bmp;
  for(ytmp = sy ; ytmp < ymax; ytmp++, linep += s->pitch) {
    if(ytmp < 0 || ytmp >= s->h) {
      if(a->type == AFONT_8BIT)
        bmpp += a->glyph_info[ch].bmpx;
      else if(a->type == AFONT_1BIT)
        bmpp += (a->glyph_info[ch].bmpx + 7) / 8;
      
      continue;
    }
    
    pp = linep;
    endp = pp + a->glyph_info[ch].bmpx * s->format->BytesPerPixel;
    mask = 1;
    for(xtmp = sx ; pp < endp;
            pp += s->format->BytesPerPixel, xtmp++, bmpp++) {
      if(xtmp < 0 || xtmp >= s->w)
        continue;

      if(a->type == AFONT_1BIT) {
        if(!(*bmpp & mask))
          continue;

        pixel = color;
      } else if (a->type == AFONT_8BIT) {
        if(!*bmpp)   /* Skip transparent chars */
          continue;

        if (mode == AFONT_RENDER_SOLID) {
          if(*bmpp < AFONT_SOLID_CUTOFF)   /* Skip transparent chars */
            continue;
          
          /* pixel should already == color */
        } else if (mode == AFONT_RENDER_SHADED) {
          if(palette)
            pixel = palette[*bmpp];
          else
            pixel = blend_alpha(s->format, color, bgpixel, *bmpp);
        } else if (mode == AFONT_RENDER_BLENDED) {
          Uint32 dst;
        
          switch(s->format->BytesPerPixel) {
            case 3:
              fprintf(stderr, "Warning: %d bpp depth is unhandled\n", 
                      s->format->BytesPerPixel * 8);
              dst = 0;
              break;
            case 1:
              dst = *(Uint8 *)pp;
              break;
            case 2:
              dst = *(Uint16 *)pp;
              break;
            case 4:
              dst = *(Uint32 *)pp;
              break;
          }
          
          if(sa == 0xff)
            pixel = blend_alpha(s->format, color, dst, *bmpp);
          else
            pixel = blend_alpha(s->format, color, dst,
                    (int)*bmpp * (int)sa >> 8);
        } else if (mode == AFONT_RENDER_ALPHA) {
          if(palette)
            pixel = palette[*bmpp];
          else
            pixel = SDL_MapRGBA(s->format, r, g, b, *bmpp);
        }
      }
      
      switch(s->format->BytesPerPixel) {
        case 3:
          fprintf(stderr, "Warning: %d bpp depth is unhandled\n", 
                  s->format->BytesPerPixel * 8);
          return;
        case 1:
          *(Uint8 *)pp = pixel;
          break;
        case 2:
          *(Uint16 *)pp = pixel;
          break;
        case 4:
          *(Uint32 *)pp = pixel;
          break;
      }

#if 0
      if(a->type == AFONT_1BIT) {
        mask <<= 1;
        if(!mask) {
          mask = 1;
          bmpp++;
        }
      } else {
        bmpp++;
      }
#endif
    }

    if(a->type == AFONT_1BIT && mask != 1) bmpp++;
  }

  SDL_UnlockSurface(s);

  return;
}

void afont_render_text( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_Surface *s, int x, int y,
        char *text )
{
  int i;
  int pen_x;

  Uint32 *palette = NULL;

  if(mode == AFONT_RENDER_SHADED || mode == AFONT_RENDER_ALPHA) {
    /* Build a palette, so we don't have to constantly be translating color
     * values. */
    int i;
    Uint32 color, bgpixel;
    Uint8 r, g, b;

    palette = ALLOCA_ALLOC(256 * sizeof(Uint32));

    r = (fg >> 24) & 0xff;
    g = (fg >> 16) & 0xff;
    b = (fg >> 8) & 0xff;
    color = SDL_MapRGBA(s->format, r, g, b, 0xff);
    bgpixel = SDL_MapRGBA(s->format, (bg >> 24) & 0xff, 
            (bg >> 16) & 0xff, (bg >> 8) & 0xff, bg & 0xff);
    
    for(i = 0; i < 256; i++) {
      if(mode == AFONT_RENDER_SHADED) {
        palette[i] = blend_alpha(s->format, color, bgpixel, i);
      } else { /* AFONT_RENDER_ALPHA */
        palette[i] = SDL_MapRGBA(s->format, r, g, b, i);
      }
    }

    ALLOCA_FREE(palette);
  }

  /* Render each character */
  for(i = 0, pen_x = x; text[i]; i++) {
    afont_render_char(a, fg, bg, mode, palette, s, pen_x, y, text[i]);
    pen_x += a->glyph_info[text[i] - a->base].advance;
  }

  return;
}

void afont_render_text_bg( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_Surface *s, int x, int y,
        char *text )
{
  int w, h, base;
  SDL_Rect dst;
  Uint32 pixel;
  
  /* Note: For this function, AFONT_RENDER_BLENDED is identical to
   * AFONT_RENDER_SHADED.  Switch to shaded mode; since 
   * we know that the surface is uniformly colored, it will be quicker. */
   
  if(mode == AFONT_RENDER_BLENDED) {
    mode = AFONT_RENDER_SHADED;
  }

  /* Find out the size and location of the bounding box */
  afont_size_text(a, text, &w, &h, &base);
  dst.x = x;
  dst.y = y - base;
  dst.w = w;
  dst.h = h;

  /* Fill the destination with the background color */
  pixel = SDL_MapRGB(s->format,
          (bg >> 24) & 0xff,
          (bg >> 16) & 0xff,
          (bg >> 8) & 0xff);
  SDL_FillRect(s, &dst, pixel);

  /* Draw the text */
  afont_render_text(a, fg, bg, mode, s, x, y, text);

  return;
}

SDL_Surface *afont_render_text_surf( afont *a, Uint32 fg, Uint32 bg,
        afont_render_mode mode,
        SDL_PixelFormat *fmt,
        char *text )
{
  int w, h, base;
  Uint32 pixel;
  SDL_Surface *s;

  /* Note: For this function, AFONT_RENDER_BLENDED is identical to
   * AFONT_RENDER_SHADED with bg == black.  Switch to shaded mode; since 
   * we know that the surface is empty, it will be quicker. */
  if(mode == AFONT_RENDER_BLENDED) {
    bg = 0x000000ff;
    mode = AFONT_RENDER_SHADED;
  }
  
  /* Find out the size and location of the bounding box */
  afont_size_text(a, text, &w, &h, &base);
 
  /* Create a new SDL_Surface for the text */
  s = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, fmt->BitsPerPixel,
          fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  /* Fill the surface with the background color */
  if(bg & 0xffffff00) {
    pixel = SDL_MapRGB(s->format,
            (bg >> 24) & 0xff,
            (bg >> 16) & 0xff,
            (bg >> 8) & 0xff);
    SDL_FillRect(s, NULL, pixel);
  }

  /* Render the text onto the surface */
  afont_render_text(a, fg, bg, mode, s, 0, base, text);

  /* Make sure colorkeying / alpha surfaces are set up properly */
  switch(mode) {
    case AFONT_RENDER_SOLID:
    case AFONT_RENDER_SHADED:
    case AFONT_RENDER_BLENDED:
      SDL_SetColorKey(s, SDL_SRCCOLORKEY, SDL_MapRGB(s->format, 0, 0, 0));
      break;

    case AFONT_RENDER_ALPHA:
      SDL_SetAlpha(s, SDL_SRCALPHA, 255);
      break;
  }

  return s;
}

/* Utility functions */

/* The blend_alpha algorithm was modified from the _putPixelAlpha 
 * function of SDL_gfxPrimitives, which is (c) A. Schiffler */
static Uint32 blend_alpha( SDL_PixelFormat *fmt, Uint32 src, Uint32 dst,
        Uint8 alpha )
{
  Uint32 
    Rmask = fmt->Rmask, 
    Gmask = fmt->Gmask,
    Bmask = fmt->Bmask,
    Amask = fmt->Amask;

  if(alpha == 255)
    return src;
  if(alpha == 0)
    return dst;
  
  switch (fmt->BytesPerPixel) {
    case 1: {    /* Assuming 8-bpp */
      Uint8 dR, dG, dB, sR, sG, sB;
      dR = fmt->palette->colors[dst].r;
      dG = fmt->palette->colors[dst].g;
      dB = fmt->palette->colors[dst].b;
      sR = fmt->palette->colors[src].r;
      sG = fmt->palette->colors[src].g;
      sB = fmt->palette->colors[src].b;

      dR = dR + ((sR - dR) * alpha >> 8);
      dG = dG + ((sG - dG) * alpha >> 8);
      dB = dB + ((sB - dB) * alpha >> 8);

      return SDL_MapRGB(fmt, dR, dG, dB);
    }

    case 2:    /* Probably 15-bpp or 16-bpp */
    case 4: {  /* Probably 32-bpp */
      Uint32 R, G, B, A;
      R = ((dst & Rmask) + (((src & Rmask) - (dst & Rmask)) * alpha >> 8))
              & Rmask;
      G = ((dst & Gmask) + (((src & Gmask) - (dst & Gmask)) * alpha >> 8))
              & Gmask;
      B = ((dst & Bmask) + (((src & Bmask) - (dst & Bmask)) * alpha >> 8))
              & Bmask;
      if (Amask)
        A = ((dst & Amask) + (((src & Amask) - (dst & Amask)) * alpha >> 8))
                & Amask;
      else
        A = 0;

      return R | G | B | A;
    }

    default:
      return src;
  }

  return src;
}
