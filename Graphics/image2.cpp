/*
 * Filename      : image2.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 31, 2009
 * Purpose       : Image loading and display
 * Notes         : See this note section in image2.h for an important clarification about the handling
 *                 of non-power of two image sizes and the difference between virtual/effective dimensions
 *                 and real dimensions.
 */

#include "Graphics/image2.h"

// Create instance by loading image from file
Image2::Image2( string filename ) {
	// Initialize variables
	w = h = rw = rh = image = 0;

	Load(filename);
}

// Load image from file
bool Image2::Load( string filename ) {

}

// Load image from buffer
bool Image2::Load( unsigned char *buf, int bufSize ) {

}

// Draw the image
void Image2::Draw( int x, int y, float angle ) {

}

// Draw the image centered on (x,y)
void Image2::DrawCentered( int x, int y, float angle ) {

}

