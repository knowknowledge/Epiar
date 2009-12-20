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
#include "Utilities/resource.h"
#include "includes.h"

class Ani: public Resource {
	public:
		Ani();
		Ani( string& filename );
		bool Load( string& filename );
		static Ani* Get(string filename);
		Image *frames;
		int numFrames;
		Uint32 delay;
		int w, h;
	private:
};

class Animation {
	public:
		Animation();
		Animation( string& filename );
		bool Draw( int x, int y, float ang );
		void SetLooping( bool looping ) { this->looping = looping; };
		bool GetLooping( void ) { return looping; };
		void Reset( void );
		int GetHalfWidth( void ) { return ani->w / 2; };
		int GetHalfHeight( void ) { return ani->h / 2; };

	private:
		Ani *ani;
		Uint32 startTime;
		bool looping;
};

#endif // __h_animation__

