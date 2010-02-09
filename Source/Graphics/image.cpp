/**\file			image.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 31, 2009
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Image loading and display
 * \details
 * See this note section in image.h for an important clarification about the handling
 * of non-power of two image sizes and the difference between virtual/effective dimensions
 * and real dimensions.
 */

#include "includes.h"
#include "Graphics/image.h"
#include "Graphics/video.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/trig.h"

/**\class Image
 * \brief Image handling. */

Image::Image() {
	// Initialize variables
	w = h = real_w = real_h = image = 0;
	scale_w = scale_h = 1.;
}

// Create instance by loading image from file
Image::Image( const string& filename ) {
	// Initialize variables
	w = h = real_w = real_h = image = 0;
	scale_w = scale_h = 1.;

	Load(filename);
}

//Deallocate allocations
Image::~Image() {
	if ( image ) {
		glDeleteTextures( 1, &image );
		image = 0;
	}
}

// Lazy fetch an Image
Image* Image::Get( string filename ) {
	Image* value;
	value = (Image*)Resource::Get(filename);
	if( value == NULL ) {
		value = new Image(filename);
		Resource::Store(filename,(Resource*)value);
	}
	return value;
}

// Load image from file
bool Image::Load( const string& filename ) {
	File file = File( filename );
	char* buffer = file.Read();
	int bytesread = file.GetLength();

	if ( buffer == NULL )
		return NULL;

	int retval = Load( buffer, bytesread );
	delete [] buffer;
	if ( retval ){
		return true;
	}
	return NULL;
}

