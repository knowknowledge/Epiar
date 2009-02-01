/*
 * Filename      : ship.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
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
		angle += (rotPerSecond * timerDelta) * 360.;
	} else {
		angle -= (rotPerSecond * timerDelta) * 360.;
	}
	
	// Normalize
	while( angle < 0. ) angle += 360.;
	while( angle > 360. ) angle -= 360.;
	
	SetAngle( angle );

	cout << "ship rotated, angle is " << angle << endl;
}

void Ship::Accelerate( void ) {
	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = trig->DegToRad( GetAngle() );

	momentum += Coordinate( trig->GetCos( angle ) * model->GetAcceleration() * Timer::GetDelta(),
	                         trig->GetSin( angle ) * model->GetAcceleration() * Timer::GetDelta() );
	
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
		
		trig->RotatePoint( (float)(position.GetScreenX() - model->GetThrustOffset()), (float)position.GetScreenY(), (float)position.GetScreenX(), (float)position.GetScreenY(), &tx, &ty, trig->DegToRad( direction ) );
		flareAnimation->Draw( (int)tx, (int)ty, direction );
		
		status.isAccelerating = false;
	}
}

// Returns the ship's integrity as a percentage (0.0-1.0, where 1.0 = 100%)
float Ship::getHullIntegrityPct() {
	assert( model );
	return( ( (float)model->getMaxEnergyAbsorption() - (float)status.hullEnergyAbsorbed ) / (float)model->getMaxEnergyAbsorption() );
}
