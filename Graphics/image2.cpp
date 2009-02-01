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
#include "Utilities/log.h"
#include "Utilities/trig.h"

// Create instance by loading image from file
Image2::Image2( string filename ) {
	// Initialize variables
	w = h = rw = rh = image = 0;
	scale_w = scale_h = 1.;

	Load(filename);
}

// Load image from file
bool Image2::Load( string filename ) {
	SDL_Surface *s = NULL;

	if( ( s = IMG_Load( filename.c_str() ) ) == NULL ) {
		Log::Warning( "Failed to load %s", filename.c_str() );
		return( false );
	}

	// virtual/effective w/h is whatever the original file intended (eg ignoring canvas expansion)
	w = s->w;
	h = s->h;

	if( ConvertToTexture( s ) == false ) {
		Log::Warning( "Failed to load %s", filename.c_str() );
		SDL_FreeSurface( s );
		return( false );
	}

	return( true );
}

// Load image from buffer
bool Image2::Load( unsigned char *buf, int bufSize ) {
	SDL_RWops *rw;
	SDL_Surface *s = NULL;

	rw = SDL_RWFromMem( buf, bufSize );
	if( !rw ) {
		Log::Warning( "Image loading failed. Could not create RWops" );
		return( false );
	}

	s = IMG_Load_RW( rw, 0 );
	SDL_FreeRW( rw );

	if( !s ) {
		Log::Warning( "Image loading failed. Could not load image from RWops" );
		return( false );
	}

	w = s->w;
	h = s->h;

	if( ConvertToTexture( s ) == false ) {
		Log::Warning( "Failed to load image from buffer" );
		SDL_FreeSurface( s );
		return( false );
	}

	return( true );
}

// Draw the image (angle is in degrees)
void Image2::Draw( int x, int y, float angle ) {
	// the four rotated (if needed) corners of the image
	float ulx, urx, llx, lrx, uly, ury, lly, lry;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear The Background Color To Black
	glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
	glEnable(GL_DEPTH_TEST); // Enable Depth Testing
	glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
	glEnable(GL_TEXTURE_2D); // Enable 2D Texture Mapping

	if( !image ) {
		Log::Warning( "Trying to draw without loading an image first." );
		return;
	}

	// bind the image and draw
 	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// calculate the coordinates of the quad	
	// avoid trig when you can
	if( angle != 0. ) {
		Trig *trig = Trig::Instance();
		float a = -(float)trig->DegToRad( angle );
		// ax/ay are the coordinate to rotate "about", hence "about points", "about x", "about y"
		float ax = (float)x + (w / 2.);
		float ay = (float)y + (h / 2.);

		trig->RotatePoint( (float)x, (float)y, ax, ay, (float *)&ulx, (float *)&uly, a );
		trig->RotatePoint( (float)x + w, (float)y + h, ax, ay, (float *)&urx, (float *)&ury, a );
		trig->RotatePoint( (float)x, (float)y, ax, ay, (float *)&llx, (float *)&lly, a );
		trig->RotatePoint( (float)x + w, (float)y, ax, ay, (float *)&lrx, (float *)&lry, a );
	} else {
		ulx = x;
		urx = x + w;
		llx = x;
		lrx = x + w;
		uly = y + h;
		ury = y + h;
		lly = y;
		lry = y;
	}

	// draw it
	glColor3f(1, 1, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, image );

	glPushMatrix();
	glBegin( GL_POLYGON );
	glTexCoord2f( 0., scale_h );
	glVertex2f( ulx, uly );
	glTexCoord2f( scale_w, scale_h );
	glVertex2f( urx, ury );
	glTexCoord2f( scale_w, 0. );
	glVertex2f( lrx, lry );
	glTexCoord2f( 0., 0. );
	glVertex2f( llx, lly );
	glEnd();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST); // Enable Depth Testing
	glDisable(GL_BLEND); // Disable Blending

	glDisable(GL_TEXTURE_2D); // Disable 2D Texture Mapping
	glBindTexture(GL_TEXTURE_2D,0); // Unbind The Blur Texture
}

