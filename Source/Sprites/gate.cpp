/*
 * Filename      : gate.cpp
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, March 16, 2010
 * Last Modified : Tuesday, March 16, 2010
 * Purpose       : Sprite SubClass for Warp Gates
 * Notes         : A gate is a two-part Sprite that ships can move through
 */

#include "includes.h"
#include "Graphics/image.h"
#include "Sprites/spritemanager.h"
#include "Sprites/sprite.h"
#include "Sprites/gate.h"
#include "Utilities/trig.h"

/**\class Gate
 * \brief A Gate is a dual-sprite; it has a Top and a Bottom.
 *        This allows ships to fly through the gate.
 * */

/**\brief Creates a Top Gate as well as a Bottom Gate automatically
 */

Gate::Gate(Coordinate pos) {
	top = true;
	SetImage( Image::Get("Resources/Graphics/gate1_top.png") );
	
	// Create the PartnerID Gate
	Gate* partner = new Gate(this->GetID());
	partnerID = partner->GetID();
	exitID = 0;
	SpriteManager::Instance()->Add((Sprite*)partner);

	// Set both Position and Angle at the same time
	SetWorldPosition(pos);
	SetAngle( float( rand() %360 ) );
}

/**\brief Creates a Bottom Gate
 */

Gate::Gate(int topID) {
	top = false;
	SetImage( Image::Get("Resources/Graphics/gate1_bottom.png") );
	partnerID = topID;
}

/**\brief Set the Angle for Top and Bottom at once
 *        This overrides the normal Sprite SetAngle.
 */

void Gate::SetAngle(float angle) {
	this->_SetAngle(angle);
	GetPartner()->_SetAngle(angle);
}

/**\brief Set the Position for Top and Bottom at once.
 *        This overrides the normal Sprite SetWorldPosition.
 */

void Gate::SetWorldPosition(Coordinate c) {
	this->_SetWorldPosition(c);
	GetPartner()->_SetWorldPosition(c);
}

/**\brief Set the exit for this Gate
 */

void Gate::SetExit(int spriteID) {
	Sprite* exit = SpriteManager::Instance()->GetSpriteByID(spriteID);
	if (exit == NULL) {
		exitID = 0;
	//} else if (exit->GetDrawOrder() & (DRAW_ORDER_GATE_TOP|DRAW_ORDER_GATE_BOTTOM)){
	// Only use other Gates as Exits
	} else {
		exitID = exit->GetID();
		assert(exitID == spriteID);
	}
}

/**\brief Get the Top Gate
 *        All Actions should go through the Top Gate to avoid confusion.
 * \return Pointer to the Top Gate
 */

Gate* Gate::GetTop() {
	if(top){
		return this;
	} else {
		Sprite* top = SpriteManager::Instance()->GetSpriteByID(partnerID);
		assert(top!=NULL);
		return (Gate*)top;
	}
}

/**\brief Get the Partner Gate
 * \return Pointer to the Partner Gate
 */

Gate* Gate::GetPartner() {
	Sprite* partner = SpriteManager::Instance()->GetSpriteByID(partnerID);
	assert(partner!=NULL);
	return (Gate*)partner;
}

/**\brief Check if any Ships have entered the Gate, send them somewhere
 * \todo Where to send the ships should not be this random
 * \todo Non-Player ships should just disappear
 */

void Gate::Update() {
	// The Bottom Gate doesn't do anything
	if(!top) return;

	SpriteManager *sprites = SpriteManager::Instance();
	Sprite* ship = sprites->GetNearestSprite( (Sprite*)this, 50,DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER );

	if(ship!=NULL) {
		if(exitID != 0) {
			SendToExit(ship);
		} else if(rand()&1) {
			SendToRandomLocation(ship);
		} else {
			SendRandomDistance(ship);
		}
	}
}

/**\brief Teleport any ship that enters the gate to a random location
 */

void Gate::SendToRandomLocation(Sprite* ship) {
	Coordinate destination = Coordinate( float(rand()%GATE_RADIUS - GATE_RADIUS/2), float(rand()%GATE_RADIUS - GATE_RADIUS/2));
	ship->SetWorldPosition( destination );
}

/**\brief Teleport a ship to the exit gate
 * The exit point is offset slightly to avoid re-entering the exit
 */

void Gate::SendToExit(Sprite* ship) {
	Trig *trig = Trig::Instance();
	Sprite* exit = SpriteManager::Instance()->GetSpriteByID(exitID);
	if(exit==NULL) {
		LogMsg(ERROR,"Gate %d cannot send to non-existant exit (%d)",this->GetID(),exitID);
		return;
	}
	float distance = 50.0f;
	// The Ship's Angle and Momentum should make it appear like it is exiting the Gate
	ship->SetWorldPosition( exit->GetWorldPosition() + Coordinate(distance,0).RotateTo( exit->GetAngle() ) );
	ship->SetMomentum( ship->GetMomentum().RotateTo( exit->GetAngle() ) );
	ship->SetAngle( exit->GetAngle() );
}

/**\brief Teleport a Ship in the direction of the Gate by a random distance
 */

void Gate::SendRandomDistance(Sprite* ship) {
	float distance = float(rand()%GATE_RADIUS);
	Trig *trig = Trig::Instance();
	float angle = static_cast<float>(trig->DegToRad( GetAngle() ));
	Coordinate destination = GetWorldPosition() +
		   Coordinate( trig->GetCos( angle ) * distance,
					  -trig->GetSin( angle ) * distance );
	ship->SetWorldPosition( destination );
}
