/**\file			music.h
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Implements background music playing ability.
 * \details
 */

#ifndef __H_MUSIC__
#define __H_MUSIC__

#include "includes.h"
#include "Audio/audio.h"

class Song {
	public:
		static Song *Get( const string& filename );
		Song( const string& filename );
		~Song( void );
		bool Play( bool loop=true );
	private:
		Mix_Music *song;
};


class Music {
	public:
		static Music& Instance();
		bool LoadPlaylist( );
		bool LoadSong( const string& filename );
		bool InsertSong();
		bool Play();
		bool Pause();
		bool Stop();

	private:
		Music();
		Music(Music const&){};				// Copy constructor
		Music& operator=(Music const&){};	// Assignment constructor
		~Music();
		list<string> playlist;
};

#endif // __H_MUSIC__
