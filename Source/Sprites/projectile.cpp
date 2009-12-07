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
#include "Engine/models.h"
#include "Sprites/spritemanager.h"

Projectile::Projectile(float angleToFire, Coordinate worldPosition, string modelName, int lifetime, int velocity)
{
	model = NULL;
	flareAnimation = NULL;
	direction = angleToFire;
	ttl = lifetime;
	this->velocity = velocity;
	isAccelerating = true;

	SetWorldPosition( worldPosition );
	SetAngle(angleToFire);
	SetRadarColor (Color::Get(255,0,0));

	Models *models = Models::Instance();
	this->SetModel( models->GetModel( modelName ) );

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
bool Projectile::SetModel( Model *model ) {

	if( model ) {
		this->model = model;

		SetImage( model->GetImage() );
		SetFlareAnimation( model->GetFlareAnimation() );

		return( true );
	}
	
	return( false );
}

Sprite *Projectile::GetSprite() {

	return( (Sprite *)this );
}


string Projectile::GetModelName() {

	if (model){
		return model->GetName();
	} else {
		return "";
	}
}
void Projectile::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	ttl--;
	
	if (ttl < 1){
		SpriteManager *sprites = SpriteManager::Instance();
		sprites->Delete( this->GetSprite() );
		
	}
	/*if( status.isAccelerating == false ) {
		flareAnimation->Reset();
	}*/
}
void Projectile::Draw( void ) {
	Trig *trig = Trig::Instance();
	
	Sprite::Draw();
	
	// Draw the flare animation, if required
	//if( isAccelerating ) {
		Coordinate position = GetWorldPosition();
		float direction = GetAngle();
		float tx, ty;
		
		trig->RotatePoint( static_cast<float>((position.GetScreenX() -
						model->GetThrustOffset())),
				static_cast<float>(position.GetScreenY()),
				static_cast<float>(position.GetScreenX()),
				static_cast<float>(position.GetScreenY()), &tx, &ty,
				static_cast<float>( trig->DegToRad( direction ) ));
		flareAnimation->Draw( (int)tx, (int)ty, direction );
		
		//isAccelerating = false;
	//}
}

bool Projectile::SetFlareAnimation( string filename ) {
	if( flareAnimation )
		delete flareAnimation;
		
	flareAnimation = new Animation( filename );
	
	return true;
}