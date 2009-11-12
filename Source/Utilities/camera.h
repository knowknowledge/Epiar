/*
 * Filename      : camera.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_camera__
#define __h_camera__

#include "includes.h"
#include "Sprites/sprite.h"
#include "Utilities/coordinate.h"

class Camera {
	public:
		static Camera *Instance();

		void Focus( double x, double y );
		void Focus( Sprite *sprite );

		// takes world coordinates and translates them into drawing coords
		void Translate( Coordinate &world, Coordinate &screen );
		// gives the most recent change in camera coordinates
		void GetDelta( double *dx, double *dy );

		void Update( void );
	
		// moves the camera given a delta. this is not the common method to
		// move the camera, you should use Focus()
		void Move( int dx, int dy );
		void setZoom(float z){
			if(zoom+z > .68 && zoom+z < 1.22){
				zoom = zoom+z;
			}
		}
		float getZoom(){return zoom;}
		
		void Shake( Uint32 duration );
	        
	protected:
		Camera();
  		Camera( const Camera & );
  		Camera& operator= (const Camera&);

 	private:
  		static Camera *pInstance;
		Sprite *focusSprite; // focused on sprite - always favored, use NULL to set camera to static locations
  		double x, y; // point where camera is looking
  		double dx, dy; // the difference in the current and last camera position
  					// (this is used by Starfield)
		float zoom; // current zoom, zoom = 1. means no zooming
		bool hasZoomed;
};

#endif // __h_camera__
