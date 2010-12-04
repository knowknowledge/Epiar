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
#include "Sprites/planets.h"
#include "Utilities/components.h"

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

void Player::AcceptMission( Mission *mission ) {
	assert( mission != NULL );
	mission->Accept();
	missions.push_back( mission );
	LogMsg(INFO, "Player has accepted the Mission to %s", mission->GetName().c_str() );
}

void Player::RejectMission( string missionName ) {
	list<Mission*>::iterator iter;
	for( iter=missions.begin(); iter!=missions.end(); ++iter )
	{
		if( (*iter)->GetName() == missionName ) {
			(*iter)->Reject();
			delete (*iter);
			missions.erase( iter );
			LogMsg(INFO, "Player has abandoned the Mission to %s.", missionName.c_str() );
			return;
		}
	}

	LogMsg(ERR, "Failed to find the Mission named %s.", missionName.c_str() );
}

/**\brief set name of last planet visited
 */
void Player::setLastPlanet( string planetName){
	lastPlanet=planetName;
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
	bool missionOver;
	list<Mission*>::iterator i;
	for( i = missions.begin(); i != missions.end(); ++i ) {
		missionOver = (*i)->Update();
		if( missionOver ) {
			LogMsg(INFO, "Completed the Mission %s", (*i)->GetName().c_str() );
			// Remove this completed mission from the list
			i = missions.erase( i );
		}
	}

	Ship::Update();
}

/**\brief Parse one player out of an xml node
 */
bool Player::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;
	Coordinate pos;
	if( (attr = FirstChildNamed(node, "planet"))){
		string temp;
		xmlNodePtr name = FirstChildNamed(attr,"name");
		lastPlanet = NodeToString(doc,name);
		Planet* p = Planets::Instance()->GetPlanet( lastPlanet );
		if( p != NULL ) {
			SetWorldPosition( p->GetWorldPosition() );
		}
	}else return false;

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

	for( attr = FirstChildNamed(node,"Mission"); attr!=NULL; attr = NextSiblingNamed(attr,"Mission") ){
		Mission *mission = Mission::FromXMLNode(doc,attr);
		if( mission != NULL ) {
			LogMsg(INFO, "Successfully loaded the %s mission of player '%s'", mission->GetName().c_str(), this->GetName().c_str() );
			missions.push_back( mission );
		} else {
			LogMsg(INFO, "Aborted loading mission of player '%s'", this->GetName().c_str() );
		}
	}

	if(this->ConfigureWeaponSlots(doc, node)){
		// great - it worked
		LogMsg( INFO, "Successfully loaded weapon slots");
	}
	else {
		LogMsg( ERR, "Weapon slot XML helper failed to configure weapon slots");
	}

	if( (attr = FirstChildNamed(node,"lastLoadTime")) ){
		lastLoadTime = NodeToInt(doc,attr);
	} else {
		lastLoadTime = (time_t)0;
	}

	return true;
}

/**\brief Configure the ship's weapon slots, first copying the default slots
  * from the model, then updating it according to the saved player XML.
 */
bool Player::ConfigureWeaponSlots( xmlDocPtr doc, xmlNodePtr node ) {

	xmlNodePtr slotPtr;
	string value;

	// Grab the default slots from the player's ship model, then update it below.
	// This makes a copy, not a pointer or reference, so we don't have to worry
	// that it might alter the slots in the model.
	this->weaponSlots = this->GetModel()->GetWeaponSlots();

	for( slotPtr = FirstChildNamed(node,"weapSlot"); slotPtr != NULL; slotPtr = NextSiblingNamed(slotPtr,"weapSlot") ){
		ws_t *existingSlot = NULL;

		xmlNodePtr attr;

		string slotName;
		if( (attr = FirstChildNamed(slotPtr,"name")) ){
			value = NodeToString(doc,attr);
			slotName = value;
		} else return false;

		for(unsigned int s = 0; s < weaponSlots.size(); s++){
			if(weaponSlots[s].name == slotName){
				existingSlot = &weaponSlots[s];
				break;
			}
		}
		if(!existingSlot) return false;

		if( (attr = FirstChildNamed(slotPtr,"content")) ){
			// this check is necessary because NodeToString() won't translate <item></item> into ""
			if(attr->xmlChildrenNode)
				value = NodeToString(doc,attr);
			else
				value = ""; // slot is empty

			existingSlot->content = value;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"firingGroup")) ){
			value = NodeToString(doc,attr);
			existingSlot->firingGroup = (short)atoi(value.c_str());
		} else return false;

	}

	// no need to push any ws_t back into the player's weaponSlots; slots were edited in place

	return true;
}



/**\brief Save this Player to an xml node
 */
