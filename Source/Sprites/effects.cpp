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

/**\class Effect
 * \brief Various Animation effects.
 */

/**\brief Creates a new Effect at specified coordinate with Animation file
 */
Effect::Effect(Coordinate pos, string filename, bool looping) {
	SetWorldPosition(pos);
	visual = new Animation(filename);
	visual->SetLooping(looping);
}

/**\brief Updates the Effect
 */
void Effect::Update( void ) {
	Sprite::Update();
	if( visual->Update() == true ) {
		SpriteManager::Instance()->Delete( (Sprite*)this );
	}
}

/**\brief Draws the Effect
 */
void Effect::Draw( void ) {
	Coordinate pos = GetWorldPosition();
	visual->Draw( pos.GetScreenX(), pos.GetScreenY(), this->GetAngle());
}

/**\fn Effect::GetDrawOrder( )
 *  \brief Returns the Draw order of the Effect
 */
