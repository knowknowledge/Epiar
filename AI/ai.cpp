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

AI::AI() {
	
}

void AI::Update(){
	// Decide
	Player *player = Player::Instance();
	this->Rotate( directionTowards( player->GetWorldPosition()  )  );
	this->Accelerate();
	
	// Now act like a normal ship
	this->Ship::Update();
}

void AI::SetScript(string script){
	my_script = script;
}
