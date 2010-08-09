/**\file			player.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/player.h"
#include "Utilities/camera.h"
#include "Sprites/spritemanager.h"

/**\class Player
 * \brief Main player-specific functions and handle. */

Player *Player::pInstance = 0;

/**\brief Fetch the current player Instance
 */
Player *Player::Instance( void ) {
	if( pInstance == NULL ) { // is this the first call?
		LogMsg(ERR,"Attempting to use Player information when no player is loaded!");
		assert(0);
	}

	return( pInstance );
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
bool Player::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
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

	if( (attr = FirstChildNamed(node,"model")) ){
		value = NodeToString(doc,attr);
		Model* model = Models::Instance()->GetModel( value );
		if( NULL!=model) {
			SetModel( model );
		} else {
			LogMsg(ERR,"No such model as '%s'", value.c_str());
			return false;
		}
	} else return false;
	
	if( (attr = FirstChildNamed(node,"engine")) ){
		value = NodeToString(doc,attr);
		Engine* engine = Engines::Instance()->GetEngine( value );
		if( NULL!=engine) {
			SetEngine( engine );
		} else {
			LogMsg(ERR,"No such engine as '%s'", value.c_str());
			return false;
		}
	} else return false;


	if( (attr = FirstChildNamed(node,"credits")) ){
		value = NodeToString(doc,attr);
		SetCredits( atoi(value.c_str()) );
	} else return false;

	for( attr = FirstChildNamed(node,"weapon"); attr!=NULL; attr = NextSiblingNamed(attr,"weapon") ){
		AddShipWeapon( NodeToString(doc,attr) );
	}

	for( attr = FirstChildNamed(node,"outfit"); attr!=NULL; attr = NextSiblingNamed(attr,"outfit") ){
		AddOutfit( NodeToString(doc,attr) );
	}

	for( attr = FirstChildNamed(node,"cargo"); attr!=NULL; attr = NextSiblingNamed(attr,"cargo") ){
		xmlNodePtr type = FirstChildNamed(attr,"type");
		xmlNodePtr ammt = FirstChildNamed(attr,"amount");
		if(!type || !ammt)
			return false;
		if( NodeToInt(doc,ammt) > 0 )
		{
			StoreCommodities( NodeToString(doc,type), NodeToInt(doc,ammt) );
		}
	}

	for( attr = FirstChildNamed(node,"ammo"); attr!=NULL; attr = NextSiblingNamed(attr,"ammo") ){
		xmlNodePtr type = FirstChildNamed(attr,"type");
		xmlNodePtr ammt = FirstChildNamed(attr,"amount");
		if(!type || !ammt)
			return false;
		AmmoType ammoType = Weapon::AmmoNameToType( NodeToString(doc,type) );
		int ammoCount = NodeToInt(doc,ammt);
		if( ammoType < max_ammo ) {
			AddAmmo( ammoType, ammoCount );
		} else return false;
	}

	if( (attr = FirstChildNamed(node,"lastLoadTime")) ){
		lastLoadTime = NodeToInt(doc,attr);
	} else {
		lastLoadTime = (time_t)0;
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
	map<Weapon*,int> weapons = this->GetWeaponsAndAmmo();
	map<Weapon*,int>::iterator it = weapons.begin();
	while( it!=weapons.end() ) {
		xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST ((*it).first)->GetName().c_str() );
		++it;
	}
	for(int a=0;a<max_ammo;a++){
		if(GetAmmo(AmmoType(a))){
			if( GetAmmo(AmmoType(a)) )
				continue; // Don't save empty ammo Nodes
			snprintf(buff, sizeof(buff), "%d", GetAmmo(AmmoType(a)) );
			xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "ammo");
			xmlNewChild(ammo, NULL, BAD_CAST "type", BAD_CAST Weapon::AmmoTypeToName((AmmoType)a).c_str() );
			xmlNewChild(ammo, NULL, BAD_CAST "amount", BAD_CAST buff );
			xmlAddChild(section, ammo);
		}
	}
	map<Commodity*,unsigned int> cargo = this->GetCargo();
	map<Commodity*,unsigned int>::iterator iter;
	for(iter = cargo.begin(); iter!=cargo.end(); ++iter) {
		if( (*iter).second )
		{
			continue; // Don't Save empty cargo Nodes
		}
		snprintf(buff, sizeof(buff), "%d", (*iter).second );
		xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "cargo");
		xmlNewChild(ammo, NULL, BAD_CAST "type", BAD_CAST ((*iter).first)->GetName().c_str() );
		xmlNewChild(ammo, NULL, BAD_CAST "amount", BAD_CAST buff );
		xmlAddChild(section, ammo);
	}
	list<Outfit*> *outfits = this->GetOutfits();
	for( list<Outfit*>::iterator it_w = outfits->begin(); it_w!=outfits->end(); ++it_w ){
		xmlNewChild(section, NULL, BAD_CAST "outfit", BAD_CAST (*it_w)->GetName().c_str() );
	}

	snprintf(buff, sizeof(buff), "%d", (int)lastLoadTime );
	xmlNewChild(section, NULL, BAD_CAST "lastLoadTime", BAD_CAST buff );
	
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

