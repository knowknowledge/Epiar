/*
 * Filename      : gate.cpp
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, March 16, 2010
 * Last Modified : Tuesday, March 16, 2010
 * Purpose       : Sprite SubClass for Warp Gates
 * Notes         : A gate is a two-part Sprite that ships can move through
 */

#include "includes.h"
#include "common.h"

#include "Graphics/image.h"
#include "Sprites/spritemanager.h"
#include "Sprites/sprite.h"
#include "Sprites/gate.h"
#include "Utilities/trig.h"
#include "Utilities/log.h"
#include "Engine/simulation_lua.h"

/** \addtogroup Sprites
 * @{
 */

/**\class Gate
 * \brief A Gate is a dual-sprite; it has a Top and a Bottom.
 *        This allows ships to fly through the gate.
 * */

/**\brief Creates a Top Gate as well as a Bottom Gate automatically
 * \todo Remove the SpriteManager Instance access.
 */
Gate::Gate(Coordinate pos, string _name) {
	top = true;
	SetImage( Image::Get("Resources/Graphics/gate1_top.png") );
	
	// Create the PartnerID Gate
	Gate* partner = new Gate(GetID());
	partnerID = partner->GetID();
	exitID = 0;
	SpriteManager::Instance()->Add((Sprite*)partner);

	// Set both Position and Angle at the same time
	SetWorldPosition(pos);
	SetAngle( float( rand() %360 ) );

	if( _name == "" ) {
		stringstream val_ss;
		val_ss << GetID();
		val_ss >> _name;
	}
	SetName( _name );
}

/**\brief Creates a Bottom Gate
 */
Gate::Gate(int topID) {
	top = false;
	SetImage( Image::Get("Resources/Graphics/gate1_bottom.png") );
	partnerID = topID;
}

/** \brief Load a Gate from XML
 */
bool Gate::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;
	Coordinate pos;

	if( (attr = FirstChildNamed(node,"x")) ){
		value = NodeToString(doc,attr);
		pos.SetX( atof( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"y")) ){
		value = NodeToString(doc,attr);
		pos.SetY( atof( value.c_str() ));
	} else return false;

	SetWorldPosition( pos );

	if( (attr = FirstChildNamed(node,"exit")) ){
		value = NodeToString(doc,attr);
		Gate* exit = Gates::Instance()->GetGate( value );
		if( exit != NULL ) {
			Gate::SetPair(this,exit);
		}
	}

	return true;
}

/** \brief Save a Gate to an XML Node
 * \todo Remove the SpriteManager Instance access.
 */
xmlNodePtr Gate::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	
	Sprite* exit = SpriteManager::Instance()->GetSpriteByID( exitID );
	if( (exit != NULL) && (exit->GetDrawOrder() & (DRAW_ORDER_GATE_TOP|DRAW_ORDER_GATE_BOTTOM) ) ) {
		xmlNewChild(section, NULL, BAD_CAST "exit", BAD_CAST ((Gate*)exit)->GetName().c_str() );
	}

	return section;
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
	exitID = spriteID;
}

void Gate::SetPair(Gate* one, Gate* two) {
	float angle;
	one->SetExit( two->GetID() );
	two->SetExit( one->GetID() );
	angle = (two->GetWorldPosition() - one->GetWorldPosition()).GetAngle();
	two->SetAngle(angle+180);
	one->SetAngle(angle);
}

/**\brief Get the Top Gate
 * \details All Actions should go through the Top Gate to avoid confusion.
 * \todo Remove the SpriteManager Instance access.
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

/**\brief Get the Exit Gate paired with this gate
 * \todo Remove the SpriteManager Instance access.
 * \return Pointer to the Exit Gate
 */
Sprite* Gate::GetExit() {
		return SpriteManager::Instance()->GetSpriteByID(exitID);
}

/**\brief Get the Partner Gate
 * \todo Remove the SpriteManager Instance access.
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

void Gate::Update( lua_State *L ) {
	// The Bottom Gate doesn't do anything
	if(!top) return;

	SpriteManager *sprites = Simulation_Lua::GetSimulation(L)->GetSpriteManager();
	Ship* ship = (Ship*)sprites->GetNearestSprite( (Sprite*)this, 50,DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER );

	if(ship!=NULL) {
		if(exitID != 0) {
			SendToExit(ship);
		} else if(rand()&1) {
			SendToRandomLocation(ship);
		} else {
			SendRandomDistance(ship);
		}
	}

	Sprite::Update( L );
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

void Gate::SendToExit(Ship* ship) {
	Sprite* exit = SpriteManager::Instance()->GetSpriteByID(exitID);
	if(exit==NULL) {
		LogMsg(ERR,"Gate %d cannot send to non-existant exit (%d)",this->GetID(),exitID);
		return;
	}
	float distance = 50.0f;
	float exitAngle = normalizeAngle( exit->GetAngle()+180 );
	// The Ship's Angle and Momentum should make it appear like it is exiting the Gate
	//ship->SetWorldPosition(  );
	ship->SetMomentum( ship->GetMomentum().RotateTo( exitAngle ) );
	ship->SetAngle( exitAngle );
	ship->Jump( exit->GetWorldPosition() + Coordinate(distance,0).RotateTo( exitAngle ), false );
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

/** \class Gates
 *  \brief A collection of Gate Component objects.
 */

Gates *Gates::pInstance = 0; // initialize pointer

/**\brief Returns or creates the gates instance.
 * \return Pointer to the gates instance
 */
Gates *Gates::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Gates; // create the solid instance
		pInstance->rootName = "gates";
		pInstance->componentName = "gate";
	}
	return( pInstance );
}

/** @} */

