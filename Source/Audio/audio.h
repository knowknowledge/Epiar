/**\file			audio.h
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Abstraction to SDL_mixer interface.
 * \details
 * This files is responsible for overall Audio system configuration.  To play
 * music or sound see the corresponding Music and Sound classes.
 */

#ifndef __H_AUDIO__
#define __H_AUDIO__

#include "includes.h"

class Audio {
	public:
		static Audio& Instance();
		bool Initialize( void );
		bool Shutdown( void );
		bool SetMusicVol ( int volume );

	private:
		Audio();
		Audio(Audio const&){};				// Copy constructor
		Audio& operator=(Audio const&){};	// Assignment constructor
		~Audio();
};

#endif // __H_AUDIO__
