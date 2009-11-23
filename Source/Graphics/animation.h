/**\file			animation.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
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

