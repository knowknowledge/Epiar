/**\file			audio.cpp
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Abstraction to SDL_mixer interface.
 * \details
 * This files is responsible for overall Audio system configuration.  To play
 * music or sound see the corresponding Music and Sound classes.
 */

#include "includes.h"
#include "Audio/audio.h"
#include "Utilities/log.h"

/**\class Audio
 * \brief This class is responsible for overall Audio system configuration.
 * \details
 * The Audio instance is implemented as a singleton.
 * \sa Sound
 * \sa Music
 */

/**\brief Creates or retrieves the current instance of the Audio device.
 */
Audio& Audio::Instance(){
	static Audio _instance;
	return _instance;
}

/**\brief Audio system initialization.
 */
bool Audio::Initialize( void ){
	SDL_Init(SDL_INIT_AUDIO);

	if(Mix_OpenAudio(this->audio_rate,
				this->audio_format,
				this->audio_channels,
				this->audio_buffers)){
		Log::Error("Audio initialization failed!");
		return false;
	}

	// Load MOD and OGG libraries
	Mix_Init( MIX_INIT_MOD | MIX_INIT_OGG );

	// Allocate channels
	Mix_AllocateChannels( this->max_chan);
	assert( this->max_chan == Mix_AllocateChannels( -1 ) );

	return true;
}

/**\brief Audio system shutdown.
 */
bool Audio::Shutdown( void ){
	/* This is the cleaning up part */
	Mix_HaltChannel( -1 );			// Halts all channels
	Mix_CloseAudio();
	Mix_Quit();
	return true;
}

/**\brief Set's the music volume (Range from 0 - 128 ).
 */
bool Audio::SetMusicVol( int volume ){
	Mix_VolumeMusic( volume );
	return true;
}

/**\brief Retrieves the first available channel.
 */
const int Audio::GetFreeChannel( void ){
	/**\todo Optimization: We could consider dynamically allocating.*/
	unsigned int numchan = Mix_AllocateChannels( -1  );
	assert( numchan == this->max_chan );
	for ( unsigned int i = 0; i < numchan; i++ ){
		if ( Mix_Playing( i ) == 0 )
			return i;
	}

	// No channels available, halt oldest used one
	Mix_HaltChannel( this->lastplayed.front() );

	return this->lastplayed.front();
}

/**\brief Wrapper for Mix_PlayChannel
 */
const int Audio::PlayChannel( int chan, Mix_Chunk *chunk, int loop ){
	int chan_used;			// Channel that was used to play a sound

	chan_used = Mix_PlayChannel( chan, chunk, loop );

	/**\todo This could be optimized.*/
	this->lastplayed.push_back( chan_used );
	// Check if queue is full
	if ( this->lastplayed.size() > this->max_chan ){
		this->lastplayed.pop_front( );
		assert( lastplayed.size() <= this->max_chan );
	}
	return chan_used;
}

/**\brief Empty constructor (use initialization lists to initialize privates.
 */
Audio::Audio(): 
	audio_rate( 22050 ),
	audio_format( AUDIO_S16 ),
	audio_channels( 2 ),
	audio_buffers( 4096 ),
	max_chan( 16 )
{
}

/**\brief Empty destructor
 */
Audio::~Audio(){
}
