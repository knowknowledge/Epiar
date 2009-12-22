/**\file			timer.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_timer__
#define __h_timer__

#include "includes.h"

class Timer {
	public:
		static Timer *Instance();
		static void Update( void );
		static void Delay( void );
		static Uint32 GetTicks( void );
		
		static float GetDelta( void );
	
	protected:
		Timer();
  		Timer( const Timer & );
  		Timer& operator= (const Timer&);
  	
  	private:
  		static Timer *pInstance;
  		static Uint32 lastLoopLength;
  		static Uint32 lastLoopTick;
		static Uint32 ticksPerFrame;
};

#endif // __h_timer__
