/*
 * Filename      : image.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Graphics/image.h"
#include "includes.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"
#include "Utilities/trig.h"

Image::Image( void ) {
	image = 0;
	masking = false;
	sw = sh = 0;
	tw = th = 1.;
}

Image::Image( string filename ) {
	image = 0;
	mask = 0;
	masking = false;
	sw = sh = 0;
	tw = th = 1.;
	this->filename = filename;
	Load( filename );
}

Image::Image( char *filename , char *maskname) {
	image = 0;
	mask = 0;
	masking = true;
	sw = sh = 0;
	tw = th = 1.;
	this->filename = filename;
	
	Log::Warning( "Image with masks is broken." );
}

Image::~Image( void ) {
	if( image )
		glDeleteTextures( 1, &image );
		glDeleteTextures( 1, &mask );
}

// Load an image from a buffer (e.g. a PNG stored in memory)
bool Image::Load( unsigned char *buf, int bufSize ) {
	SDL_RWops *rw;

	Log::Message( "Loading image from memory." );

	rw = SDL_RWFromMem( buf, bufSize );
	if( !rw ) {
		Log::Warning( "Image loading failed. Could not create RWops" );
		return( false );
	}

	SDL_Surface *texture = IMG_Load_RW( rw, 0 );
	SDL_FreeRW( rw );
	if( !texture ) {
		Log::Warning( "Image loading failed. Could not load image from RWops" );
		return( false );
	}

	if( _Load( texture ) == false ) {
		Log::Warning( "Image loading failed." );
		return( false );
	}

	return( true );
}

// Assumes fp is already open
bool Image::Load( FILE *fp, int size ) {
	SDL_RWops *rw;

	//Log::Message( "Loading image from file pointer." );

	if( !fp ) {
		Log::Warning( "Image loading failed. Invalid FILE pointer" );
		return( false );
	}

	rw = SDL_RWFromFP( fp, size );
	if( !rw ) {
		Log::Warning( "Image loading failed. Could not create RWops" );
		return( false );
	}

	SDL_Surface *texture = IMG_Load_RW( rw, 0 );
	SDL_FreeRW( rw );
	if( !texture ) {
		Log::Warning( "Image loading failed. Could not load image from RWops" );
		return( false );
	}

	if( _Load( texture ) == false ) {
		Log::Warning( "Image loading failed." );
		return( false );
	}

	return( true );
}

// set the image to be scaled to new width/height nw/nh
void Image::SetScaling( int nw, int nh ) {
	sw = nw;
	sh = nh;
}

bool Image::Load( string filename ) {
	SDL_Surface *texture = NULL;

	//Log::Message( "Loading image %s", filename.c_str() );

	if( ( texture = IMG_Load( filename.c_str() ) ) == NULL ) {
		Log::Warning( "Failed to load %s", filename.c_str() );
		return( false );
	}

	if( _Load( texture ) == false ) {
		Log::Warning( "Failed to load %s", filename.c_str() );
		SDL_FreeSurface( texture );

		return( false );
	}

	return( true );
}

// _Load will free the texture
bool Image::_Load( SDL_Surface *texture ) {

 	// check to ensure SDL_image loaded the file
 	if( texture ) {
		float q;
		int nw = 0, nh = 0; // possible new width/height for power of 2 adjustment

		Debug::Set();

		vw = texture->w;
		vh = texture->h;
		
		// ensure image dimensions are a power of two
		// check width
		q = texture->w;
		if( q != 1 )
			while( !((int)( q /= 2. ) % 2) && q != 1. );
		if( q != 1 ) {
			// texture->w is not a power of 2
			int c = 1;
			while( c < texture->w ) c *= 2;
			nw = c;
			Debug::Print( "%s: Width of image (%d) is not a power of two. Expansion yields new width (%d)", filename.c_str(), texture->w, nw );
		}
		// check height
		q = texture->h;
		if( q != 1 )
			while( !((int)( q /= 2. ) % 2) && q != 1. );
		if( q != 1 ) {
			// texture->w is not a power of 2
			int c = 1;
			while( c < texture->h ) c *= 2;
			nh = c;
			Debug::Print( "%s: Height of image (%d) is not a power of two. Expansion yields new height (%d)", filename.c_str(), texture->h, nh );
		}

		if( texture->w == 1 ) nw = 2; // many cards won't accept 1 as a valid power of two
		if( texture->h == 1 ) nh = 2;

		if( nw || nh ) {
			// The image dimensions are not a power of 2. We need to expand the canvas. This is an OpenGL restriction.
			if( nw == 0 ) nw = texture->w;
			if( nh == 0 ) nh = texture->h; // just in case one dimension needed expansion and not the other
			
			assert( nw >= texture->w );
			assert( nh >= texture->h );
			
			// Expand the canvas
			SDL_Surface *newSurface = NULL;
			Debug::Print("%s will have an expanded canvas\n", filename.c_str());
			newSurface = ExpandCanvas( texture, nw, nh );
			texture = newSurface;
		}

		// delete an old loaded image if one eixsts
		if( image ) {
			glDeleteTextures( 1, &image );
			image = 0;

			Log::Warning( "Loading an image after another is loaded already. Deleting old ... " );
		}

		w = texture->w;
		h = texture->h;

		// check the pixel format, since it could depend on the file format:
		GLenum internal_format;
 		GLenum img_format, img_type;
		switch (texture->format->BitsPerPixel) {
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
		glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, img_format, img_type, texture->pixels );

		masking = false;

		Debug::Unset();		

		return( true );
	}

	return( false );
}

bool Image::o_Load( char *filename, char *maskname) {
	SDL_Surface *texture;
	SDL_Surface *textureMask;

	texture = IMG_Load( filename );
	textureMask = IMG_Load( maskname );

	// check to ensure SDL_image loaded the file
	if( texture ) {
		// delete an old loaded image if one eixsts
		if( image ) {
			glDeleteTextures( 1, &image );
			image = 0;

			Log::Warning( "Loading an image after another is loaded already. Deleting old ... " );
		}

		w = texture->w;
		h = texture->h;

		// check the pixel format, since it could depend on the file format:
		GLenum internal_format;
		GLenum img_format, img_type;
		switch (texture->format->BitsPerPixel) {
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
		//	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h,
		//			img_format, GL_RGB, GL_UNSIGNED_BYTE, texture->pixels);
	
		glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, img_format, img_type, texture->pixels );
	
		// the SDL_Surface is no longer needed
		SDL_FreeSurface( texture );

		//	logger->Message( "Image '%s' loaded successfully\n", filename );
		} else {
			Log::Warning( "Failed to load file '%s'\n ", filename );
		}

		if( textureMask ) {
		// delete an old loaded image if one eixsts
		if( mask ) {
			glDeleteTextures( 1, &mask );
			mask = 0;

			Log::Warning( "Loading an image after another is loaded already. Deleting old ... " );
		}

		w = texture->w;
		h = texture->h;

		// check the pixel format, since it could depend on the file format:
		GLenum internal_format;
		GLenum img_format, img_type;
		switch (textureMask->format->BitsPerPixel) {
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
		glGenTextures( 1, &mask );

		// use the bitmap data stored in the SDL_Surface
		glBindTexture( GL_TEXTURE_2D, (unsigned int)mask );

		// these settings depend entirely on how you intend to use the texture!
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, textureMask->w, textureMask->h, img_format, GL_RGB, GL_UNSIGNED_BYTE, textureMask->pixels);
		// turn on linear filtering
		// upload the texture data, letting OpenGL do any required conversion.
		//glTexImage2D( GL_TEXTURE_2D, 0, internal_format,
		//w, h, 0,
		//img_format, img_type, textureMask->pixels );

		// the SDL_Surface is no longer need
		SDL_FreeSurface( textureMask );

		//logger->Message( "Mask '%s' loaded successfully\n", filename );
		masking = true;
		return( true );
	} else {
		Log::Warning( "Failed to load file '%s'\n ", filename );

		return( false );
	}
}

void Image::Draw( int x, int y, float ang ) {
	float ulx, urx, llx, lrx; // the four rotated corners of the image
	float uly, ury, lly, lry;
	Trig *trig = Trig::Instance();
	float a;

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
	
	int ow = w, oh = h;
	if(( sw > 0 ) && ( sh > 0 )) {
		// we need to scale. we do this by simply lying about the image's size to opengl
		w = sw;
		h = sh;
		Debug::Print("%s: Setting artifical scale, originally %d,%d, now %d,%d\n", filename.c_str(), ow, oh, sw, sh);
	} else {
		Debug::Print("%s: Not setting artifical scale.\n", filename.c_str());
	}
	
	// avoid trig when you can
	if( ang != 0 ) {
		a = -(float)trig->DegToRad( ang );
		trig->RotatePoint( (float)(x - ((w * tw)/2)), (float)(y + ((h * th)/2)), (float)x, (float)y, (float *)&ulx, (float *)&uly, a );
		trig->RotatePoint( (float)(x + ((w * tw)/2)), (float)(y + ((h * th)/2)), (float)x, (float)y, (float *)&urx, (float *)&ury, a );
		trig->RotatePoint( (float)(x - ((w * tw)/2)), (float)(y - ((h * th)/2)), (float)x, (float)y, (float *)&llx, (float *)&lly, a );
		trig->RotatePoint( (float)(x + ((w * tw)/2)), (float)(y - ((h * th)/2)), (float)x, (float)y, (float *)&lrx, (float *)&lry, a );
	} else {
		ulx = x - ((w * tw) / 2);
		urx = x + ((w * tw) / 2);
		llx = ulx;
		lrx = urx;
		uly = y - ((h * th) / 2);
		ury = uly;
		lly = y + ((h * th) / 2);
		lry = lly;
	}

	//for masking
	if (masking == true){
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture( GL_TEXTURE_2D, mask );
		glBegin( GL_QUADS );

		glTexCoord2f( 0., th );
		glVertex2f( ulx, uly );
		glTexCoord2f( tw, th );
		glVertex2f( urx, ury );
		glTexCoord2f( tw, 0. );
		glVertex2f( lrx, lry );
		glTexCoord2f( 0., 0. );
		glVertex2f( llx, lly );

		glEnd();
	}

	//normal draw
	glColor3f(1,1,1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, image );

	glPushMatrix();

	glBegin( GL_POLYGON );

	Debug::Print("text coordinates: 0, 0, %f, %f\n", tw, th);
	Debug::Print("vert coordinates: %f, %f, %f, %f\n", ulx, uly, lrx, lry);
	glTexCoord2f( 0., th );
	glVertex2f( ulx, uly );
	glTexCoord2f( tw, th );
	glVertex2f( urx, ury );
	glTexCoord2f( tw, 0. );
	glVertex2f( lrx, lry );
	glTexCoord2f( 0., 0. );
	glVertex2f( llx, lly );

	glEnd();

	glPopMatrix();

	// "undo" our scaling effect by restoring the true w/h values
	if(( sw > 0 ) && ( sh > 0 )) {
		w = ow;
		h = oh;
	}
	
	glEnable(GL_DEPTH_TEST); // Enable Depth Testing
	glDisable(GL_BLEND); // Disable Blending

	glDisable(GL_TEXTURE_2D); // Disable 2D Texture Mapping
	glBindTexture(GL_TEXTURE_2D,0); // Unbind The Blur Texture
}

void Image::LoadAndDraw(int x, int y, float ang, char *filename) {
	Load(filename);
	Draw(x,y,ang);
}

// Normally, the coordinates are the center of the image - DrawAbsolute forces (x,y) to be the upper-left of the image
void Image::DrawAbsolute( int x, int y ) {
	int w, h;

	// if we use scaling, we account for it by using a fake w/h and simply pass that onto OpenGL
	if(( sw > 0 ) && ( sh > 0 )) {
		w = sw;
		h = sh;
	} else {
		w = this->w;
		h = this->h;
	}
	
	x += (int)((double)w / 2.);
	y -= (int)((double)h / 2.);
	
	Debug::Print("DrawAbsolute is passing to Draw the coordinates %d, %d", x, y);
	
	Draw( x, y );
}

// calls draw absolute to fill the rectangle (x, y, w, h) NOTE: TOFIX if the tile is not 1x1, this will "spill" over a little
void Image::DrawAbsoluteTiled( int x, int y, int w, int h ) {
	// set the clipping region to avoid tiles spilling out
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y - h, w, h);
	
	for( int j = 0; j < h; j += (this->h) ) {
		for( int i = 0; i < w; i += (this->w) ) {
			DrawAbsolute( x + i, y - j );
		}
	}
	
	// restore previous clipping region
	glDisable(GL_SCISSOR_TEST);
}

void Image::Draw( int x, int y ) {
	Draw( x, y, 0. );
}

int Image::GetWidth( void ) {
	return( vw );
}

int Image::GetHeight( void ) {
	return( vh );
}

SDL_Surface *Image::ExpandCanvas( SDL_Surface *s, int w, int h ) {
	SDL_Surface *expanded = NULL;
	SDL_Surface *original = s;
	
	// create the expanded surface
	expanded = SDL_CreateRGBSurface(original->flags, w, h, original->format->BitsPerPixel, 
            original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);
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
	tw = (float)original->w / (float)w;
	th = (float)original->h / (float)h;
	
	cout << "expanded canvas, tw/th is " << tw << ", " << th << endl;
	
	// update the callee's pointer to the new image and free the old one
	SDL_FreeSurface( original );
	
	return( expanded );
}
