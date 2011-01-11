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
#include "Engine/weapons.h"
#include "includes.h"
class Projectile :
	public Sprite
{
public:
	Projectile(float damageBooster, float angleToFire, Coordinate worldPosition, Coordinate firedMomentum, Weapon* weapon);
	~Projectile(void);
	void Update( void );
	void SetOwnerID(int id) { ownerID = id; }
	void SetTargetID(int id) { targetID = id; }
	int GetDrawOrder( void ) {
			return( DRAW_ORDER_WEAPON );
	}
private:
	Uint32 secondsOfLife; //time to live before projectile blows up
	Uint32 start;
	int ownerID;
	int targetID;
	float damageBoost;
	Weapon *weapon;
};
#endif // __h_PROJECTILE__
