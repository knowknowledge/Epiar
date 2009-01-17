/* buildfont.c */

/**********************************************************
* Generated font format:
*
* (All integer values are big-endian)
*
* Bytes   Type    Value
* 8       char    "AFNT0004"
*                 Header will change with each version number of the afont
*                 format.  AFNT0003 corresponds to the format introduced in
*                 afont v0.3.
* 4       uint    font type:
*                 0: 1-bit bitmap; no alpha
1: 8-bit bitmap; alpha data
* 4       uint    number of characters
* 4       uint    ASCII base character
* 
*    For each character:
* 4       uint    bitmap width
* 4       uint    bitmap height
* 4       int     bitmap left position
* 4       int     bitmap top position
* 4       int     horizontal pen advance
*
* varies  char[]  image data
*    For 1-bit bitmap:
*         0x01 is leftmost bit, stored MSB first.
*         rows are padded to 1-byte boundaries
*    For 8-bit bitmap:
*         char 0xff == opaque,  char 0x00 = transparent
*
*********************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ft2build.h>
#include <freetype/freetype.h>

#ifndef WIN32
#include <netinet/in.h>
#else
#define htonl(n) afont_htonl(n)
#pragma comment( lib, "libfreetype.lib" )
#endif

//#define aotf_assert(level, cond, ...) assert(cond)

struct glyphinfo {
	int bmpx, bmpy;                /* Actual bitmap size */
	int bitmap_left, bitmap_top;   /* Offsets of the bitmap location */
	int advance;                   /* The horizontal pen advance */
};

struct fontinfo {
	int nchars;              /* Total number of characters */
	int base;
	struct glyphinfo *info;
	int imgbuf_size;
	char *imgbuf;
	int bits;
};

static int afont_htonl( int h );

/* All integers are written big-endian */
void write_fontdata( FILE *fp, struct fontinfo *fi )
{
	int i;
	
	unsigned int tmpi;
		
	const char *header = "AFNT0004";
	
	fwrite(header, strlen(header), 1, fp);
	
	if(fi->bits == 1)
		tmpi = 0x00;  /* 1-bit bitmap */
	else
		tmpi = 0x01;  /* 8-bit bitmap */
    
	tmpi = htonl(tmpi);
	fwrite(&tmpi, sizeof(unsigned int), 1, fp);
	
	tmpi = htonl(fi->nchars);
	fwrite(&tmpi, sizeof(unsigned int), 1, fp);
	
	tmpi = htonl(fi->base);
	fwrite(&tmpi, sizeof(unsigned int), 1, fp);
	
	for(i = 0; i < fi->nchars; i++) {
		tmpi = htonl(fi->info[i].bmpx);
		fwrite(&tmpi, sizeof(unsigned int), 1, fp);
		
		tmpi = htonl(fi->info[i].bmpy);
		fwrite(&tmpi, sizeof(unsigned int), 1, fp);
		
		tmpi = htonl(fi->info[i].bitmap_left);
		fwrite(&tmpi, sizeof(unsigned int), 1, fp);
		
		tmpi = htonl(fi->info[i].bitmap_top);
		fwrite(&tmpi, sizeof(unsigned int), 1, fp);
		
		tmpi = htonl(fi->info[i].advance);
		fwrite(&tmpi, sizeof(unsigned int), 1, fp);
	}
	
	fwrite(fi->imgbuf, fi->imgbuf_size, 1, fp);
	
	return;
}