xmlNodePtr Player::ToXMLNode(string componentName) {
	char buff[256];
	char *timestamp;
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	// Player Stats
	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	xmlNodePtr planet = xmlNewNode(NULL, BAD_CAST "planet" );
	xmlNewChild(planet, NULL, BAD_CAST "name", BAD_CAST lastPlanet.c_str()); 
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetX() );
	xmlNewChild(planet, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetY() );
	xmlNewChild(planet, NULL, BAD_CAST "y", BAD_CAST buff );
	xmlAddChild(section,planet);
	
	xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST this->GetModelName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST this->GetEngineName().c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetCredits() );
	xmlNewChild(section, NULL, BAD_CAST "credits", BAD_CAST buff );

	// this part is becoming less important and may be removed at some point
	for(unsigned int i = 0; i < weaponSlots.size(); i++){
		char *w = (char*)weaponSlots[i].content.c_str();
		if(strlen(w) > 0)
			xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST w);
	}


	for(int a=0;a<max_ammo;a++){
		if(GetAmmo(AmmoType(a)) != 0 ){ // Don't save empty ammo Nodes
			snprintf(buff, sizeof(buff), "%d", GetAmmo(AmmoType(a)) );

			xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "ammo");
			xmlNewChild(ammo, NULL, BAD_CAST "type", BAD_CAST Weapon::AmmoTypeToName((AmmoType)a).c_str() );
			xmlNewChild(ammo, NULL, BAD_CAST "amount", BAD_CAST buff );
			xmlAddChild(section, ammo);
		}
	}

	// save info about whichever items players are able to change in their slot configuration (content and firing group)
	char *ntos = (char*)malloc(256);
	for(unsigned int w=0; w < weaponSlots.size(); w++){
		ws_t *slot = &weaponSlots[w];
		xmlNodePtr slotPtr = xmlNewNode(NULL, BAD_CAST "weapSlot");

		xmlNewChild(slotPtr, NULL, BAD_CAST "name", BAD_CAST slot->name.c_str() );
		xmlNewChild(slotPtr, NULL, BAD_CAST "content", BAD_CAST slot->content.c_str() );

		snprintf(ntos, 256, "%d", slot->firingGroup);
		xmlNewChild(slotPtr, NULL, BAD_CAST "firingGroup", BAD_CAST ntos);
		xmlAddChild(section, slotPtr); // saved player data is less structured than model data, so just add it here
	}
	free(ntos);

	// Cargo
	map<Commodity*,unsigned int> cargo = this->GetCargo();
	map<Commodity*,unsigned int>::iterator iter_com;
	for(iter_com = cargo.begin(); iter_com!=cargo.end(); ++iter_com) {
		if( !(*iter_com).second )
		{
			continue; // Don't Save empty cargo Nodes
		}
		snprintf(buff, sizeof(buff), "%d", (*iter_com).second );
		xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "cargo");
		xmlNewChild(ammo, NULL, BAD_CAST "type", BAD_CAST ((*iter_com).first)->GetName().c_str() );
		xmlNewChild(ammo, NULL, BAD_CAST "amount", BAD_CAST buff );
		xmlAddChild(section, ammo);
	}

	// Outfit
	list<Outfit*> *outfits = this->GetOutfits();
	for( list<Outfit*>::iterator it_w = outfits->begin(); it_w!=outfits->end(); ++it_w ){
		xmlNewChild(section, NULL, BAD_CAST "outfit", BAD_CAST (*it_w)->GetName().c_str() );
	}

	// Missions
	list<Mission*>::iterator iter_mission;
	for(iter_mission = missions.begin(); iter_mission != missions.end(); ++iter_mission){
		xmlAddChild( section,  (*iter_mission)->ToXMLNode() );
	}

	// Last Load Time
	snprintf(buff, sizeof(buff), "%d", (int)lastLoadTime );
	xmlNewChild(section, NULL, BAD_CAST "lastLoadTime", BAD_CAST buff );

	// Save a Human readable comment to explain the Last Load time
	strcpy( buff, "Last Load: " );
	timestamp = ctime( &lastLoadTime );
	timestamp[strlen(timestamp)-1] = '\0';
	xmlAddChild( section, xmlNewComment( BAD_CAST timestamp));

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

	// We check the planet location at loadtime in case the planet has moved or the lastPlanet has changed.
	// This happens with the --random-universe option.
	Planet* p = Planets::Instance()->GetPlanet( newPlayer->lastPlanet );
	if( p != NULL ) {
		newPlayer->SetWorldPosition( p->GetWorldPosition() );
	} else {
		newPlayer->SetWorldPosition( defaultLocation );
	}

	// We can't start the game with bad player Information
	assert( newPlayer->GetModelName() != "" );
	assert( newPlayer->GetEngineName() != "" );

	// Restart the missions that were ongoing.
	list<Mission*>::iterator iter_m;
	list<Mission*>* missions = newPlayer->GetMissions();
	for( iter_m = missions->begin(); iter_m != missions->end(); ++iter_m) {
		(*iter_m)->Accept(); ///< TODO: This should be a distinct function.  Mission::Load perhaps?
	}

	// Remember this Player
	newPlayer->lastLoadTime = time(NULL);
	SpriteManager::Instance()->Add( newPlayer );
	Camera::Instance()->Focus( newPlayer );

	Player::pInstance = newPlayer;

	LogMsg(INFO, "Successfully loaded the player: '%s'.",newPlayer->GetName().c_str() );
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

