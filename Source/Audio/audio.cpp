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
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  int audio_channels = 2;
  int audio_buffers = 4096;

  SDL_Init(SDL_INIT_AUDIO);

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)){
	  Log::Error("Audio initialization failed!");
	  return false;
  }

  // Load MOD and OGG libraries
  Mix_Init( MIX_INIT_MOD | MIX_INIT_OGG );
  return true;
}

/**\brief Audio system shutdown.
 */
bool Audio::Shutdown( void ){
  /* This is the cleaning up part */
  Mix_CloseAudio();
  return true;
}

/**\brief Set's the music volume (Range from 0 - 128 ).
 */
bool Audio::SetMusicVol( int volume ){
	Mix_VolumeMusic( volume );
}

/**\brief Empty constructor
 */
Audio::Audio(){
}

/**\brief Empty destructor
 */
Audio::~Audio(){
}
