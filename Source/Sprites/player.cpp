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
	if( pInstance == NULL ) { // is this the first call?
		LogMsg(ERROR,"Attempting to use Player information when no player is loaded!");
		//assert(0);
		CreateNew("Steve");
	}

	return( pInstance );
}

void Player::CreateNew(string playerName) {
	pInstance = new Player;
	pInstance->name = playerName;

	//Model* defaultModel = Models::Instance()->GetModel( OPTION(string,"options/defaults/playerModel" ) );
	//Engine* defaultEngine = Engines::Instance()->GetEngine( OPTION(string,"options/defaults/playerEngine" ) );

	//cout<<"Default Model: ["<<OPTION(string,"options/defaults/playerModel")<<"]"<<endl;
	Model* defaultModel = Models::Instance()->GetModel( "Terran FV-5 Frigate"  );
	pInstance->SetModel( defaultModel );
	
	//cout<<"Default Engine: ["<<OPTION(string,"options/defaults/playerEngine")<<"]"<<endl;
	Engine* defaultEngine = Engines::Instance()->GetEngine( "Altaire Corp. NM66 Sublight Thrusters" );
	pInstance->SetEngine( defaultEngine );

	Players::Instance()->Add((Component*)pInstance);
}

void Player::LoadLast() {
	list<string>* names = Players::Instance()->GetNames();
	// TODO: sort by time
	Player::Load( *(names->begin()) );
}

void Player::Load(string playerName) {
	
	Player* newPlayer = Players::Instance()->GetPlayer(playerName);
	// If the player saved a bad Model or Engine, pick the default
	if(newPlayer->GetModelName() == "") {
		LogMsg(ERROR, "The Player '%s' has been corrupted: Bad model.",newPlayer->GetName().c_str() );
		newPlayer->SetModel( Models::Instance()->GetModel( OPTION(string,"options/defaults/playerModel" ) ) );
	}
	if(newPlayer->GetEngineName() == "") {
		LogMsg(ERROR, "The Player '%s' has been corrupted: Bad engine.",newPlayer->GetName().c_str() );
		newPlayer->SetEngine( Engines::Instance()->GetEngine( OPTION(string,"options/defaults/playerEngine" ) ) );
	}
	// We can't start the game with bad player Information
	assert( newPlayer->GetModelName() != "" );
	assert( newPlayer->GetEngineName() != "" );

	pInstance = newPlayer;
}


Player::Player() {
	this->SetRadarColor( Color::Get(0xFF,0xD7,0) );
}

Player::~Player() {
	pInstance = NULL;
	LogMsg(INFO, "You have been destroyed..." );
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


