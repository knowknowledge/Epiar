/*
 * Filename      : projectile.h
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : Header for Projectile class, child of sprite class, Sprite
 * Notes         :
 */

#ifndef __H_PROJECTILE__
#define __H_PROJECTILE__

#include "Sprites/sprite.h"
class Projectile :
	public Sprite
{
public:
	Projectile(float angleToFire, Coordinate worldPosition, Image* img, int lifetime, int velocity);
	~Projectile(void);
	void Update( void );
	int GetDrawOrder( void ) {
			return( DRAW_ORDER_WEAPON );
	}
private:
	int ttl; //time to live before projectile blows up
	float direction;
	int velocity;
	bool isAccelerating; 
	/*todo Add 
 -graphics:
	 firing graphic
	 travel graphic
	 explode graphic
*/
};
#endif // __h_PROJECTILE__