// Load image from buffer
bool Image::Load( char *buf, int bufSize ) {
	SDL_RWops *rw;
	SDL_Surface *s = NULL;

	rw = SDL_RWFromMem( buf, bufSize );
	if( !rw ) {
		Log::Warning( "Image loading failed. Could not create RWops" );
		return( false );
	}

	s = IMG_Load_RW( rw, 0 );

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
void Image::Draw( int x, int y, float angle, float resize_ratio_w, float resize_ratio_h) {
	// the four rotated (if needed) corners of the image
	float ulx, urx, llx, lrx, uly, ury, lly, lry;

	assert(image);
	if( !image ) {
		Log::Warning( "Trying to draw without loading an image first." );
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear The Background Color To Black
	glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
	glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
	glEnable(GL_TEXTURE_2D); // Enable 2D Texture Mapping
 	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// calculate the coordinates of the quad	
	// avoid trig when you can
	if( angle != 0. ) {
		Trig *trig = Trig::Instance();
		float a = (float)trig->DegToRad( angle );
		// ax/ay are the coordinate to rotate "about", hence "about points", "about x", "about y"
		float ax = static_cast<float>(x + (w / 2.));
		float ay = static_cast<float>(y + (h / 2.));

		trig->RotatePoint( (float)x, (float)y + h, ax, ay, (float *)&ulx, (float *)&uly, a );
		trig->RotatePoint( (float)x + w, (float)y + h, ax, ay, (float *)&urx, (float *)&ury, a );
		trig->RotatePoint( (float)x, (float)y, ax, ay, (float *)&llx, (float *)&lly, a );
		trig->RotatePoint( (float)x + w, (float)y, ax, ay, (float *)&lrx, (float *)&lry, a );
	} else {
		ulx = static_cast<float>(x);
		urx = static_cast<float>(x + w);
		llx = static_cast<float>(x);
		lrx = static_cast<float>(x + w);
		uly = static_cast<float>(y + h);
		ury = static_cast<float>(y + h);
		lly = static_cast<float>(y);
		lry = static_cast<float>(y);
	}

	// draw!
	glColor3f(1, 1, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, image );

	glPushMatrix();
	
	// the deltas are the differences needed in width, e.g. a resize_ratio_w of 1.1 would produce a value
	// equal to the original width of the image but adding 10%. 0.9 would then be 10% smaller, etc.
	float resize_w_delta = (w * resize_ratio_w) - w;
	float resize_h_delta = (h * resize_ratio_h) - h;

	glBegin( GL_QUADS );
	glTexCoord2f( 0., 0. ); glVertex2f( llx, lly );
	glTexCoord2f( scale_w, 0. ); glVertex2f( lrx + resize_w_delta, lry );
	glTexCoord2f( scale_w, scale_h ); glVertex2f( urx + resize_w_delta, ury + resize_h_delta );
	glTexCoord2f( 0., scale_h ); glVertex2f( ulx, uly + resize_h_delta );
	glEnd();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST); // Enable Depth Testing
	glDisable(GL_BLEND); // Disable Blending

	glDisable(GL_TEXTURE_2D); // Disable 2D Texture Mapping
	glBindTexture(GL_TEXTURE_2D,0); // Unbind The Blur Texture
}

// Draw the image centered on (x,y)
void Image::DrawCentered( int x, int y, float angle ) {
	Draw( x - (w / 2), y - (h / 2), angle );
}

// Draw the image stretched within to a box
void Image::DrawStretch( int x, int y, int box_w, int box_h, float angle ) {
	float resize_ratio_w = static_cast<float>(box_w) / static_cast<float>(this->w);
	float resize_ratio_h = static_cast<float>(box_h) / static_cast<float>(this->h);
	Draw(x, y, angle, resize_ratio_w, resize_ratio_h);
}

// Draw the image within a box but not stretched
void Image::DrawFit( int x, int y, int box_w, int box_h, float angle ) {
	float resize_ratio_w = (float)box_w / (float)this->w;
	float resize_ratio_h = (float)box_h / (float)this->h;
	// Use Minimum of the two ratios
	float resize_ratio = resize_ratio_w<resize_ratio_h ? resize_ratio_w : resize_ratio_h;
	Draw(x, y, angle, resize_ratio, resize_ratio);
}

// Returns the next highest power of two if num is not a power of two
int Image::PowerOfTwo(int num) {
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

// Converts an SDL surface to an OpenGL texture. Will free 's' by design. Do not do anything with it after this point.
bool Image::ConvertToTexture( SDL_Surface *s ) {
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
	real_w = s->w;
	real_h = s->h;

	// check the pixel format, since it could depend on the file format:
	GLenum internal_format;
 	GLenum img_format, img_type;
	switch (s->format->BitsPerPixel) {
		case 32:
			img_format = GL_RGBA;
			if(s->format->Bmask != 0x00ff0000)
				img_format = GL_BGRA;
			img_type = GL_UNSIGNED_BYTE;
			internal_format = GL_RGBA8;
			break;
		case 24:
			img_format = GL_RGB;
			img_type = GL_UNSIGNED_BYTE;
			internal_format = GL_RGB8;
			break;
		case 16:
			img_format = GL_RGBA;
			img_type = GL_UNSIGNED_SHORT;
			internal_format = GL_RGB5_A1;
			break;
		default:
			img_format = GL_LUMINANCE;
			img_type = GL_UNSIGNED_BYTE;
			internal_format=GL_LUMINANCE8;
			break;
	}

	// generate the texture
	glGenTextures( 1, &image );

	// use the bitmap data stored in the SDL_Surface
	glBindTexture( GL_TEXTURE_2D, (unsigned int)image );

	// upload the texture data, letting OpenGL do any required conversion.
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, real_w, real_h, 0, img_format, img_type, s->pixels );

	// linear filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	SDL_FreeSurface( s );

	return( true );
}

// Draw the image tiled to fill a rectangle of w/h - will crop to meet w/h and won't overflow
void Image::DrawTiled( int x, int y, int fill_w, int fill_h )
{
	if( !image ) {
		Log::Warning( "Trying to draw without loading an image first." );
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear The Background Color To Black
	glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
	glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
	glEnable(GL_TEXTURE_2D); // Enable 2D Texture Mapping
 	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// draw it
	glColor3f(1, 1, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, image );

	glPushMatrix();

	glEnable(GL_SCISSOR_TEST);
	glScissor(x, Video::GetHeight() - y - fill_h, fill_w, fill_h); // for some reason, glScissor counts (0,0) as lower-left

	glBegin( GL_QUADS );
	for( int j = 0; j < fill_h; j += h) {
		for( int i = 0; i < fill_w; i += w) {
			//cout<<"Image "<<j<<","<<i<<endl;
			glTexCoord2f( 0., 0. ); glVertex2f( static_cast<GLfloat>(x+i), static_cast<GLfloat>(y+j) ); // Lower Left
			glTexCoord2f( scale_w, 0. ); glVertex2f( static_cast<GLfloat>(x+w+i) , static_cast<GLfloat>(y+j)); // Lower Right
			glTexCoord2f( scale_w, scale_h ); glVertex2f( static_cast<GLfloat>(x+w+i) , static_cast<GLfloat>(y+h+j) ); // Upper Right
			glTexCoord2f( 0., scale_h ); glVertex2f( static_cast<GLfloat>(x+i), static_cast<GLfloat>(y+h+j) ); // Upper Left
		}
	}
	glEnd();

	glDisable(GL_SCISSOR_TEST);

	glPopMatrix();

	glEnable(GL_DEPTH_TEST); // Enable Depth Testing
	glDisable(GL_BLEND); // Disable Blending

	glDisable(GL_TEXTURE_2D); // Disable 2D Texture Mapping
	glBindTexture(GL_TEXTURE_2D,0); // Unbind The Blur Texture
}


// Will destroy 's' so don't do anything with it after this and don't worry about freeing it (it's freed here)
// e.g. proper usage: convert = ExpandCanvas( convert, w, h );
SDL_Surface *Image::ExpandCanvas( SDL_Surface *s, int w, int h ) {
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

