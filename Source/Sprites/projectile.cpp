/*
 * Filename      : projectile.cpp
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : Projectile class, child of sprite class, Sprite
 * Notes         :
 */

#include "includes.h"
#include "Sprites/projectile.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Sprites/ship.h"
#include "Utilities/timer.h"
#include "Engine/weapons.h"

Projectile::Projectile(float angleToFire, Coordinate worldPosition, Coordinate firedMomentum, Weapon* weapon)
{
	// All Projectiles get these
	ownerID = -1;
	start = Timer::GetTicks();
	SetRadarColor (Color::Get(0x55,0x55,0x55));

	// These are based off of the Ship firing this projectile
	SetWorldPosition( worldPosition );
	SetAngle(angleToFire);

	// These are based off of the Weapon
	secondsOfLife = weapon->GetLifetime();
	SetImage(weapon->GetImage());

	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = static_cast<float>(trig->DegToRad( angleToFire ));

	momentum = firedMomentum +
	           Coordinate( trig->GetCos( angle ) * weapon->GetVelocity(),
	                      -trig->GetSin( angle ) * weapon->GetVelocity() );
	
	SetMomentum( momentum );
}

Projectile::~Projectile(void)
{
}

void Projectile::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	SpriteManager *sprites = SpriteManager::Instance();
	int numImpacts = 0;
	
	list<Sprite*> *impacts = sprites->GetSpritesNear( this->GetWorldPosition(), 50 );
	if( impacts->size() > 1) {
		list<Sprite *>::iterator i;
		for( i = impacts->begin(); i != impacts->end(); ++i ) {
			if( ( ( (*i)->GetDrawOrder() == DRAW_ORDER_SHIP )
			    ||( (*i)->GetDrawOrder() == DRAW_ORDER_PLAYER ) )
			  &&( (*i)->GetID() != ownerID )
			 ) {
				((Ship*)(*i))->Damage( 20 );
				numImpacts++;
			}
		}
	}
	if (numImpacts || ( Timer::GetTicks() > secondsOfLife + start )) {
		//if(numImpacts ) cout<<"Projectile Hit "<<numImpacts<<" Ships!\n";
		sprites->Delete( (Sprite*)this );
	}
}

