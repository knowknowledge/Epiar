/**\file			sound.cpp
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Implements sound playing abilities.
 * \details
 */

#include "includes.h"
#include "Audio/audio.h"
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

	this->channel = Audio::Instance().PlayChannel( -1, this->sound, 0 );
	if ( channel == -1 )
		return false;
	
	return true;
}

/**\brief Plays the sound at a specified coordinate from origin.
 */
bool Sound::Play( Coordinate offset ){
	/**\todo Distance fading: consider tweaking this scaling factor.*/
	double dist = 0.1 * offset.GetMagnitude();
	if ( dist > 255 )
		return false;			// Sound is out of range

	Uint8 sounddist = static_cast<Uint8>( dist );
	/**\todo Panning: consider tweaking this scaling factor.*/
	float panx = 0.1*(offset.GetX())+127;
	Uint8 soundpan = 127;
	if ( panx < 0 )
		soundpan = 0;
	else if ( panx > 254 )
		soundpan = 254;
	else
		soundpan = static_cast<Uint8>( panx );

	int freechan = Audio::Instance().GetFreeChannel();
	if( Mix_SetDistance( freechan, sounddist ) == 0 )
		Log::Error("Set distance %d failed on channel %d.", sounddist, freechan );
	else
		Log::Message("Distance set to %d on channel %d.", sounddist, freechan );
	if( Mix_SetPanning( freechan, soundpan, 254 - soundpan ) == 0 )
		Log::Error("Set panning %d failed on channel %d.", soundpan - 127, freechan );
	else
		Log::Message("Panning set to %d on channel %d.", soundpan - 127, freechan );

	this->channel = Audio::Instance().PlayChannel( freechan, this->sound, 0 );

	if ( channel == -1 )
		return false;
	
	return true;

}