/**\brief Create a new Player
 * This is used instead of a normal class constructor
 */
Player* Players::CreateNew(string playerName) {
	Player* newPlayer = new Player;

	newPlayer->name = playerName;

	newPlayer->SetModel( defaultModel );
	newPlayer->SetEngine( defaultEngine );
	newPlayer->SetCredits( defaultCredits );
	newPlayer->SetWorldPosition( defaultLocation );

	newPlayer->lastLoadTime = time(NULL);

	// Focus the camera on the sprite
	Camera::Instance()->Focus( newPlayer );
	Add((Component*)newPlayer);
	SpriteManager::Instance()->Add(newPlayer);
	Player::pInstance = newPlayer;

	return newPlayer;
}

/**\brief Create a new Player
 */
Player* Players::LoadLast() {
	list<string>* names = GetNames();
	list<string>::iterator i = names->begin();
	Player* latest;

	if( names->empty() ){
		return false;
	}

	latest = GetPlayer(*i);
	i++;
	for(; i != names->end(); ++i )
	{
		if( latest->lastLoadTime < GetPlayer(*i)->lastLoadTime )
			latest = GetPlayer(*i);
	}
	return LoadPlayer( latest->GetName() );
}

/**\brief Load a given Player
 */
Player* Players::LoadPlayer(string playerName) {
	Player* newPlayer = GetPlayer(playerName);
	// If the player saved a bad Model or Engine, pick the default
	if(newPlayer->GetModelName() == "") {
		LogMsg(ERR, "The Player '%s' has been corrupted: Bad model.",newPlayer->GetName().c_str() );
		newPlayer->SetModel( defaultModel );
	}
	if(newPlayer->GetEngineName() == "") {
		LogMsg(ERR, "The Player '%s' has been corrupted: Bad engine.",newPlayer->GetName().c_str() );
		newPlayer->SetEngine( defaultEngine );
	}

	// We can't start the game with bad player Information
	assert( newPlayer->GetModelName() != "" );
	assert( newPlayer->GetEngineName() != "" );

	// Remeber this Player
	newPlayer->lastLoadTime = time(NULL);
	SpriteManager::Instance()->Add( newPlayer );
	Camera::Instance()->Focus( newPlayer );

	Player::pInstance = newPlayer;

	LogMsg(ERR, "Loaded the Player '%s'.",newPlayer->GetName().c_str() );
	return newPlayer;
}

/**\brief Set Default values for new Players
 */
void Players::SetDefaults(
	Model *_defaultModel,
	Engine *_defaultEngine,
	int _defaultCredits,
	Coordinate _defaultLocation)
{
	assert(_defaultModel);
	assert(_defaultEngine);
	assert(_defaultCredits > 0);
	defaultModel = _defaultModel;
	defaultEngine = _defaultEngine;
	defaultCredits = _defaultCredits;
	defaultLocation = _defaultLocation;
}

