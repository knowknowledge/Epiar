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

#include "Engine/models.h"
#include "Sprites/sprite.h"
class Projectile :
	public Sprite
{
public:
	Projectile(float angleToFire, Coordinate worldPosition, string modelName, int lifetime, int velocity);
	~Projectile(void);
	bool SetModel( Model *model );
	bool SetFlareAnimation( string filename );
	Sprite *GetSprite();
	string GetModelName();
	void Update( void );
	void Draw( void );
	int GetDrawOrder( void ) {
			return( 2 );
	}
private:
	int ttl; //time to live before projectile blows up
	Model *model;
	Animation *flareAnimation;
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