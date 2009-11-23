/**\file			animation.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/animation.h"
#include "Utilities/file.h"
#include "Utilities/log.h"

#define ANI_VERSION 1

/**\class Animation
 * \brief Animations */

Animation::Animation() {
	frames = NULL;
	startTime = 0;
	looping = false;
	w = h = 0;
}

Animation::Animation( string& filename ) {
	frames = NULL;
	startTime = 0;
	looping = false;
	w = h = 0;
	Load( filename );
}

bool Animation::Load( string& filename ) {
	char byte;
	const char *cName = filename.c_str();
	File file = File( cName );

	Log::Message( "Loading animation '%s' ...\n", cName );

	file.Read( 1, &byte );
	//cout << "\tVersion: " << (int)byte << endl;
	if( byte != ANI_VERSION ) {
		Log::Error( "Incorrect ani version" );
		return( false );
	}

	file.Read( 1, &byte );
	if( byte <= 0 ) {
		Log::Error( "Cannot have zero or less frames" );
		return( false );
	}
	numFrames = byte;
	//cout << "\tNum Frames: " << numFrames << endl;
	// Allocate space for frames
	frames = new Image[byte];

	file.Read( 1, &byte );
	if( byte <= 0 ) {
		Log::Error( "Cannot have zero or less for a delay" );
		delete [] frames;
		frames = NULL;
		return( false );
	}
	delay = byte;
	//cout << "\tDelay: " << delay << endl;

	for( int i = 0; i < numFrames; i++ ) {
		long pos;
		int fs;

		file.Read( sizeof(int), (char *) &fs );

		pos = file.Tell();

		// On OS X 10.6 with SDL_image 1.2.8, the load from fp is broken, so we load it into a buffer ourselves and SDL_image
		// loads from that correctly. It's an extra step on our part, but performance/functionally they're identical. Hopefully
		// this gets fixed in a future SDL_image
		char *buf = new char [fs];
		file.Read( fs, buf );

		//frames[i].Load( fp, (int)fs );
		frames[i].Load( buf, fs );

		delete [] buf;
		buf = NULL;

		file.Seek( pos + fs );
	}

	w = frames[0].GetWidth();
	h = frames[0].GetHeight();

	Log::Message( "Animation loading done." );

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

	frame->DrawCentered( x, y, ang );

	return( finished );
}

// Resets animation data back to the first frame
void Animation::Reset( void ) {
	startTime = 0;
}