// Draw the image centered on (x,y)
void Image2::DrawCentered( int x, int y, float angle ) {
	Draw( x - (w / 2), y - (h / 2), angle );
}

// Returns the next highest power of two if num is not a power of two
int Image2::PowerOfTwo(int num) {
	float q = (float)num;

	if(q != 1.)
		while( !((int)( q /= 2. ) % 2) && q != 1. );

	if(q != 1.) {
		// num is not a power of two
		int c = 1;
		while(c < num) c *= 2;
		return(c);
	} else {
		// num is a power of two
		return(num);
	}
}

// Converts an SDL surface to an OpenGL texture
bool Image2::ConvertToTexture( SDL_Surface *s ) {
	assert(s);

	// delete an old loaded image if one eixsts
	if( image ) {
		glDeleteTextures( 1, &image );
		image = 0;

		Log::Warning( "Loading an image after another is loaded already. Deleting old ... " );
	}

	// Check to see if we need to expand the image
	int expanded_w = PowerOfTwo(s->w);
	int expanded_h = PowerOfTwo(s->h);

	if(expanded_w == 1) expanded_w = 2; // many cards won't accept 1 as a power of two
	if(expanded_h == 1) expanded_h = 2;

	if((expanded_w != s->w) || (expanded_h != s->h)) {
		// Expand the canvas (needed)
		SDL_Surface *newSurface = NULL;
		newSurface = ExpandCanvas( s, expanded_w, expanded_h ); // ExpandCavas will set new scale_w/scale_h
		s = newSurface;
	}

	// real width/height always equal the expanded canvas (or original canvas if no expansion)'s w/h
	rw = s->w;
	rh = s->h;

	// check the pixel format, since it could depend on the file format:
	GLenum internal_format;
 	GLenum img_format, img_type;
	switch (s->format->BitsPerPixel) {
		case 32: img_format = GL_RGBA; img_type = GL_UNSIGNED_BYTE;
			internal_format = GL_RGBA8; break;
		case 24: img_format = GL_RGB; img_type = GL_UNSIGNED_BYTE;
			internal_format = GL_RGB8; break;
		case 16: img_format = GL_RGBA; img_type = GL_UNSIGNED_SHORT;
			internal_format = GL_RGB5_A1; break;
		default: img_format = GL_LUMINANCE; img_type = GL_UNSIGNED_BYTE;
			internal_format=GL_LUMINANCE8; break;
	}

	// generate the texture
	glGenTextures( 1, &image );

	// use the bitmap data stored in the SDL_Surface
	glBindTexture( GL_TEXTURE_2D, (unsigned int)image );

	// these settings depend entirely on how you intend to use the texture!
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// turn on linear filtering
	// upload the texture data, letting OpenGL do any required conversion.
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, img_format, img_type, s->pixels );

	return( true );
}

SDL_Surface *Image2::ExpandCanvas( SDL_Surface *s, int w, int h ) {
	SDL_Surface *expanded = NULL;
	SDL_Surface *original = s;
	
	// create the expanded surface
	expanded = SDL_CreateRGBSurface(original->flags, w, h, original->format->BitsPerPixel, 
	                                original->format->Rmask, original->format->Gmask, original->format->Bmask,
	                                original->format->Amask);
	assert( expanded );

	// make sure alpha values are copied properly
	SDL_SetAlpha( original, 0, SDL_ALPHA_OPAQUE );

	// copy the old image to the upper-left corner of the expanded canvas
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = original->w;
	area.h = original->h;
	SDL_BlitSurface( original, &area, expanded, &area );
	
	// re-calculate the texture coordinates given to opengl during drawing (u/v coordinates)
	scale_w = (float)original->w / (float)w;
	scale_h = (float)original->h / (float)h;
	
	// update the callee's pointer to the new image and free the old one
	SDL_FreeSurface( original );
	
	return( expanded );
}

