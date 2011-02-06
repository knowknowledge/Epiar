/**\file			animation.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/animation.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/resource.h"


#define ANI_VERSION 1

/** \class Ani
 *  \brief An animation data object
 *  \details The Ani class is a package of Images allocated adjacent to one
 *  another.  The Ani also knows how long each frame should last (0 to 255 ms).
 *  A single Ani object is meant to be shared between multiple Animation
 *  objects.  The Ani object stores the Image frames, and the Animation object
 *  knows what frame it is currently on.  This implementation split is done to
 *  make sharing the Animation Resource possible between many different
 *  instances.
 *  
 *  The .ani filetype is Epiar specific.
 *
 *  ANI_VERSION 1:
 *
 * 	- One byte of delay time
 *
 *  - One byte of number of frames
 *
 *  - Multiple Images concatenated together
 *  
 *  The external python script "ani.py" can be used to extract, modify, and create .ani files.
 *
 *  \warning Since this file format is developed specifically for Epiar it is more fragile than other file formats.  For example, it makes endianess assumptions that require the bytes be swapped before it can be loaded on Big Endian machines.
 *  \see Animation
 */

/**\brief Gets the resource object.
 * \param filename string containing the animation
 */
Ani* Ani::Get( string filename ) {
	Ani* value;
	value = (Ani*)Resource::Get(filename);
	if( value == NULL ) {
		value = new Ani(filename);
		Resource::Store(filename,(Resource*)value);
	}
	return value;
}

/**\brief The resource object (no file).
 */
Ani::Ani() {
	frames = NULL;
	delay = 0;
	numFrames = 0;
	w = h = 0;
}

/**\brief The resource object based on the file.
 * \param filename String pointer to file.
 * \sa Ani::Load
 */
Ani::Ani( string& filename ) {
	LogMsg(INFO,"New Animation from '%s'", filename.c_str() );
	frames = NULL;
	delay = 0;
	numFrames = 0;
	w = h = 0;
	Load( filename );
}

/**\brief Loads the animation file.
 * \param filename File name of the animation
 */
bool Ani::Load( string& filename ) {
	char byte;
	const char *cName = filename.c_str();
	File file = File( cName );

	LogMsg(INFO, "Loading animation '%s'", cName );

	file.Read( 1, &byte );
	if( byte != ANI_VERSION ) {
		LogMsg(ERR, "Incorrect ani version" );
		return( false );
	}

	file.Read( 1, &byte );
	if( byte <= 0 ) {
		LogMsg(ERR, "Cannot have zero or less frames" );
		return( false );
	}
	numFrames = byte;
	// Allocate space for frames
	frames = new Image[byte];

	file.Read( 1, &byte );
	if( byte <= 0 ) {
		LogMsg(ERR, "Cannot have zero or less for a delay" );
		delete [] frames;
		frames = NULL;
		return( false );
	}
	delay = byte;

	for( int i = 0; i < numFrames; i++ ) {
		long pos;
		int fs;

		file.Read( sizeof(int), (char *) &fs );

		// Big Endian Machines need to swap the bytes here.
		if( IsBigEndian() ) {
			LogMsg(ERR, "Swapping Bytes for Endianness");
			fs = SDL_SwapLE32(fs);
		}

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

	//LogMsg(INFO, "Animation loading done." );

	return( true );
}

/** \brief Get the Image at a specific Frame
 * 	\param[in] frameNum
 * 	\returns Image pointer;
 */
Image* Ani::GetFrame(int frameNum) {
	assert(frames);
	assert(frameNum >= 0);
	assert(frameNum < numFrames);
	return &(frames[frameNum]);
}

/**\var Ani::frames
 *  \brief Frames of the animation as Image objects
 */
/**\var Ani::numFrames
 *  \brief Number of frames
 */
/**\var Ani::delay
 *  \brief Animation delay
 */
/**\var Ani::w
 *  \brief Width of Ani
 */
/**\var Ani::h
 *  \brief Height of Ani
 */

/** \class Animation
 *  \brief Animations implementation.
 *  \details The Animation class is used for each instantiation of an
 *  animation.  Many Animations can share the same Ani object while each having
 *  a different timestamp.
 *  \note The Animation uses "real" clocktime, so they will continue to play
 *  while the game is paused.
 *  \see Ani, Effect
 */

/**\brief Empty constructor.
 */
Animation::Animation() {
	fnum=0;
	startTime = 0;
	loopPercent = 0.0f;
}

/**\brief Constructor (based on file).
 * \param filename File to load.
 * \sa Ani::Get
 */
Animation::Animation( string filename ) {
	fnum=0;
	startTime = 0;
	loopPercent = 0.0f;
	ani = Ani::Get( filename );
}

/**\brief Returns true while animation is still playing.
 * \details
 * false when animation is over
 * Note: if looping is turned on, the animation will always return true.
 */
bool Animation::Update() {
	
	Image *frame = NULL;
	bool finished = false;

	if( startTime ) {
		fnum = (SDL_GetTicks() - startTime) / ani->GetDelay();

		if( fnum > ani->GetNumFrames() - 1 ) {
			fnum = TO_INT(ani->GetNumFrames() * (1.0f-loopPercent)); // Step back a few frames.
			startTime = SDL_GetTicks() - ani->GetDelay()*fnum; // Pretend that we started fnum frames ago
			if( loopPercent <= 0.0f ) {
				finished = true;
			}
		}

	} else {
		startTime = SDL_GetTicks();
		frame = ani->GetFrame(0);
	}
	return finished;
}

/**\brief Draws the animation at given coordinate.
 */
void Animation::Draw( int x, int y, float ang ) {
	Image* frame = ani->GetFrame( fnum );
	frame->DrawCentered( x, y, ang );
}


/**\brief Resets animation data back to the first frame.
 */
void Animation::Reset( void ) {
	fnum=0;
	startTime = 0;
}

/**\fn Animation::SetLoopPercent( float newLoopPercent )
 *  \brief Set the amount that the animation should loop.
 *  \details 0.0 means that this does not loop at all.
 *           0.5 means that this loops back to the halfway point.
 *           1.0 means that this loops back to the beginning.
 *  \param[in] newLoopPercent must be between +0.0f and +1.0f.
 */
void Animation::SetLoopPercent( float newLoopPercent ) {
	if( newLoopPercent < 0.0f ) {
		loopPercent = 0.0f;
	} else if( newLoopPercent > 1.0f ) {
		loopPercent = 1.0f;
	} else {
		loopPercent = newLoopPercent;
	}
}

/* \fn Animation::GetLooping( void )
 *  \brief Returns the status of the looping.
 */

/**\fn Animation::GetHalfWidth( void )]
 *  \brief Returns half the width of the animation
 * \fn Animation::GetHalfHeight( void )
 *  \brief Returns half the height of the animation
 */

