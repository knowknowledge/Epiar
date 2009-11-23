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

/**\class AI
 * \brief AI controls the non-player shipts.*/

AI::AI() {
	
}

void AI::Update(){
	// Decide
	
	// Now act like a normal ship
	this->Ship::Update();
}

void AI::SetScript( string& script )
{
	my_script = script;
}


