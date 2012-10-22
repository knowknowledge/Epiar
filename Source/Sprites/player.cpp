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
#include "Sprites/planets.h"
#include "Sprites/spritemanager.h"
#include "Utilities/components.h"
#include "Utilities/file.h"
#include "Utilities/filesystem.h"
#include "Engine/simulation_lua.h"

/** \addtogroup Sprites
 * @{
 */

/**\class Player
 * \brief Main player-specific functions and handle.
 */

/**\brief Load a player from a file.
 * \param[in] filename of a player's xml saved game.
 * \returns pointer to new Player instance.
 */
Player* Player::Load( string filename ) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	Player* newPlayer = new Player();

	File xmlfile = File (filename);
	long filelen = xmlfile.GetLength();
	char *buffer = xmlfile.Read();
	doc = xmlParseMemory( buffer, static_cast<int>(filelen) );
	cur = xmlDocGetRootElement( doc );

	newPlayer->FromXMLNode( doc, cur );

	// We check the planet location at loadtime in case the planet has moved or the lastPlanet has changed.
	// This happens with the --random-universe option.
	Planet* p = Planets::Instance()->GetPlanet( newPlayer->lastPlanet );
	if( p != NULL ) {
		newPlayer->SetWorldPosition( p->GetWorldPosition() );
	} else {
		LogMsg(INFO, "There is no planet named: '%s'.", newPlayer->lastPlanet.c_str() );
	}

	newPlayer->RemoveLuaControlFunc();

	// We can't start the game with bad player Information
	assert( newPlayer->GetModelName() != "" );
	assert( newPlayer->GetEngineName() != "" );

	// Tell Lua to initialize these escorts.
	for(list<Player::HiredEscort*>::iterator iter_escort = newPlayer->hiredEscorts.begin(); iter_escort != newPlayer->hiredEscorts.end(); iter_escort++){
		(*iter_escort)->Lua_Initialize( newPlayer->GetID(), newPlayer->GetWorldPosition() );
	}

	// Remember this Player
	newPlayer->lastLoadTime = time(NULL);

	LogMsg(INFO, "Successfully loaded the player: '%s'.",newPlayer->GetName().c_str() );
	LogMsg(INFO, "Loaded Player '%s' with Model='%s' Engine='%s' Credits = %d at (%.0f,%.0f).",
		newPlayer->GetName().c_str(),
		newPlayer->GetModel()->GetName().c_str(),
		newPlayer->GetEngine()->GetName().c_str(),
		newPlayer->GetCredits(),
		newPlayer->GetWorldPosition().GetX(), newPlayer->GetWorldPosition().GetY()
	);
	
	return newPlayer;
}

/**\brief Set a function to control the Player
 */
void Player::SetLuaControlFunc( string _luaControlFunc ) {
	LogMsg(INFO, "Setting Player control to '%s'", _luaControlFunc.c_str() );
	luaControlFunc = _luaControlFunc;
}

/**\brief Return full control to the player.
 */
void Player::RemoveLuaControlFunc() {
	LogMsg(INFO, "Clearing Player control '%s'", luaControlFunc.c_str() );
	luaControlFunc = "";
}

/**\brief Fetch the current player Instance
 */
void Player::AcceptMission( Mission *mission ) {
	assert( mission != NULL );
	mission->Accept();
	missions.push_back( mission );
	LogMsg(INFO, "Player has accepted the Mission to %s", mission->GetName().c_str() );
}

/**\brief Fetch the current player Instance
 */
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

/**\brief Get an Alliance's favor for a player
 */
int Player::GetFavor(Alliance* alliance ) {
	map<Alliance*,int>::iterator finder = favor.find( alliance );
	if( finder == favor.end() ) {
		return 0;
	}

	return favor[alliance];
}

/**\brief Change an Alliance's favor for a player
 * \param[in] allianceName The name of the Alliance
 * \param[in] deltaFavor The change in favor.  This can be positive or negative.
 */
void Player::UpdateFavor( string allianceName, int deltaFavor ) {
	Alliance *alliance = Alliances::Instance()->GetAlliance( allianceName );

	if( NULL == alliance ) {
		LogMsg(ERR, "Failed to find the Alliance named %s.", allianceName.c_str() );
		return;
	}

	map<Alliance*,int>::iterator finder = favor.find( alliance );
	if( finder == favor.end() ) {
		// Initialize favor for this alliance
		favor[alliance] = 0;
	}
	favor[alliance] += deltaFavor;
}