int main( int argc, char **argv )
{
	FT_Library ftlib;
	FT_Face ftface;
	int error;
	
	char *fontpath;
	int fontsize;
	
	struct fontinfo fi;
	
	int base = 32;           /* The base index of the character list */
	char *bufp;
	
	int i;
	FILE *fp;
	
	if(argc != 4) {
		fprintf(stderr, "Usage: %s [fontname] [fontsize] [bits]\n", argv[0]);
		return 0;
	}
	
	fontpath = argv[1];
	fontsize = atoi(argv[2]);
	
	fi.nchars = 96;
	fi.base = 32;
	fi.bits = atoi(argv[3]);
	if(fi.bits != 1 && fi.bits != 8) fi.bits = 8;
	
	error = FT_Init_FreeType(&ftlib);
	//aotf_assert(ERR_FATAL, !error,
	//	"Couldn't initialize freetype, error %d", error);
	assert( !error );
	
	error = FT_New_Face(ftlib, fontpath, 0, &ftface);
	//aotf_assert(ERR_FATAL, !error,
	//	"Couldn't load font [%s], error %d", fontpath, error);
	assert( !error );
	
	error = FT_Set_Pixel_Sizes(ftface, 0, fontsize);
	//aotf_assert(ERR_FATAL, !error,
	//	"Couldn't set font size to %d, error %d", fontsize, error);
	assert( !error );
	
	for(i = 0, fi.imgbuf_size = 0; i < fi.nchars; i++) {
		int glyphidx;
		
		glyphidx = FT_Get_Char_Index(ftface, i + base);
		
		error = FT_Load_Glyph(ftface, glyphidx, FT_LOAD_RENDER);
		//aotf_assert(ERR_FATAL, !error,
        //	"Error loading glyph %d, error %d", i + base, glyphidx);

		assert( !error );
		
		if(fi.bits == 1) {
			fi.imgbuf_size += ((ftface->glyph->bitmap.width + 7) / 8) *
				ftface->glyph->bitmap.rows;
		} else if(fi.bits == 8) {
			fi.imgbuf_size += 
				ftface->glyph->bitmap.width * ftface->glyph->bitmap.rows;
		}
	}
	
	fi.info = (struct glyphinfo *)malloc(fi.nchars * sizeof(struct glyphinfo));
	fi.imgbuf = (char *)malloc(fi.imgbuf_size);  /* Zero image data */
	
	/* Loop through each character */
	for(i = 0, bufp = fi.imgbuf; i < fi.nchars; i++) {
		int glyphidx;
		
		glyphidx = FT_Get_Char_Index(ftface, i + base);
		
		error = FT_Load_Glyph(ftface, glyphidx, FT_LOAD_DEFAULT);
		//aotf_assert(ERR_FATAL, !error,
        //	"Error loading glyph %d, error %d", i + base, glyphidx);
		assert( !error );
		
		error = FT_Render_Glyph(ftface->glyph, (fi.bits == 1) ?
				ft_render_mode_mono : ft_render_mode_normal);
		
		/* Fill in the info structure for this character */
		fi.info[i].bmpx = ftface->glyph->bitmap.width;
		fi.info[i].bmpy = ftface->glyph->bitmap.rows;
		fi.info[i].bitmap_left = ftface->glyph->bitmap_left;
		fi.info[i].bitmap_top = ftface->glyph->bitmap_top;
		fi.info[i].advance = ftface->glyph->advance.x / 64;
		
		/* Copy the bitmap into the buffer */
		if(fi.bits == 1) {
			unsigned char *bp1, *bp2, *line2;
			int j, k;
			
			bp1 = (unsigned char *)bufp;  
			bp2 = ftface->glyph->bitmap.buffer;
			line2 = ftface->glyph->bitmap.buffer;
			for(j = 0; j < fi.info[i].bmpy; 
			j++, line2 += ftface->glyph->bitmap.pitch) {
				bp2 = line2;
				for(k = 0; k < ((fi.info[i].bmpx + 7) / 8); k++, bp1++, bp2++)
					*bp1 = *bp2;
			}
			
			bufp += ((fi.info[i].bmpx + 7) / 8) * fi.info[i].bmpy;
		} else if(fi.bits == 8) {
			memcpy(bufp, ftface->glyph->bitmap.buffer,
				fi.info[i].bmpx * fi.info[i].bmpy);
			bufp += fi.info[i].bmpx * fi.info[i].bmpy;
		}
	}
	
#ifdef WIN32 
	fp = fopen("out.af", "wb");
	write_fontdata(fp, &fi);
	fclose(fp);
#else
	/* Dump the info to stdout */
	write_fontdata(stdout, &fi);
#endif
	
	return 0;
}

static int afont_htonl( int h )
{
	int buf = 1;
	if(*(char *)(&buf) == 0x01) {
		/* Little-endian */
		((char *)(&buf))[0] = ((char *)(&h))[3];
		((char *)(&buf))[1] = ((char *)(&h))[2];
		((char *)(&buf))[2] = ((char *)(&h))[1];
		((char *)(&buf))[3] = ((char *)(&h))[0];
		return buf;
	} else {
		/* Big-endian */
		return h;
	}
}

