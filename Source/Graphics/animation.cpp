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

/**\class Ani
 * \brief Animations */

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

	//LogMsg(INFO, "Loading animation '%s' ...\n", cName );

	file.Read( 1, &byte );
	//cout << "\tVersion: " << (int)byte << endl;
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
	//cout << "\tNum Frames: " << numFrames << endl;
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

	//LogMsg(INFO, "Animation loading done." );

	return( true );
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

/**\class Animation
 * \brief Animations implementation. */

/**\brief Empty constructor.
 */
Animation::Animation() {
	fnum=0;
	startTime = 0;
	looping = false;
}

/**\brief Constructor (based on file).
 * \param filename File to load.
 * \sa Ani::Get
 */
Animation::Animation( string filename ) {
	fnum=0;
	startTime = 0;
	looping = false;
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
		fnum = (SDL_GetTicks() - startTime) / ani->delay;

		if( fnum > ani->numFrames - 1 ) {
			if( looping ) {
				fnum= 0;
				startTime = SDL_GetTicks();
			} else {
				fnum = ani->numFrames - 1;
				finished = true;
			}
		}

	} else {
		startTime = SDL_GetTicks();
		frame = &(ani->frames)[0];
	}
	return finished;
}

/**\brief Draws the animation at given coordinate.
 */
void Animation::Draw( int x, int y, float ang ) {
	Image* frame = &(ani->frames)[fnum];
	frame->DrawCentered( x, y, ang );
}


/**\brief Resets animation data back to the first frame.
 */
void Animation::Reset( void ) {
	fnum=0;
	startTime = 0;
}

/**\fn Animation::SetLooping( bool looping )
 *  \brief Set to true to loop the animation
 * \fn Animation::GetLooping( void )
 *  \brief Returns the status of the looping.
 * \fn Animation::GetHalfWidth( void )]
 *  \brief Returns half the width of the animation
 * \fn Animation::GetHalfHeight( void )
 *  \brief Returns half the height of the animation
 */

