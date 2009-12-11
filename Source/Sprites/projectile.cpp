/*
 * Filename      : projectile.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : Projectile class, child of sprite class, Sprite
 * Notes         :
 */
#include "Sprites/projectile.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"

Projectile::Projectile(float angleToFire, Coordinate worldPosition, Image* img, int lifetime, int velocity)
{
	direction = angleToFire;
	ttl = lifetime;
	this->velocity = velocity;
	isAccelerating = true;

	SetWorldPosition( worldPosition );
	SetAngle(angleToFire);
	SetRadarColor (Color::Get(255,0,0));

	SetImage(img);

	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = static_cast<float>(trig->DegToRad( angleToFire ));

	momentum = Coordinate( trig->GetCos( angle ) * velocity,
	                         -1 * trig->GetSin( angle ) * velocity );
	
	SetMomentum( momentum );

}

Projectile::~Projectile(void)
{
}

void Projectile::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	ttl--;
	
	if (ttl < 1){
		SpriteManager *sprites = SpriteManager::Instance();
		sprites->Delete( (Sprite*)this );
		
	}
}

