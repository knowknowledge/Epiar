/**\file			ai.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "AI/ai.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "Utilities/lua.h"

/**\class AI
 * \brief AI controls the non-player shipts.*/

AI::AI() {
	
}

/**\brief Updates the AI controlled ship by first calling the Lua function
 * and then calling Ship::Update()
 */
void AI::Update(){
	// Decide
	Lua::Call("MoveShip","i>",this->GetID() );

	// Now act like a normal ship
	this->Ship::Update();
}

/**\brief Sets the Lua script to be used for this AI
 */
void AI::SetScript( string& script )
{
	my_script = script;
}


