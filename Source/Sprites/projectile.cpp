/**\file			projectile.cpp
 * \author			Shawn Reynolds (eb0s@yahoo.com)
 * \date			Created: Friday, November 21, 2009
 * \date			Modified: Friday, November 21, 2009
 * \brief			Projectile class, child of sprite class, Sprite
 * \details
 */

#include "includes.h"
#include "Sprites/projectile.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Sprites/ship.h"
#include "Sprites/effects.h"
#include "Utilities/timer.h"
#include "Engine/weapons.h"

Projectile::Projectile(float angleToFire, Coordinate worldPosition, Coordinate firedMomentum, Weapon* _weapon)
{
	// All Projectiles get these
	ownerID = 0;
	targetID = 0;
	start = Timer::GetTicks();
	SetRadarColor (Color::Get(0x55,0x55,0x55));

	// These are based off of the Ship firing this projectile
	SetWorldPosition( worldPosition );
	SetAngle(angleToFire);

	// These are based off of the Weapon
	weapon = _weapon;
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

	// Check for projectile collisions
	Sprite* impact = sprites->GetNearestSprite( (Sprite*)this, 100,DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER );
	if( (impact != NULL) && (impact->GetID() != ownerID) && ((this->GetWorldPosition() - impact->GetWorldPosition()).GetMagnitude() < impact->GetRadarSize() )) {
		((Ship*)impact)->Damage( weapon->GetPayload() );
		sprites->Delete( (Sprite*)this );
		
		// Create a fire burst where this projectile hit the ship's shields.
		// TODO: This shows how much we need to improve our collision detection.
		Effect* hit = new Effect(this->GetWorldPosition(), "Resources/Animations/shield.ani", 0);
		hit->SetAngle( -this->GetAngle() );
		hit->SetMomentum( impact->GetMomentum() );
		sprites->Add( hit );
	}

	// Expire the projectile after a time period
	if (( Timer::GetTicks() > secondsOfLife + start )) {
		sprites->Delete( (Sprite*)this );
	}

	// Track the target
	Sprite* target = sprites->GetSpriteByID( targetID );
	float tracking = weapon->GetTracking();
	if( target != NULL && tracking > 0.01f ) {
		float angleTowards = normalizeAngle( ( target->GetWorldPosition() - this->GetWorldPosition() ).GetAngle() - GetAngle() );
		SetMomentum( GetMomentum().RotateBy( angleTowards*tracking ) );
		SetAngle( GetMomentum().GetAngle() );
	}

}

