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

}

void Effect::Draw( void ) {
	bool finished;
	SpriteManager *sprites = SpriteManager::Instance();
	Coordinate pos = GetWorldPosition();
	finished = visual->Draw( pos.GetScreenX(), pos.GetScreenY(), 0.0);
	if(finished && !visual->GetLooping() )
		sprites->Delete( (Sprite*)this );
}
