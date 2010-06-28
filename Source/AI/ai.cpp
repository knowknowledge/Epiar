/**\file			ai.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "AI/ai.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "Utilities/lua.h"

/**\class AI
 * \brief AI controls the non-player shipts.*/

AI::AI(string machine) :
	stateMachine(machine),
	state("default")
{
	
}

/**\brief Updates the AI controlled ship by first calling the Lua function
 * and then calling Ship::Update()
 */
void AI::Update(){
	// Decide
	lua_State *L = Lua::CurrentState();
	lua_settop(L,0);

	// Get the current state machine
	lua_getglobal(L, stateMachine.c_str() );
	if( ! lua_istable(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no State Machine named '%s'!", stateMachine.c_str() );
		return; // This ship will just sit idle...
	}

	// Get the current state
	lua_pushstring(L, state.c_str() );
	lua_gettable(L,1);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(WARN, "The State Machine '%s' has no state '%s'.", stateMachine.c_str(), state.c_str() );
		lua_getglobal(L, stateMachine.c_str() );
		lua_pushstring(L, "default" );
		lua_gettable(L,1);
		if( !lua_isfunction(L,lua_gettop(L)) )
		{
			LogMsg(ERR, "The State Machine '%s' has no default state.", stateMachine.c_str() );
			lua_settop(L,0);
			return; // This ship will just sit idle...
		}
	}

	// Push Current AI Variables
	lua_pushinteger( L, this->GetID() );
	lua_pushnumber( L, this->GetWorldPosition().GetX() );
	lua_pushnumber( L, this->GetWorldPosition().GetY() );
	lua_pushnumber( L, this->GetAngle() );
	lua_pushnumber( L, this->GetMomentum().GetMagnitude() ); // Speed
	lua_pushnumber( L, this->GetMomentum().GetAngle() ); // Vector

	// Run the current AI state
	//printf("Call:"); Lua::stackDump(L); // DEBUG
	if( lua_pcall(L, 6, 1, 0) != 0)
	{
		printf("Failed to run %s(%s): %s\n", stateMachine.c_str(), state.c_str(), lua_tostring(L, -1));
		lua_settop(L,0);
		return;
	}
	//printf("Return:"); Lua::stackDump(L); // DEBUG
	
	if( lua_isstring( L, lua_gettop(L) ) )
	{
		state = (string)luaL_checkstring(L, lua_gettop(L));
	}

	//printf("Complete:");Lua::stackDump(L); // DEBUG
	lua_settop(L,0);

	// Now act like a normal ship
	this->Ship::Update();
}

void AI::Draw(){
	this->Ship::Draw();
	if( OPTION(int,"options/development/debug-ai") ) {
		SansSerif->SetColor(1.0,1.0,1.0,1.0);
		Coordinate position = this->GetWorldPosition();
		SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight(),stateMachine);
		SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight()+20,state);
	}
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
