/*
 * Filename      : sprite.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "common.h"
#include "Sprites/sprite.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

int Sprite::sprite_ids = 0;

Sprite::Sprite() {
	id = sprite_ids++;

	// Momentum caps
	float top = OPTION( float, "options/momentum-caps/top" );
	float right = OPTION( float, "options/momentum-caps/right" );
	float bottom = OPTION( float, "options/momentum-caps/bottom" );
	float left = OPTION( float, "options/momentum-caps/left" );

	angle = 0.;
	momentum.SetBoundaries( top, right, bottom, left ); // game defaults
	
	image = NULL;
	
	radarSize = 1;
	radarColor = Color::Get(0.7f,0.7f,0.7f);
}

Coordinate Sprite::GetWorldPosition( void ) {
	return worldPosition;
}

void Sprite::SetWorldPosition( Coordinate coord ) {
	worldPosition = coord;
}

void Sprite::Update( void ) {
	// Apply their momentum to change their coordinates
	worldPosition += momentum;
	//update acceleration
	acceleration = lastMomentum - momentum; 
	lastMomentum = momentum;
}

// sprites are drawn centered on wx,wy by design of Image
void Sprite::Draw( void ) {
	int wx, wy;

	wx = worldPosition.GetScreenX();
	wy = worldPosition.GetScreenY();
	
	if( image ) {
		image->DrawCentered( wx, wy, angle );
	} else {
		Log::Warning( "Attempt to draw a sprite before an image was assigned." );
	}
}

bool compareSpritePtrs(Sprite* a, Sprite* b){
	if(a->GetDrawOrder() != b->GetDrawOrder()) {
		return a->GetDrawOrder() < b->GetDrawOrder();
	} else {
		return a->GetID() < b->GetID();
	}
}

