/*
 * Filename      : timer.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Utilities/timer.h"

Timer *Timer::pInstance = 0; // initialize pointer
Uint32 Timer::lastLoopLength = 25;
Uint32 Timer::lastLoopTick = SDL_GetTicks();

Timer::Timer( void ) {
	lastLoopLength = 25;
	lastLoopTick = SDL_GetTicks();
}

Timer *Timer::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Timer; // create the sold instance
	}
	return( pInstance );
}

void Timer::Update( void ) {
	Uint32 now = SDL_GetTicks();
	
	lastLoopLength = now - lastLoopTick;
	
	lastLoopTick = now;
}

Uint32 Timer::GetTicks( void )
{
	return( lastLoopTick );
}

void Timer::Delay( void ) {
	SDL_Delay( 15 );
}

float Timer::GetDelta( void ) {
	return( (float)lastLoopLength / 1000. );
}

