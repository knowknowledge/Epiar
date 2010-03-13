/**\file			music.cpp
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Implements background music playing ability.
 * \details
 */

#include "includes.h"
#include "Audio/music.h"
#include "Utilities/log.h"
#include "Utilities/resource.h"

/**\class Song
 * \brief This represents a song object.
 */

/**\brief Gets the song or loads it.
 * \param filename Song file
 */
Song *Song::Get( const string& filename ){
	Song* value;
	value = (Song*) Resource::Get( filename );
	if( value == NULL ){
		value = new Song( filename );
		Resource::Store( filename, (Resource*) value );
	}
	return value;
}

/**\brief Loads the song based on filename
 * \param filename Song file
 */
Song::Song( const string& filename ){
	this->song = NULL;
	this->song = Mix_LoadMUS( filename.c_str() );
	if ( this->song == NULL )
		Log::Error( "Could not load song file: %s, Mixer error: %s",
			filename.c_str(), Mix_GetError());
}

/**\brief Destructor to free the music file
 */
Song::~Song(){
	Mix_FreeMusic( this->song );
}

/**\brief Plays the current song.
 */
bool Song::Play( bool loop ){
	if ( this->song == NULL )
		return false;

	if ( loop )
		Mix_PlayMusic( this->song, 1 );
	else
		Mix_PlayMusic( this->song, 0 );
	/** \bug: SDL_mixer doesn't seem to respect VolumeMusic when starting a new song.*/
	Mix_VolumeMusic( Mix_VolumeMusic(-1) );
	return true;
}

