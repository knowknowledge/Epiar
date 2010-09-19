/**\file			animation.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
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
		Animation( string filename );
		bool Update( void );
		void Draw( int x, int y, float ang );
		void SetLoopPercent( float loopPercent );
		bool GetLoopPercent( void ) { return loopPercent; };
		void Reset( void );
		int GetHalfWidth( void ) { return ani->w / 2; };
		int GetHalfHeight( void ) { return ani->h / 2; };

	private:
		Ani *ani;
		Uint32 startTime;
		float loopPercent;
		int fnum;
};

#endif // __h_animation__

