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

Ship::Ship() {
	model = NULL;
	flareAnimation = NULL;
	
	/* Initalize ship's condition */
	status.hullEnergyAbsorbed = 0;
	SetRadarColor(Color::Get(255,0,0));
	SetAngle( float( rand() %360 ) );
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

void Ship::Rotate( int direction ) {
	float rotPerSecond, timerDelta;
	float angle = GetAngle();
	
	if( !model ) {
		Log::Warning( "Attempt to rotate sprite with no model." );
		
		return;
	}
	
	rotPerSecond = model->GetRotationsPerSecond();
	timerDelta = Timer::GetDelta();
	
	if( direction == _LEFT ) {
		angle += static_cast<float>((rotPerSecond * timerDelta) * 360.);
	}
    if( direction == _RIGHT){
		angle -= static_cast<float>((rotPerSecond * timerDelta) * 360.);
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

void Ship::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	
	if( status.isAccelerating == false ) {
		flareAnimation->Reset();
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

Direction Ship::directionTowards(Coordinate target){
	float theta;
	//Trig *trig = Trig::Instance();
	Coordinate position = target - GetWorldPosition();

	theta = position.GetAngle();//trig->RadToDeg(atan2( - position.GetY(), position.GetX()));
	//Log::Message("Angle towards target (%f,%f) is %f.",target.GetX(),target.GetY(),theta);
	//Log::Message("Current Angle %f",this->GetAngle());
	return this->directionTowards(theta);
}

Direction Ship::directionTowards(float angle){
	float aim;
	// aim must be of the correct domain (0,360)
	angle = normalizeAngle(angle);
	aim = angle - this->GetAngle();
	aim = normalizeAngle(aim);
	
	//Log::Message("Aim %f",aim);
	// return the correct direction
	if	  ( aim < 180 ) return _LEFT ;
	else if ( aim > 180 ) return _RIGHT; 
	return _STRAIGHT;
}

// Returns the ship's integrity as a percentage (0.0-1.0, where 1.0 = 100%)
float Ship::getHullIntegrityPct() {
	assert( model );
	return( ( (float)model->getMaxEnergyAbsorption() - (float)status.hullEnergyAbsorbed ) / (float)model->getMaxEnergyAbsorption() );
}
