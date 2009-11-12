/*
 * Filename      : starfield.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_starfield__
#define __h_starfield__

class Starfield {
	public:
		Starfield( int num );
		~Starfield( void );

		void Draw( void );
		void Update( void );
		void drawStar( float x, float y, float brightness);

	private:
		struct _stars {
			float x, y;
			float clr;
		} *stars;

		int num; // number of stars
};

#endif // __h_starfield__

