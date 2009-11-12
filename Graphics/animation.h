/*
 * Filename      : animation.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_animation__
#define __h_animation__

#include "Graphics/image.h"
#include "includes.h"

class Animation {
	public:
		Animation();
		Animation( string& filename );
		bool Load( string& filename );
		bool Draw( int x, int y, float ang );
		void SetLooping( bool looping ) { this->looping = looping; };
		void Reset( void );
		int GetHalfWidth( void ) { return w / 2; };
		int GetHalfHeight( void ) { return h / 2; };

	private:
		Image *frames;
		int numFrames;
		Uint32 startTime;
		Uint32 delay;
		bool looping;
		int w, h;
};

#endif // __h_animation__

