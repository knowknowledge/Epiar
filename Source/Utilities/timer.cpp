/**\file			timer.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Utilities/timer.h"

/**\class Timer
 * \brief Timer class. */

Uint32 Timer::lastLoopLength = 25;
Uint32 Timer::lastLoopTick = SDL_GetTicks();
Uint32 Timer::ticksPerFrame = 0;
float Timer::logicFPS = LOGIC_FPS;
double Timer::virtualTime = 0;

void Timer::Initialize( void ) {
	lastLoopLength = 0;
	lastLoopTick = SDL_GetTicks();
	ticksPerFrame = 1000 / OPTION( Uint32, "options/video/fps" );
}

int Timer::Update( void ) {
	Uint32 tick = SDL_GetTicks();
	
	lastLoopLength = tick - lastLoopTick;
	lastLoopTick = tick;
	
	float dt = lastLoopLength * 0.001f;
	float frames = dt * Timer::logicFPS;
	
	int i = static_cast<int>(floor(virtualTime + frames) - floor(virtualTime));
	virtualTime += frames;
	
	return i;
}

Uint32 Timer::GetTicks( void )
{
	return( lastLoopTick );
}

void Timer::Delay( void ) {
//#ifdef EPIAR_CAP_FRAME
//	Uint32 ticksElapsed = SDL_GetTicks() - lastLoopTick;
// Require a definition to activate frame cap (so we can check performance)
//	if(ticksElapsed < ticksPerFrame)
//	{
//		Uint32 ticksToSleep = ticksPerFrame - ticksElapsed;
//		SDL_Delay(ticksToSleep);
//	}
//#else
	SDL_Delay(10);
//#endif
}

float Timer::GetDelta( void ) {
	return 1.f / Timer::logicFPS;
	//return( static_cast<float>(lastLoopLength / 1000. ));
}

