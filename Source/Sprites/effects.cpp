/**\file			effect.cpp
 * \author			Matt Zweig
 * \date			Created: Tuesday, December 15, 2009
 * \date			Modified: Tuesday, December 15, 2009
 * \brief			Sprite SubClass for Animated backgrounds
 * \details
 */

#include "includes.h"
#include "Graphics/animation.h"
#include "Graphics/image.h"
#include "Sprites/spritemanager.h"
#include "Sprites/sprite.h"
#include "Sprites/effects.h"

Effect::Effect(Coordinate pos, string filename, bool looping) {
	SetWorldPosition(pos);
	visual = new Animation(filename);
	visual->SetLooping(looping);
}

void Effect::Update( void ) {
	if( visual->Update() == true ) {
		SpriteManager::Instance()->Delete( (Sprite*)this );
	}
}

void Effect::Draw( void ) {
	Coordinate pos = GetWorldPosition();
	visual->Draw( pos.GetScreenX(), pos.GetScreenY(), 0.0);
}
