/*
 * Filename      : animation.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Graphics/animation.h"
#include "Utilities/log.h"

#define ANI_VERSION 1

Animation::Animation() {
	frames = NULL;
	startTime = 0;
	looping = false;
	w = h = 0;
}

Animation::Animation( string filename ) {
	frames = NULL;
	startTime = 0;
	looping = false;
	w = h = 0;
	Load( filename );
}

bool Animation::Load( string filename ) {
	char byte;
	FILE *fp = NULL;

	if( ( fp = fopen( filename.c_str(), "rb" ) ) == NULL ) {
		Log::Error( "Could not load %s", filename.c_str() );
		return( false );
	}

	//Log::Message( "Loading animation '%s' ...\n", filename.c_str() );

	fread( 	&byte, sizeof( byte ), 1, fp );
	//cout << "\tVersion: " << (int)byte << endl;
	if( byte != ANI_VERSION ) {
		Log::Error( "Incorrect ani version" );
		fclose( fp );
		return( false );
	}

	fread( &byte, sizeof( byte ), 1, fp );
	if( byte <= 0 ) {
		Log::Error( "Cannot have zero or less frames" );
		fclose( fp );
		return( false );
	}
	numFrames = byte;
	// Allocate space for frames
	frames = new Image[byte];

	fread( &byte, sizeof( byte ), 1, fp );
	if( byte <= 0 ) {
		Log::Error( "Cannot have zero or less for a delay" );
		fclose( fp );
		delete [] frames;
		frames = NULL;
		return( false );
	}
	delay = byte;

	for( int i = 0; i < numFrames; i++ ) {
		long fs, pos;

		fread( &fs, sizeof( fs ), 1, fp );

		pos = ftell( fp );

		frames[i].Load( fp, fs );

		fseek( fp, pos + fs, SEEK_SET );
	}

	fclose( fp );

	w = frames[0].GetWidth();
	h = frames[0].GetHeight();

	//Log::Message( "Animation loading done." );

	return( true );
}

// Returns true while animation is still playing - false when animation is over
// Note: if looping is turned on, the animation will always return true
bool Animation::Draw( int x, int y, float ang ) {
	Image *frame = NULL;
	bool finished = false;

	if( startTime ) {
		int fnum = (SDL_GetTicks() - startTime) / delay;

		if( fnum > numFrames - 1 ) {
			if( looping ) {
				fnum = 0;
				startTime = SDL_GetTicks();
			} else {
				fnum = numFrames - 1;
				finished = true;
			}
		}

		frame = &frames[fnum];
	} else {
		startTime = SDL_GetTicks();
		frame = &frames[0];
	}

	frame->Draw( x, y, ang );

	return( finished );
}

// Resets animation data back to the first frame
void Animation::Reset( void ) {
	startTime = 0;
}

