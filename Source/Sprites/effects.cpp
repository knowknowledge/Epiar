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
#include "Engine/simulation_lua.h"

/**\class Effect
 * \brief Various Animation effects.
 */

/**\brief Creates a new Effect at specified coordinate with Animation file
 */
Effect::Effect(Coordinate pos, string filename, float loopPercent) {
	SetWorldPosition(pos);
	visual = new Animation(filename);
	visual->SetLoopPercent( loopPercent );
}

/**\brief Destroy an Effect
 */
Effect::~Effect() {
	delete visual;
}

/**\brief Updates the Effect
 */
void Effect::Update( lua_State *L ) {
	Sprite::Update( L );
	if( visual->Update() == true ) {
		SpriteManager *sprites = Simulation_Lua::GetSimulation(L)->GetSpriteManager();
		sprites->Delete( (Sprite*)this );
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
