/*
 * Filename      : ai.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "AI/ai.h"

#include "Sprites/Player.h"
#include "Sprites/spriteManager.h"

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


