/**\file			camera.cpp
 * \author			Chris Thielen (chris@epiar.net)
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
 * \brief The Camera designates where the user is currently looking.
 */

Camera *Camera::pInstance = 0; // initialize pointer

/**\fn GetFocus
 * \brief Get the sprite that is currently being watched by the camera.
 *
 * \fn setZoom
 * \brief Set the zoom factor
 * \todo Zooming is not currently enabled.
 *
 * \fn getZoom
 * \brief Get the zoom factor
 * \todo Zooming is not currently enabled.
 *
 */


/**\brief Fetch the current camera Instance
 */
Camera *Camera::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Camera; // create the sold instance
	}
	return( pInstance );
}

/**\brief Camera default constructor
 */
Camera::Camera( void ) {
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	focusID = 0;
	zoom = 1.;
	hasZoomed = true;
	cameraShakeDur = 0;
	cameraShakeXOffset = 0;
	cameraShakeYOffset = 0;
	cameraShakeXDec = 0;
	cameraShakeYDec = 0;
}

/**\brief Focus on a specific Location
 */
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

/**\brief Focus the camera on a Sprite
 */
void Camera::Focus( Sprite *sprite ) {
	if( sprite != NULL ) {
		focusID = sprite->GetID();
	} else {
		focusID = 0;
	}
}

/**\brief Get the current Camera Coordinate
 */
Coordinate Camera::GetFocusCoordinate() {
	return Coordinate(x,y);
}

/**\brief Translate a given World Coordinate into the relative Screen Coordinate
 * This should be used to answer questions like, "When a ship is located at
 * a given _world_ position, where should it be drawn on the screen?"
 *
 * \param[in] world Coordinate relative to the universe center.
 * \param[out] screen Coordinate relative to the upper-left of the screen.
 * \see TranslateScreenToWorld
 */
void Camera::TranslateWorldToScreen( Coordinate &world, Coordinate &screen ) {
	int tx, ty;

	tx = (int)(world.GetX() - x + Video::GetHalfWidth());
	ty = (int)(world.GetY() - y + Video::GetHalfHeight());
	
	screen.SetX( tx );
	screen.SetY( ty );
}

/**\brief Translate a given Screen Coordinate into the relative World Coordinate
 * This should be used to answer questions like, "When the user clicks the
 * _screen_, what is the corresponding _world_ position?"
 *
 * \param[in] screen Coordinate relative to the upper-left of the screen.
 * \param[out] world Coordinate relative to the universe center.
 * \see TranslateWorldToScreen
 */
void Camera::TranslateScreenToWorld( Coordinate &screen, Coordinate &world ) {
	int tx, ty;

	tx = (int)(screen.GetX() + x - Video::GetHalfWidth());
	ty = (int)(screen.GetY() + y - Video::GetHalfHeight());
	
	world.SetX( tx );
	world.SetY( ty );
}


/**\brief The current Camera position delta
 */
void Camera::GetDelta( double *dx, double *dy ) {
	if( !dx || !dy ) {

		LogMsg(WARN, "dx/dy are NULL pointers!" );

		return;
	}
	
	*dx = this->dx;
	*dy = this->dy;
}

/**\brief Moves the camera by a delta.
 * \note This is not the recommended way to move the camera.
 */
void Camera::Move( int dx, int dy ) {
	this->dx -= dx;
	this->dy -= dy;

	x -= dx;
	y -= dy;
}

/**\brief Updates camera, including currently focused position
 */
void Camera::Update( SpriteManager *sprites ) {
	Sprite* focusSprite;
	dx = 0;
	dy = 0;
	focusSprite = sprites->GetSpriteByID( focusID );
	if( focusSprite ) {
		Coordinate pos = focusSprite->GetWorldPosition();
		/*
		 * Turning off any Camera Lag
		//get player acceleration
		cameraLag += Player::Instance()->GetAcceleration();
		//use the inverase of the acceleration to reduce camer back to center
		cameraLag = cameraLag*.997;
		// until the camera lag is cleaned up and only applies during rapid accel, it's just going
		// to be turned off
		cameraLag = 0;
		*/
		Focus( pos.GetX() + cameraShakeXOffset - (cameraLag.GetX() * 10), 
		       pos.GetY() + cameraShakeYOffset - (cameraLag.GetY() * 10));

		UpdateShake();
	}
	
}

/**\brief "Shakes" the camera based on the duration, intensity, source specified
 */
void Camera::Shake( Uint32 duration, int intensity, Coordinate* source ) {
	Trig *trig = Trig::Instance();
	float angle;
	//Coordinate position = focusSprite->GetWorldPosition() - *source;
	Coordinate position = *source;
	angle = position.GetAngle();

	cameraShakeXOffset = (int)(intensity * trig->GetCos(angle));
	cameraShakeYOffset = (int)(intensity * trig->GetSin(angle));	

	cameraShakeDur = duration;

	if (cameraShakeDur != 0) {
		cameraShakeXDec = cameraShakeXOffset/(cameraShakeDur/10);
		cameraShakeYDec = cameraShakeYOffset/(cameraShakeDur/10);
	}
}

/**\brief "Shakes" the camera 
*/
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
