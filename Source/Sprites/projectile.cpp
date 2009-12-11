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
#include "Sprites/ship.h"

Projectile::Projectile(float angleToFire, Coordinate worldPosition, Image* img, int lifetime, int velocity)
{
	direction = angleToFire;
	ttl = lifetime;
	this->velocity = velocity;
	isAccelerating = true;

	SetWorldPosition( worldPosition );
	SetAngle(angleToFire);
	SetRadarColor (Color::Get(0x99,0x99,0x99));

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
	SpriteManager *sprites = SpriteManager::Instance();
	int numImpacts = 0;
	
	list<Sprite*> *impacts = sprites->GetSpritesNear( this->GetWorldPosition(), 50 );
	if( impacts->size() > 1) {
		list<Sprite *>::iterator i;
		for( i = impacts->begin(); i != impacts->end(); ++i ) {
			if( ( (*i)->GetDrawOrder() == DRAW_ORDER_SHIP )
//			 || ( (*i)->GetDrawOrder() == DRAW_ORDER_PLAYER )
			 ) {
				((Ship*)(*i))->Damage( 200 );
				numImpacts++;
			}
		}
	}
	if (numImpacts || (ttl < 1)) {
		if(numImpacts ) cout<<"Projectile Hit "<<numImpacts<<" Ships!\n";
		sprites->Delete( (Sprite*)this );
	}
}

