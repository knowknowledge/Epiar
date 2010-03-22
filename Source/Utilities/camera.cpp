/**\file			camera.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/video.h"
#include "Utilities/camera.h"
#include "Utilities/log.h"
#include "Utilities/trig.h"
#include "Sprites/player.h"
#include "Utilities/coordinate.h"

/**\class Camera
 * \brief Camera handling. */

Camera *Camera::pInstance = 0; // initialize pointer

Camera *Camera::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Camera; // create the sold instance
	}
	return( pInstance );
}

Camera::Camera( void ) {
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	focusSprite = NULL;
	zoom = 1.;
	hasZoomed = true;
	cameraShakeDur = 0;
	cameraShakeXOffset = 0;
	cameraShakeYOffset = 0;
	cameraShakeXDec = 0;
	cameraShakeYDec = 0;
}

void Camera::Focus( double x, double y ) {
	// calculate the difference (this is used in many conversions)
	dx = x - this->x;
	dy = y - this->y;

	this->x = x;
	this->y = y;
	// assign the new camera position
	//if(hasZoomed){
		//this->x = x;
		//this->y = y;
		//hasZoomed=true;
	//} else {
		//this->x = x;
		//this->y = y;
		//hasZoomed=false;
	//}
}

void Camera::Focus( Sprite *sprite ) {
	focusSprite = sprite;
}

Coordinate Camera::GetFocusCoordinate() {
	if(focusSprite) {
		return focusSprite->GetWorldPosition();
	} else {
		return Coordinate(x,y);
	}
}

// Converts world to screen coordinates
void Camera::TranslateWorldToScreen( Coordinate &world, Coordinate &screen ) {
	int tx, ty;

	tx = (int)(world.GetX() - x + Video::GetHalfWidth());
	ty = (int)(world.GetY() - y + Video::GetHalfHeight());
	
	screen.SetX( tx );
	screen.SetY( ty );
}

void Camera::TranslateScreenToWorld( Coordinate &screen, Coordinate &world ) {
	int tx, ty;

	tx = (int)(screen.GetX() + x - Video::GetHalfWidth());
	ty = (int)(screen.GetY() + y - Video::GetHalfHeight());
	
	world.SetX( tx );
	world.SetY( ty );
}


// returns the most recent camera position change
void Camera::GetDelta( double *dx, double *dy ) {
	if( !dx || !dy ) {

		Log::Warning( "dx/dy are NULL pointers!" );

		return;
	}
	
	*dx = this->dx;
	*dy = this->dy;
}

// moves the camera by a delta. this is not the recommended way to move the camera
void Camera::Move( int dx, int dy ) {
	this->dx = -dx;
	this->dy = -dy;

	x -= dx;
	y -= dy;
}

// updates camera, including currently focused position
void Camera::Update( void ) {
	dx = 0;
	dy = 0;
	if( focusSprite ) {
		Coordinate pos = focusSprite->GetWorldPosition();
		//get player acceleration
		cameraLag += Player::Instance()->GetAcceleration();
		//use the inverase of the acceleration to reduce camer back to center
		Coordinate cameraCatchup = Coordinate( -cameraLag.GetX()*0.003,-cameraLag.GetY()*0.003 );
		cameraLag += cameraCatchup;
		// until the camera lag is cleaned up and only applies during rapid accel, it's just going
		// to be turned off
		cameraLag = 0;
		Focus( pos.GetX() + cameraShakeXOffset - (cameraLag.GetX() * 10), 
			pos.GetY() + cameraShakeYOffset - (cameraLag.GetY() * 10));

		UpdateShake();
	}
	
}
// "Shakes" the camera based on the duration, intensity, source specified
void Camera::Shake( Uint32 duration, int intensity, Coordinate* source ) {
	Trig *trig = Trig::Instance();
	float angle;
	Coordinate position = focusSprite->GetWorldPosition() - *source;
	angle = position.GetAngle();

	cameraShakeXOffset = (int)(intensity * trig->GetCos(angle));
	cameraShakeYOffset = (int)(intensity * trig->GetSin(angle));	

	cameraShakeDur = duration;

	if (cameraShakeDur != 0) {
		cameraShakeXDec = cameraShakeXOffset/(cameraShakeDur/10);
		cameraShakeYDec = cameraShakeYOffset/(cameraShakeDur/10);
	}

}
// "Shakes" the camera 
//Note: Shakes the camera 
void Camera::UpdateShake() {
	if (cameraShakeDur < 1) {
		cameraShakeXOffset = 0;
		cameraShakeYOffset = 0;
		return;
	}

	if (cameraShakeDur % 10 == 0) {
		if (cameraShakeXOffset > 0) {
			cameraShakeXOffset -= cameraShakeXDec;
			cameraShakeYOffset -= cameraShakeYDec;
		} else {
			cameraShakeXOffset += cameraShakeXDec;
			cameraShakeYOffset += cameraShakeYDec;
		}
		cameraShakeXOffset *= -1;
		cameraShakeYOffset *= -1;
	} 
	cameraShakeDur--;
}
