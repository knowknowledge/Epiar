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

AI::AI(string machine) :
	stateMachine(machine),
	state("")
{
	
}

/**\brief Updates the AI controlled ship by first calling the Lua function
 * and then calling Ship::Update()
 */
void AI::Update(){
	// Decide
	string newstate;
	
	Lua::Call(this->stateMachine.c_str(), "iddddds>s",
		this->GetID(),
		this->GetWorldPosition().GetX(),
		this->GetWorldPosition().GetY(),
		this->GetAngle(),
		this->GetMomentum().GetMagnitude(), // Speed
		this->GetMomentum().GetAngle(), // Vector
		this->state.c_str()
		,&(newstate)
		);
	state= newstate;

	// Now act like a normal ship
	this->Ship::Update();
}


/**\fn AI::SetStateMachine(string _machine)
 * \brief Set's the state machine.
 */

/**\fn AI::SetState(string _state)
 * \brief Set's the current state.
 */

/**\fn AI::GetStateMachine
 * \brief Retrieves the state machine.
 */

/**\fn AI::GetState
 * \brief Retrieves the current state.
 */