/**\brief set name of last planet visited
 */
void Player::Land( lua_State *L, Planet* planet ){
	assert( planet );

	if( planet->GetLandable() == false
	 || planet->GetForbidden() == true ) {
		return;
	}

	LogMsg(INFO, "Landed on %s", planet->GetName().c_str() );

	Lua::Call( "landingDialog", "i", planet->GetID() );

	// Stay on the Planet
	SetMomentum( Coordinate(0,0) );

	// Run Land function for each Mission
	bool missionOver;
	list<Mission*>::iterator i = missions.begin();
	while( i != missions.end() )
	{
		missionOver = (*i)->Land();
		if( missionOver ) {
			LogMsg(INFO, "Completed the Mission '%s'", (*i)->GetName().c_str() );
			// Remove this completed mission from the list
			i = missions.erase( i );
		} else {
			++i;
		}
	}

	lastPlanet = planet->GetName();
	Save( Simulation_Lua::GetSimulation(L)->GetName() );
  
  //AdvanceFromLand
}

/**\brief Constructor
 */
Player::Player() {
	this -> playerCheck = true;
	this->SetRadarColor( WHITE );
}

/**\brief Destructor
 */
Player::~Player() {
	LogMsg(INFO, "You have been destroyed..." );
}

/**\brief Run the Player Update
 */
void Player::Update( lua_State *L ) {
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

	if(luaControlFunc != ""){
		Lua::Run(luaControlFunc);
	}

	Ship::Update( L );
}

/**\brief Save an XML file for this player
 * \details The filename is by default the player's name.
 */
void Player::Save( string simulation ) {
	xmlDocPtr xmlPtr;
	LogMsg( INFO, "Creation of %s", GetFileName().c_str() );

	// Create new XML Document
	xmlPtr = xmlNewDoc( BAD_CAST "1.0" );
	xmlNodePtr root_node = ToXMLNode("player");
	xmlDocSetRootElement(xmlPtr, root_node);

	xmlSaveFormatFileEnc( GetFileName().c_str(), xmlPtr, "ISO-8859-1", 1);

	// Update and Save this player's info in the master players list.
	Players::Instance()->GetPlayerInfo( GetName() )->Update( this, simulation );
	Players::Instance()->Save();
}

/**\brief Parse one player out of an xml node
 */
bool Player::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;
	Coordinate pos;

	if( (attr = FirstChildNamed(node,"name")) ){
		SetName(NodeToString(doc,attr));
	}

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

	for( attr = FirstChildNamed(node,"favor"); attr!=NULL; attr = NextSiblingNamed(attr,"favor") ){
		xmlNodePtr alliance = FirstChildNamed(attr,"alliance");
		xmlNodePtr value = FirstChildNamed(attr,"value");
		if(!alliance || !value)
			return false;
		if( NodeToInt(doc,value) > 0 )
		{
			UpdateFavor( NodeToString(doc,alliance), NodeToInt(doc,value) );
		}
	}

	for( attr = FirstChildNamed(node,"hiredEscort"); attr!=NULL; attr = NextSiblingNamed(attr,"hiredEscort") ){
		xmlNodePtr typePtr = FirstChildNamed(attr, "type");
		xmlNodePtr payPtr = FirstChildNamed(attr, "pay");
		assert(typePtr && payPtr);
		if(!typePtr || !payPtr) return false;
		string type = NodeToString(doc, typePtr);
		int pay = atoi( NodeToString(doc, payPtr).c_str() );
		// Adding it with sprite ID -1 means it's up to player.lua to go ahead and create the correct sprite.
		this->AddHiredEscort(type, pay, -1);
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

	RemoveLuaControlFunc();

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
		WeaponSlot *existingSlot = NULL;

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

			Weapon* weapon = Weapons::Instance()->GetWeapon( value );
			existingSlot->content = weapon;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"firingGroup")) ){
			value = NodeToString(doc,attr);
			existingSlot->firingGroup = (short)atoi(value.c_str());
		} else return false;

	}

	// no need to push any WeaponSlot back into the player's weaponSlots; slots were edited in place

	return true;
}



