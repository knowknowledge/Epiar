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
		bool Play( Coordinate offset );
		bool PlayNoRestart( Coordinate offset );
		void SetFactors( double fade, float pan );

	private:
		Mix_Chunk *sound;
		int channel;		/* Last channel the sound is playing on. */
		double fadefactor;	// Scale factor to fade by as distance drops off
		float panfactor;	// Scale factor to pan by, higher = more sensitive
};


#endif // __H_SOUND__
