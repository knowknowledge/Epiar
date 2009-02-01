/*
 * Filename      : image.h
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 31, 2009
 * Purpose       : Image loading and display
 * Notes         : You don't have to worry about OpenGL's power of 2 image dimension requirements.
 *                 This class will scale for you while still displaying correctly.
 *                 When editing this class, there are a number of curious variables and conventions to
 *                 pay attention to. The real width/height of the image (rw, rh) is rarely used - it is
 *                 the true width and height of the image according to OpenGL. However, as we often expand
 *                 images whose dimensions weren't a power of two, but must be for proper OpenGL texture
 *                 size requirements, we may internally expand the dimensions of the image (thus the rw,rh grow)
 *                 but the w/h is still effectively (at least as far as we care on the outside) whatever non-
 *                 power of two dimensions, and these effective, "fake" dimensions are called it's _virtual_
 *                 dimensions, or virtual width/height, stored in w, h.
 */

#ifndef __H_IMAGE__
#define __H_IMAGE__

#include "includes.h"

class Image {
	public:
		Image();
		// Create instance by loading image from file
		Image( string filename );

		// Load image from file
		bool Load( string filename );
		// Load image from buffer
		bool Load( unsigned char *buf, int bufSize );
		// Load image from FILE *
		bool Load( FILE *fp, int size );

		// Get information about image dimensions (always the virtual/effective size)
		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		int GetHalfWidth( void ) { return w / 2; };
		int GetHalfHeight( void ) { return h / 2; };

		// Draw the image (angle in degrees)
		void Draw( int x, int y, float angle = 0. );
		// Draw the image centered on (x,y) (angle in degrees)
		void DrawCentered( int x, int y, float angle = 0. );
		// Draw the image tiled to fill a rectangle of w/h - will crop to meet w/h and won't overflow
		void DrawTiled( int x, int y, int w, int h );

	private:
		// Converts an SDL surface to an OpenGL texture
		bool ConvertToTexture( SDL_Surface *s );
		// Expands surface 's' to width/height of w/h, keeping the original image in the upper-left
		SDL_Surface *ExpandCanvas( SDL_Surface *s, int w, int h );
		// Returns the next highest power of two if num is not a power of two
		int PowerOfTwo(int num);

		int w, h; // virtual w/h (effective, same as original file)
		int rw, rh; // real w/h, size of expanded canvas (image) should expansion be needed
		            //   to meet power of two requirements
		float scale_w, scale_h; // used by draw() on images with expanded canvases to know what percent of
		                        // the larger canvas actually contains the original image (<= 1.0)
		                        // defaults = 1.0, this factor is always used, so non-expanded images are
		                        // simply "scaled" at 1.0
		GLuint image; // OpenGL pointer to texture
};

#endif // __H_IMAGE__