/**\brief Save this Player to an xml node
 */
xmlNodePtr Player::ToXMLNode(string componentName) {
	char buff[256];
	char *timestamp;
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	// Version information
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MAJOR);
	xmlNewChild(section, NULL, BAD_CAST "version-major", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MINOR);
	xmlNewChild(section, NULL, BAD_CAST "version-minor", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MICRO);
	xmlNewChild(section, NULL, BAD_CAST "version-macro", BAD_CAST buff);

	// Player Stats
	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST GetName().c_str() );

	xmlNodePtr planet = xmlNewNode(NULL, BAD_CAST "planet" );
	xmlNewChild(planet, NULL, BAD_CAST "name", BAD_CAST lastPlanet.c_str()); 
	snprintf(buff, sizeof(buff), "%d", (int)GetWorldPosition().GetX() );
	xmlNewChild(planet, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetWorldPosition().GetY() );
	xmlNewChild(planet, NULL, BAD_CAST "y", BAD_CAST buff );
	xmlAddChild(section,planet);
	
	xmlNewChild(section, NULL, BAD_CAST "model", BAD_CAST GetModelName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST GetEngineName().c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetCredits() );
	xmlNewChild(section, NULL, BAD_CAST "credits", BAD_CAST buff );

	// this part is becoming less important and may be removed at some point
	for(unsigned int i = 0; i < weaponSlots.size(); i++){
		xmlNewChild(section, NULL, BAD_CAST "weapon", BAD_CAST GetWeaponSlotContent(i).c_str() );
	}

	// Ammo
	for(int a=0;a<max_ammo;a++){
		if(GetAmmo(AmmoType(a)) != 0 ){ // Don't save empty ammo Nodes
			snprintf(buff, sizeof(buff), "%d", GetAmmo(AmmoType(a)) );

			xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "ammo");
			xmlNewChild(ammo, NULL, BAD_CAST "type", BAD_CAST Weapon::AmmoTypeToName((AmmoType)a).c_str() );
			xmlNewChild(ammo, NULL, BAD_CAST "amount", BAD_CAST buff );
			xmlAddChild(section, ammo);
		}
	}

	// Weapon Slots
	// save info about whichever items players are able to change in their slot configuration (content and firing group)
	for(unsigned int w=0; w < weaponSlots.size(); w++){
		WeaponSlot *slot = &weaponSlots[w];
		xmlNodePtr slotPtr = xmlNewNode(NULL, BAD_CAST "weapSlot");

		xmlNewChild(slotPtr, NULL, BAD_CAST "name", BAD_CAST GetWeaponSlotName(w).c_str() );
		xmlNewChild(slotPtr, NULL, BAD_CAST "content", BAD_CAST GetWeaponSlotContent(w).c_str() );

		snprintf(buff, sizeof(buff), "%d", slot->firingGroup);
		xmlNewChild(slotPtr, NULL, BAD_CAST "firingGroup", BAD_CAST buff);
		xmlAddChild(section, slotPtr); // saved player data is less structured than model data, so just add it here
	}

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

	// Favor
	map<Alliance*,int>::iterator iter_favor;
	for(iter_favor = favor.begin(); iter_favor!=favor.end(); ++iter_favor) {
		if( !(*iter_favor).second )
		{
			continue; // Don't Save empty favor Nodes
		}
		snprintf(buff, sizeof(buff), "%d", (*iter_favor).second );
		xmlNodePtr ammo = xmlNewNode(NULL, BAD_CAST "favor");
		xmlNewChild(ammo, NULL, BAD_CAST "alliance", BAD_CAST ((*iter_favor).first)->GetName().c_str() );
		xmlNewChild(ammo, NULL, BAD_CAST "value", BAD_CAST buff );
		xmlAddChild(section, ammo);
	}

	// Hired escorts
	for(list<HiredEscort*>::iterator iter_escort = hiredEscorts.begin(); iter_escort != hiredEscorts.end(); iter_escort++){
		// Check that the sprite hasn't already been destroyed. (If it has, leave it out.)
		///\todo We should remove the SpriteManager reference here to remove a dependency on global variables.
		if(
		   (*iter_escort)->spriteID != -1 &&
		   SpriteManager::Instance()->GetSpriteByID(
		      (*iter_escort)->spriteID
		   ) != NULL
		){
			xmlNodePtr hePtr = xmlNewNode(NULL, BAD_CAST "hiredEscort");
			xmlNewChild(hePtr, NULL, BAD_CAST "type", BAD_CAST (*iter_escort)->type.c_str() );
			snprintf(buff, sizeof(buff), "%d", (*iter_escort)->pay);
			xmlNewChild(hePtr, NULL, BAD_CAST "pay", BAD_CAST buff);
			xmlAddChild(section, hePtr);
			// Don't include spriteID in the XML file, for obvious reasons.
		}
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

/**\brief Record that a Hired Escort
 * \details The Record does not get added if the Escort is already attached.
 * \param[in] type The Model name of the Escort.
 * \param[in] pay The Cost per day.
 *            0 Is acceptable.
 *            -1 means "don't check / don't alter".
 * \param[in] spriteID The ID of the Escort.
 *            spriteID should be -1 if the Escort has not been created yet.
 */
void Player::AddHiredEscort(string type, int pay, int spriteID){

	for(
	   list<HiredEscort*>::iterator it = hiredEscorts.begin();
	   it != hiredEscorts.end() && spriteID != -1; // if specified ID is -1, skip this loop
	   it++
	){
		// already have it listed after loading from XML; just update the sprite ID now that it has been created
		if( (*it)->type == type && (*it)->spriteID == -1 ){
			(*it)->spriteID = spriteID;
			return;
		}
	}

	// don't have it yet; add a new entry
	this->hiredEscorts.push_back( new HiredEscort(type, (pay > 0 ? pay : 0), spriteID) );
	//LogMsg(WARN, "Could not find the escort to update");
}

/**\class Player::HiredEscort
 * \brief A record of an escort attached to this player.
 * \details The majority of the escort code is on the Lua side,
 *          but this is used to save and restore Escorts across
 *          saved games.
 */

/**\brief Constructor for HiredEscort
 * \param[in] type The Model name of the Escort.
 * \param[in] pay Unused? -1 means "don't check / don't alter".
 * \param[in] spriteID The ID of the Escort.
 *            spriteID should be -1 if the Escort has not been created yet.
 */
Player::HiredEscort::HiredEscort(string _type, int _pay, int _spriteID){
	type = _type;
	pay = _pay;
	spriteID = _spriteID;
}

/**\brief Initialize the Lua AI state of a Hired Escort
 * This function which interacts with Lua may be seen as analogous to Mission::Accept()
 */
void Player::HiredEscort::Lua_Initialize(int playerID, Coordinate playerPos){
	char *command = (char*)malloc(256);
	// Need to specify player ID and position because the
	// player object can't be examined from Lua yet.
	snprintf(command, 256, "initHiredEscort(%d, %f, %f, '%s', %d)", playerID, playerPos.GetX(), playerPos.GetY(), this->type.c_str(), this->pay);
	int returns = Lua::Run(command, true);
	free(command);
	lua_State *L = Lua::CurrentState();
	if(returns > 0){
		this->spriteID = luaL_checkint(L, -1);
		lua_pop(L, returns);
	}
}

/**\class PlayerInfo
 * \brief Collection of Player objects
 */

/**\brief Construct a blank PlayerInfo
 */
PlayerInfo::PlayerInfo()
	:file("")
	,lastLoadTime((time_t)0) // January 1, 1970
{
	avatar = NULL;
}

/**\brief Construct the PlayerInfo from a Player
 * \param[in] player The player instance that this PlayerInfo will represent.
 */
PlayerInfo::PlayerInfo( Player* player, string simulation )
{
	Update( player, simulation );
}

/**\brief Construct the PlayerInfo from attributes
 */
PlayerInfo::PlayerInfo(  string _name, string _simulation, int _seed )
    :avatar(NULL)
    ,file("")
    ,simulation(_simulation)
    ,seed(_seed)
    ,lastLoadTime(0)
{
    SetName(_name);
}

/**\brief Update the Player Information based on a Player
 * \param[in] player The player instance that this PlayerInfo will represent.
 */
void PlayerInfo::Update( Player* player, string simName ) {
	name = player->GetName();
	avatar = (player->GetModel() != NULL) ? player->GetModel()->GetImage() : NULL;
	file = player->GetFileName();
	simulation = simName;
	seed = OPTION(int, "options/simulation/random-seed");
	lastLoadTime = player->GetLoadTime();
}

/**\brief Extract this PlayerInfo from an XML Node
 * \param[in] doc The XML document.
 * \param[in] xnode The XML Node.
 * \returns true if the PlayerInfo was loaded correctly.
*/
bool PlayerInfo::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;

	// If the node has a Model then it is using the old format.
	// Create that player's XML File before continuing
	if( (attr = FirstChildNamed(node,"model")) ) {
		node = ConvertOldVersion( doc, node );
	}

	// The file attribute is the saved game xml file.
	if( (attr = FirstChildNamed(node,"file")) ) {
		file = NodeToString(doc,attr);
		if( File::Exists( file ) == false ) {
			LogMsg(ERR, "Player %s is Corrupt. There is no file '%s'.", name.c_str(), file.c_str() );
			return false;
		}
	} else {
		LogMsg(ERR, "Player %s is Corrupt. There is no file attribute.", name.c_str() );
		return false;
	}

	// A corrupt avatar isn't fatal, just don't try to draw it.
	if( (attr = FirstChildNamed(node,"avatar")) ){
		avatar = Image::Get( NodeToString(doc,attr) );
		if( avatar == NULL ) {
			LogMsg(WARN, "Player %s has a corrupt avatar.  There is no image '%s' ", name.c_str(), NodeToString(doc,attr).c_str() );
		}
	}

	if( (attr = FirstChildNamed(node,"simulation")) ){
		simulation = NodeToString(doc,attr);
	} else {
		simulation = "default";
	}

	if( (attr = FirstChildNamed(node,"seed")) ){
		seed = NodeToInt(doc,attr);
	} else {
		seed = 0;
	}

	// A corrupt lastLoadTime isn't fatal, just use January 1, 1970.
	if( (attr = FirstChildNamed(node,"lastLoadTime")) ){
		lastLoadTime = NodeToInt(doc,attr);
	} else {
		lastLoadTime = (time_t)0;
	}

	return true;
}

