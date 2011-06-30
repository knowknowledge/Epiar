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
		// If the sound couldn't be loaded, then abort
		if(value->sound == NULL )
		{
				delete value;
				return NULL;
		}
		else
		{
				Resource::Store( filename, (Resource*) value );
		}
	}
	return value;
}

/**\brief Loads the sound based on filename
 * \param filename Sound file
 */
Sound::Sound( const string& filename ):
	sound( NULL ),
	channel( -1 ),
	fadefactor( 0.03 ),
	panfactor( 0.1f ),
	volume( 128 )
{
	if( pathName.OpenRead( filename ) == false ) {
		LogMsg(ERR, "Could not load sound file: '%s'", filename.c_str() );
		sound = NULL;
		return;
	}

	this->sound = Mix_LoadWAV( pathName.GetAbsolutePath().c_str() );
	if( this->sound == NULL ) {
		LogMsg(ERR, "Could not load sound file: '%s', Mixer error: %s",
				filename.c_str(), Mix_GetError() );
		sound = NULL;
	}
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
	assert(this);
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
	assert(this);
	if ( this->sound == NULL )
		return false;

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
		LogMsg(ERR,"Set distance %d failed on channel %d.", sounddist, freechan );
	//else
	//	LogMsg(INFO,"Distance set to %d on channel %d.", sounddist, freechan );

	/**\bug SDL_mixer bug possibly: Need to check whether SDL_mixer is getting
	 * Left/Right speaker switched around.
	 */
	if( Mix_SetPanning( freechan, 254 - soundpan, soundpan ) == 0 )
		LogMsg(ERR,"Set panning %d failed on channel %d.", soundpan - 127, freechan );
	//else
	//	LogMsg(INFO,"Panning set to %d on channel %d.", soundpan - 127, freechan );

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
	assert(this);
	if ( this->sound == NULL )
		return false;

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
	assert(this);
	if ( this->sound == NULL )
		return false;

	this->volume = static_cast<int>( volume * 128.f );
	return true;
}


/**\brief Sets distance fading and panning factor.
 * \param fade distance fading factor (defaults to 0.03)
 * \param pan Pan factor (defaults to 0.1)
 */
void Sound::SetFactors( double fade, float pan ){
	assert(this);
	this->fadefactor = fade;
	this->panfactor = pan;
}
