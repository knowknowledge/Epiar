/*
 * Filename      : ship.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Friday, November 14, 2009
 * Purpose       : 
 * Notes         :
 */

#include "Sprites/ship.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"
#include "Engine/weapons.h"
#include "Engine/weapon.h"
#include "Sprites/spritemanager.h"

Ship::Ship() {
	model = NULL;
	flareAnimation = NULL;
	
	/* Initalize ship's condition */
	status.hullEnergyAbsorbed = 0;
	SetRadarColor(Color::Get(255,0,0));
	SetAngle( float( rand() %360 ) );
	projectileAmmo = 10;
	fireDelay = 0;
	/*Debug: Add all weapons to this ships weapons list, later we will only basic and when they are purchasedhi*/
	Weapons* weapons = Weapons::Instance();
	//need to copy weapons instead of modifying originals, this is temp
	for( list<Weapon *>::iterator i = weapons->weapons.begin(); i != weapons->weapons.end(); ++i ) {
		shipWeapons.push_back(*i);
		(*i)->setAmmo(1000);
	}
	selectedWeapon = 0;
}

bool Ship::SetModel( Model *model ) {
	if( model ) {
		this->model = model;
		
		SetImage( model->GetImage() );
		SetFlareAnimation( model->GetFlareAnimation() );
		
		return( true );
	}
	
	return( false );
}

Sprite *Ship::GetSprite() {
	return( (Sprite *)this );
}

string Ship::GetModelName() {
	if (model){
		return model->GetName();
	} else {
		return "";
	}
}

void Ship::Rotate( float direction ) {
	float rotPerSecond, timerDelta, maxturning;
	float angle = GetAngle();
	
	if( !model ) {
		Log::Warning( "Attempt to rotate sprite with no model." );
		return;
	}

	// Compute the maximum amount that the ship can turn
	rotPerSecond = model->GetRotationsPerSecond();
	timerDelta = Timer::GetDelta();
	maxturning = static_cast<float>((rotPerSecond * timerDelta) * 360.);

	// Cap the ship rotation
	if (fabs(direction) > maxturning){ 
		if (direction > 0 )
			angle += maxturning;
		else
			angle -= maxturning;
	} else {
		angle += direction;
	}
	
	// Normalize
    angle = normalizeAngle(angle);
	
	SetAngle( angle );
}

void Ship::Accelerate( void ) {
	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = static_cast<float>(trig->DegToRad( GetAngle() ));

	momentum += Coordinate( trig->GetCos( angle ) * model->GetAcceleration() * Timer::GetDelta(),
	                         -1 * trig->GetSin( angle ) * model->GetAcceleration() * Timer::GetDelta() );
	
	SetMomentum( momentum );
	
	status.isAccelerating = true;
}


void Ship::Damage(short int damage) {
	status.hullEnergyAbsorbed += damage;
}

void Ship::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	if (fireDelay > 0) {
		fireDelay--;
	}
	
	if( status.isAccelerating == false ) {
		flareAnimation->Reset();
	}

	// Shiw the hits taken as part of the radar color
	SetRadarColor(Color::Get(int(255 *getHullIntegrityPct()),0 ,0));
	
	// Ship has taken as much damage as possible...
	// It Explodes!
	if( status.hullEnergyAbsorbed >=  (float)model->getMaxEnergyAbsorption() ) {
		SpriteManager *sprites = SpriteManager::Instance();
		sprites->Delete( (Sprite*)this );
	}
}

bool Ship::SetFlareAnimation( string filename ) {
	if( flareAnimation )
		delete flareAnimation;
		
	flareAnimation = new Animation( filename );
	
	return true;
}

void Ship::Draw( void ) {
	Trig *trig = Trig::Instance();
	
	Sprite::Draw();
	
	// Draw the flare animation, if required
	if( status.isAccelerating ) {
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
		
		status.isAccelerating = false;
	}
}

void Ship::Fire() {
	Weapon* currentWeapon = shipWeapons.at(selectedWeapon);
	if (fireDelay < 1 && !shipWeapons.empty()) {
		currentWeapon->fireWeapon(GetAngle(), GetWorldPosition());
		fireDelay = currentWeapon->getFireDelay();
	}
}

void Ship::ChangeWeapon() {
	if (selectedWeapon < shipWeapons.size()-1) {
		selectedWeapon++;
		return;
	} 
	selectedWeapon = 0;
}


float Ship::directionTowards(Coordinate target){
	float theta;
	//Trig *trig = Trig::Instance();
	Coordinate position = target - GetWorldPosition();

	theta = position.GetAngle();//trig->RadToDeg(atan2( - position.GetY(), position.GetX()));
	//Log::Message("Angle towards target (%f,%f) is %f.",target.GetX(),target.GetY(),theta);
	//Log::Message("Current Angle %f",this->GetAngle());
	return this->directionTowards(theta);
}

// Returns the best direction to turn in order to aim in a certain direction
float Ship::directionTowards(float angle){
	return normalizeAngle(angle - this->GetAngle());
}

// Returns the ship's integrity as a percentage (0.0-1.0, where 1.0 = 100%)
float Ship::getHullIntegrityPct() {
	assert( model );
	float remaining =  ( (float)model->getMaxEnergyAbsorption() - (float)status.hullEnergyAbsorbed ) / (float)model->getMaxEnergyAbsorption();
	//Log::Message("Ship has taken %d damage out of %d possibile. %02f%% Remaining",status.hullEnergyAbsorbed,model->getMaxEnergyAbsorption(),remaining);
	return(remaining);
}