/**\brief Generate an XMLNode of this PlayerInfo
 * \param[in] componentName This should always be "player".
 * \return A new XML node that represents the PlayerInfo.
 */
xmlNodePtr PlayerInfo::ToXMLNode(string componentName) {
	char buff[256];
	char *timestamp;
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST name.c_str() );
	xmlNewChild(section, NULL, BAD_CAST "file", BAD_CAST file.c_str() );
	if( (avatar != NULL) && (avatar->GetPath() != "") ) {
		xmlNewChild(section, NULL, BAD_CAST "avatar", BAD_CAST avatar->GetPath().c_str() );
	}

	xmlNewChild(section, NULL, BAD_CAST "simulation", BAD_CAST simulation.c_str() );
	snprintf(buff, sizeof(buff), "%d", seed );
	xmlNewChild(section, NULL, BAD_CAST "seed", BAD_CAST buff );

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

/**\brief Convert an XML node from the old saved-games.xml format to the new format.
 * \details
 *   The old saved-games.xml format put all of the Player information in one file.
 *   The new saved-games.xml format only stores some Player information, but
 *   nothing that relies on loading the Simulation.  Everything in the old
 *   style format is by itself in a standalone xml file named after the player.
 * \todo This could save a copy of the old saved-games.xml to a backup location.
 * \param[in] doc The XML document.
 * \param[in] xnode The XML Node.
 * \return A new XML node that represents the PlayerInfo for the Player.
 */
