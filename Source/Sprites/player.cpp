/**\file			player.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/player.h"

/**\class Player
 * \brief Main player-specific functions and handle. */

Player *Player::pInstance = 0;

Player *Player::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Player; // create the sold instance
		pInstance->name = "Steve";
		Players::Instance()->Add((Component*)pInstance);
	}

	return( pInstance );
}

void Player::LoadLast() {
	list<string>* names = Players::Instance()->GetNames();
	// TODO: sort by time
	pInstance = Players::Instance()->GetPlayer( *(names->begin()) );
	// If the player saved a bad Model or Engine, pick the default
	if(pInstance->GetModelName() == "") {
		Log::Error( "The Player '%s' has been corrupted: Bad model.",pInstance->GetName().c_str() );
		pInstance->SetModel( Models::Instance()->GetModel( OPTION(string,"options/defaults/playerModel" ) ) );
	}
	if(pInstance->GetEngineName() == "") {
		Log::Error( "The Player '%s' has been corrupted: Bad engine.",pInstance->GetName().c_str() );
		pInstance->SetEngine( Engines::Instance()->GetEngine( OPTION(string,"options/defaults/playerModel" ) ) );
	}
	// We can't start the game with bad player Information
	assert( pInstance->GetModelName() != "" );
	assert( pInstance->GetEngineName() != "" );
}

Player::Player() {
	this->SetRadarColor( Color::Get(0xFF,0xD7,0) );

}

Player::~Player() {
	pInstance = NULL;
	Log::Message( "You have been destroyed..." );
}

void Player::Update( void ) {
	Ship::Update();
}

bool Player::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "x" ) {
		Coordinate pos = GetWorldPosition();
		pos.SetX( (double)atof( value.c_str() ) );
		SetWorldPosition( pos );
	} else PPA_MATCHES( "y" ) {
		Coordinate pos = GetWorldPosition();
		pos.SetY( (double)atof( value.c_str() ) );
		SetWorldPosition( pos );
	} else PPA_MATCHES( "model" ) {
		SetModel( Models::Instance()->GetModel( value ) );
	} else PPA_MATCHES( "engine" ) {
		SetEngine( Engines::Instance()->GetEngine( value ) );
	} else PPA_MATCHES( "weapon" ) {
		addShipWeapon( value );
		addAmmo( value, 100 );
	}
	return true;
}

xmlNodePtr Player::ToXMLNode(string componentName) {
	char buff[256];
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());
	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST this->GetModelName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST this->GetEngineName().c_str() );
	map<Weapon*,int> weapons = this->getWeaponsAndAmmo();
	map<Weapon*,int>::iterator it = weapons.begin();
	while( it!=weapons.end() ) {
		xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST ((*it).first)->GetName().c_str() );
		++it;
	}
	
	return section;
}


/**\class Players
 * \brief Collection of Player objects
 */
Players *Players::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Players instance.
 * \return Pointer to the Players instance
 */
Players *Players::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Players; // create the sold instance
		pInstance->rootName = "players";
		pInstance->componentName = "player";
	}
	return( pInstance );
}


