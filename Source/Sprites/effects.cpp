/*
 * Filename      : effect.cpp
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, December 15, 2009
 * Last Modified : Tuesday, December 15, 2009
 * Purpose       : Sprite SubClass for Animated backgrounds
 * Notes         :
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
