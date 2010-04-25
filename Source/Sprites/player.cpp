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

/**\brief Fetch the current player Instance
 */
Player *Player::Instance( void ) {
	if( pInstance == NULL ) { // is this the first call?
		LogMsg(ERROR,"Attempting to use Player information when no player is loaded!");
		assert(0);
	}

	return( pInstance );
}

/**\brief Create a new Player
 * This is used instead of a normal class constructor
 */
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

	pInstance->SetCredits(2000);

	Players::Instance()->Add((Component*)pInstance);
}

/**\brief Create a new Player
 */
void Player::LoadLast() {
	list<string>* names = Players::Instance()->GetNames();
	// TODO: sort by time
	Player::Load( *(names->begin()) );
}

/**\brief Load a given Player
 */
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


/**\brief Constructor
 */
Player::Player() {
	this->SetRadarColor( Color::Get(0xFF,0xD7,0) );
}

/**\brief Destructor
 */
Player::~Player() {
	pInstance = NULL;
	LogMsg(INFO, "You have been destroyed..." );
}

/**\brief Run the Player Update
 */
void Player::Update( void ) {
	Ship::Update();
}

/**\brief Parse one player out of an xml node
 */
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
	} else PPA_MATCHES( "credits" ) {
		SetCredits( atoi(value.c_str()) );
	} else if(Weapon::AmmoNameToType(subName)<max_ammo){
	// Check if this is an Ammo listing
		addAmmo( Weapon::AmmoNameToType(subName), atoi(value.c_str()) );
		LogMsg(DEBUG1,"Adding %d Ammo of type %s",atoi(value.c_str()),subName.c_str() );
	} else if(Commodities::Instance()->Get(subName)!=NULL){
	// Check if this is a Commodity listing
		StoreCommodities( subName, atoi(value.c_str()) );
	} else {
		LogMsg(ERROR,"The Player Class cannot understand the XML key '%s'.",subName.c_str() );
		return false;
	}
	return true;
}

/**\brief Save this Player to an xml node
 */
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
	snprintf(buff, sizeof(buff), "%d", this->GetCredits() );
	xmlNewChild(section, NULL, BAD_CAST "credits", BAD_CAST buff );
	map<Weapon*,int> weapons = this->getWeaponsAndAmmo();
	map<Weapon*,int>::iterator it = weapons.begin();
	while( it!=weapons.end() ) {
		xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST ((*it).first)->GetName().c_str() );
		++it;
	}
	for(int a=0;a<max_ammo;a++){
		if(getAmmo(AmmoType(a))){
			snprintf(buff, sizeof(buff), "%d", getAmmo(AmmoType(a)) );
			xmlNewChild(section, NULL, BAD_CAST Weapon::AmmoTypeToName((AmmoType)a).c_str(), BAD_CAST buff );
		}
	}
	map<Commodity*,unsigned int> cargo = this->getCargo();
	map<Commodity*,unsigned int>::iterator iter = cargo.begin();
	while( iter!=cargo.end() ) {
		snprintf(buff, sizeof(buff), "%d", (*iter).second );
		xmlNewChild(section, NULL, BAD_CAST ((*iter).first)->GetName().c_str(), BAD_CAST buff );
		++iter;
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


