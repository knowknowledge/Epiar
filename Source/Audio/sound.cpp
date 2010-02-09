/**\file			sound.cpp
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Implements sound playing abilities.
 * \details
 */

#include "includes.h"
#include "Audio/sound.h"
#include "Utilities/log.h"
#include "Utilities/resource.h"

/**\class Sound
 * \brief This represents a sound object.
 */

/**\brief Gets the sound or loads it.
 * \param filename Sound file
 */
Sound *Sound::Get( const string& filename ){
	Sound* value;
	value = (Sound*) Resource::Get( filename );
	if( value == NULL ){
		value = new Sound( filename );
		Resource::Store( filename, (Resource*) value );
	}
	return value;
}

/**\brief Loads the sound based on filename
 * \param filename Sound file
 */
Sound::Sound( const string& filename ){
	this->sound = NULL;
	this->sound = Mix_LoadWAV( filename.c_str() );
	if( this->sound == NULL )
		Log::Error( "Could not load sound file: %s, Mixer error: %s",
				filename.c_str(), Mix_GetError() );
}

/**\brief Destructor to free the sound file
 */
Sound::~Sound(){
	Mix_HaltChannel( this->channel );
	Mix_FreeChunk( this->sound );
}

/**\brief Plays the sound.
 */
bool Sound::Play( void ){
	if ( this->sound == NULL )
		return false;

	if ( Mix_Playing( -1 ) >31 )
		return false;

	this->channel = Mix_PlayChannel( -1, this->sound, 0 );
	if ( channel == -1 )
		return false;
	
	return true;
}