xmlNodePtr PlayerInfo::ConvertOldVersion( xmlDocPtr doc, xmlNodePtr node ) {
	char buff[256];
	xmlDocPtr xmlPtr;
	xmlNodePtr  attr;
	xmlNodePtr  copy = xmlCopyNode( node, 1);
	string filename = "Resources/Definitions/"+ name +".xml";

	LogMsg(INFO, "Converting %s to an xml file: %s ", name.c_str(), filename.c_str() );

	xmlPtr = xmlNewDoc( BAD_CAST "1.0" );
	xmlDocSetRootElement(xmlPtr, copy);

	// Version information
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MAJOR);
	xmlNewChild(copy, NULL, BAD_CAST "version-major", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MINOR);
	xmlNewChild(copy, NULL, BAD_CAST "version-minor", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MICRO);
	xmlNewChild(copy, NULL, BAD_CAST "version-macro", BAD_CAST buff);

	xmlSaveFormatFileEnc( filename.c_str(), xmlPtr, "ISO-8859-1", 1);

	xmlNodePtr new_node = xmlNewNode(NULL, BAD_CAST "player");
	xmlNewChild(new_node, NULL, BAD_CAST "name", BAD_CAST GetName().c_str() );
	xmlNewChild(new_node, NULL, BAD_CAST "file", BAD_CAST filename.c_str() );

	if( (attr = FirstChildNamed(copy, "lastLoadTime")) ){
		xmlNewChild(new_node, NULL, BAD_CAST "lastLoadTime", BAD_CAST NodeToString(doc,attr).c_str() );
	}
	
	return new_node;
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
		pInstance = new Players; // create the solid instance
		pInstance->rootName = "players";
		pInstance->componentName = "player";
	}
	return( pInstance );
}

