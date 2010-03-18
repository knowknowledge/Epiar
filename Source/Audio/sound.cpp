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
Sound::Sound( const string& filename ):
	sound( NULL ),
	pathName(filename),
	channel( -1 ),
	fadefactor( 0.03 ),
	panfactor( 0.1f ),
	volume( 128 )
{
	this->sound = Mix_LoadWAV( filename.c_str() );
	if( this->sound == NULL )
		Log::Error( "Could not load sound file: %s, Mixer error: %s",
				filename.c_str(), Mix_GetError() );
}

/**\brief Destructor to free the sound file.
 */
Sound::~Sound(){
	// Halts any channel this sound is playing on
	for ( int i = 0; i < Audio::Instance().GetTotalChannels(); i++ ){
		if ( Mix_GetChunk( i ) == this->sound)
			Mix_HaltChannel( i );
	}
	Mix_FreeChunk( this->sound );
}

/**\brief Plays the sound.
 */
bool Sound::Play( void ){
	if ( this->sound == NULL )
		return false;

	// Disable panning and distance
	int freechan = Audio::Instance().GetFreeChannel();
	Mix_SetDistance( freechan, 0 );
	Mix_SetPanning( freechan, 127, 127 );
	Mix_Volume( freechan, this->volume );
	this->channel = Audio::Instance().PlayChannel( freechan, this->sound, 0 );
	if ( channel == -1 )
		return false;
	
	return true;
}

/**\brief Plays the sound at a specified coordinate from origin.
 */
bool Sound::Play( Coordinate offset ){
	// Distance fading
	double dist = this->fadefactor * offset.GetMagnitude();
	if ( dist > 255 )
		return false;			// Sound is out of range
	Uint8 sounddist = static_cast<Uint8>( dist );

	// Left-Right panning
	float panx = this->panfactor * static_cast<float>(offset.GetX())+127.f;
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
	//else
	//	Log::Message("Distance set to %d on channel %d.", sounddist, freechan );

	/**\bug SDL_mixer bug possibly: Need to check whether SDL_mixer is getting
	 * Left/Right speaker switched around.
	 */
	if( Mix_SetPanning( freechan, 254 - soundpan, soundpan ) == 0 )
		Log::Error("Set panning %d failed on channel %d.", soundpan - 127, freechan );
	//else
	//	Log::Message("Panning set to %d on channel %d.", soundpan - 127, freechan );

	Mix_Volume( freechan, this->volume );
	this->channel = Audio::Instance().PlayChannel( freechan, this->sound, 0 );

	if ( channel == -1 )
		return false;
	
	return true;
}

/**\brief Plays the sound if not playing, but do not restart if already playing.
 * \details
 * This is sort of a roundabout way to implement engine sounds.
 */
bool Sound::PlayNoRestart( Coordinate offset ){
	if ( (this->channel != -1) &&
			Mix_Playing( this->channel ) &&
			(Mix_GetChunk( this->channel ) == this->sound ) )
		return false;

	this->Play( offset );
	return true;
}

/**\brief Sets the volume for this sound only (for next time it is played).
 */
bool Sound::SetVolume( float volume ){
	this->volume = static_cast<int>( volume * 128.f );
	return true;
}


/**\brief Sets distance fading and panning factor.
 * \param fade distance fading factor (defaults to 0.03)
 * \param pan Pan factor (defaults to 0.1)
 */
void Sound::SetFactors( double fade, float pan ){
	this->fadefactor = fade;
	this->panfactor = pan;
}
