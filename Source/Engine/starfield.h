/**\file			starfield.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified : Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "Utilities/camera.h"

#ifndef __h_starfield__
#define __h_starfield__

class Starfield {
	public:
		Starfield( int num );
		~Starfield( void );

		void Draw( void );
		void Update( Camera *camera );
		void drawStar( float x, float y, float brightness);

	private:
		struct _stars {
			float x, y;
			float clr;
		} *stars;

		int num; // number of stars
};

#endif // __h_starfield__