/**\brief Create a new Player
 * This is used instead of a normal class constructor
 */
Player* Players::CreateNew(
            string simulation,
            string playerName,
			Model *model,
			Engine *engine,
			int credits,
			Coordinate location)
{
	Player* newPlayer = new Player;

	LogMsg(INFO, "Creating New Player '%s' with Model='%s' Engine='%s' Credits = %d at (%.0f,%.0f).",
		playerName.c_str(),
		model->GetName().c_str(),
		engine->GetName().c_str(),
		credits,
		location.GetX(), location.GetY()
	);

	newPlayer->name = playerName;

	newPlayer->SetModel( model );
	newPlayer->SetEngine( engine );
	newPlayer->SetCredits( credits );
	newPlayer->SetWorldPosition( location );
	newPlayer->RemoveLuaControlFunc();

	newPlayer->lastLoadTime = time(NULL);

	Add( (Component*)(new PlayerInfo( newPlayer, simulation )) );

	return newPlayer;
}

/**\brief Deletes a player
 */
bool Players::DeletePlayer(string playerName) {
	bool ret;

	// remove player from Players list
	PlayerInfo* info = GetPlayerInfo( playerName );
	if( (ret = Remove( (Component*)info ) ) == false) {
		LogMsg(ERR, "Could not remove player!\n");
		return false;
	}

	// save players compoent
	if( Save() == false) {
		LogMsg(ERR, "Removed player from list but could not save list.\n");
		return false;
	}
	
	// delete the separate player xml
	string filename = "Resources/Definitions/" + playerName + ".xml";
	if( Filesystem::DeleteFile( filename ) != true ) {
		LogMsg(ERR, "Could not remove player XML file.\n");
		return false;
	}

	return true;
}

/**\brief Returns true if a player already exists
 */
bool Players::PlayerExists(string playerName) {
	list<string>* names = GetNames();
	list<string>::iterator i;

	if( names->empty() ) return false;

	for(i = names->begin(); i != names->end(); ++i ) {
		if(*i == playerName) return true;
	}

	return false;
}

/**\brief Get the PlayerInfo for the most recent player
 */
PlayerInfo* Players::LastPlayer() {
	list<string>* names = GetNames();
	list<string>::iterator i;
	PlayerInfo* latest = NULL;

	if( names->empty() ){
		return NULL;
	}

	for(i = names->begin(); i != names->end(); ++i ) {
		if( (latest == NULL) || (latest->lastLoadTime < GetPlayerInfo(*i)->lastLoadTime ) ) {
			latest = GetPlayerInfo(*i);
		}
	}

	return latest;
}

/**\brief Load a given Player
 */
Player* Players::LoadPlayer(string playerName) {
	PlayerInfo* info = GetPlayerInfo( playerName );
	Player* newPlayer = Player::Load( info->file );
	return newPlayer;
}

/** @} */

