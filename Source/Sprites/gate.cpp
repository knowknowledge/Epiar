/**\file			effect.cpp
 * \author			Matt Zweig
 * \date			Created: Tuesday, December 15, 2009
 * \date			Modified: Tuesday, December 15, 2009
 * \brief			Sprite SubClass for Animated backgrounds
 * \details
 */

#include "includes.h"
#include "Graphics/image.h"
#include "Sprites/spritemanager.h"
#include "Sprites/sprite.h"
#include "Sprites/gate.h"


Gate::Gate(Coordinate pos) {
	SetWorldPosition(pos);
	top = true;
	SetImage( Image::Get("Resources/Graphics/gate1_top.png") );
	SetAngle( float( rand() %360 ) );
	
	// Create
	Gate* bottom = new Gate(pos, this->GetID());
	bottom->SetAngle( this->GetAngle() );
	partner = bottom->GetID();
	SpriteManager::Instance()->Add((Sprite*)bottom);
}

Gate::Gate(Coordinate pos, int topID) {
	SetWorldPosition(pos);
	top = false;
	SetImage( Image::Get("Resources/Graphics/gate1_bottom.png") );
	partner = topID;
}

Gate::~Gate(){
	if(top){
		Sprite* bottom = SpriteManager::Instance()->GetSpriteByID(partner);
		if(bottom==NULL){
			SpriteManager::Instance()->Delete( bottom );
		}
	}
}
