/**\file			sound.h
 * \author			Maoserr
 * \date			Created: Monday, February 08, 2010
 * \date			Modified: Monday, February 08, 2010
 * \brief			Implements sound playing abilities.
 * \details
 */

#ifndef __H_SOUND__
#define __H_SOUND__

#include "Utilities/coordinate.h"

class Sound {
	public:
		static Sound *Get( const string& filename );
		Sound( const string& filename );
		~Sound( void );
		bool Play( void );
		bool Play( const Coordinate& offset );

	private:
		Mix_Chunk *sound;
		int channel;		/* Channel the sound is playing on. */
};


#endif // __H_SOUND__
