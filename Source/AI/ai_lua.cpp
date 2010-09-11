/**\file			ai_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Monday, November 16 2009
 * \brief			Lua Bridge for AI objects
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Utilities/lua.h"
#include "Sprites/effects.h"
#include "Sprites/player.h"
#include "AI/ai_lua.h"
#include "Audio/sound.h"
#include "Utilities/camera.h"
#include "Utilities/trig.h"
#include "Engine/commodities.h"

/**\class AI_Lua
 * \brief Lua bridge for AI.*/

/**\brief Registers functions callable by Lua scripts for the AI.
 */
void AI_Lua::RegisterAI(lua_State *L){

	// This is the Function for creating Ships
	// Call this like:
	// some_ship = Epiar.Ship.new()
	static const luaL_Reg shipFunctions[] = {
		{"new", &AI_Lua::newShip},
		{NULL, NULL}
	};

	// These are the Ship we're supporting in Lua
	// Call these like:
	// some_ship:Accelerate()
	static const luaL_Reg shipMethods[] = {
		// Actions
		{"Accelerate", &AI_Lua::ShipAccelerate},
		{"Rotate", &AI_Lua::ShipRotate},
		{"SetRadarColor", &AI_Lua::ShipRadarColor},
		{"Fire", &AI_Lua::ShipFire},
		{"Damage", &AI_Lua::ShipDamage},
		{"Repair", &AI_Lua::ShipRepair},
		{"Explode", &AI_Lua::ShipExplode},
		{"Remove", &AI_Lua::ShipRemove},
		{"ChangeWeapon", &AI_Lua::ShipChangeWeapon},
		
		//Power Distribution
		{"GetShieldBooster", &AI_Lua::ShipGetShieldBooster},
		{"GetEngineBooster", &AI_Lua::ShipGetEngineBooster},
		{"GetDamageBooster", &AI_Lua::ShipGetDamageBooster},		
		{"SetShieldBooster", &AI_Lua::ShipSetShieldBooster},
		{"SetEngineBooster", &AI_Lua::ShipSetEngineBooster},
		{"SetDamageBooster", &AI_Lua::ShipSetDamageBooster},
		
		// Outfit Changes
		{"AddWeapon", &AI_Lua::ShipAddWeapon},
		{"AddAmmo", &AI_Lua::ShipAddAmmo},
		{"SetModel", &AI_Lua::ShipSetModel},
		{"SetEngine", &AI_Lua::ShipSetEngine},
		{"AddOutfit", &AI_Lua::ShipAddOutfit},
		{"SetCredits", &AI_Lua::ShipSetCredits},
		{"StoreCommodities", &AI_Lua::ShipStoreCommodities},
		{"DiscardCommodities", &AI_Lua::ShipDiscardCommodities},
		{"AcceptMission", &AI_Lua::ShipAcceptMission},

		// Current State
		{"GetID", &AI_Lua::ShipGetID},
		{"GetType", &AI_Lua::ShipGetType},
		{"GetAngle", &AI_Lua::ShipGetAngle},
		{"GetPosition", &AI_Lua::ShipGetPosition},
		{"GetMomentumAngle", &AI_Lua::ShipGetMomentumAngle},
		{"GetMomentumSpeed", &AI_Lua::ShipGetMomentumSpeed},
		{"directionTowards", &AI_Lua::ShipGetDirectionTowards},
		{"GetCurrentWeapon", &AI_Lua::ShipGetCurrentWeapon},
		{"GetCurrentAmmo", &AI_Lua::ShipGetCurrentAmmo},

		// General State
		{"GetModelName", &AI_Lua::ShipGetModelName},
		{"GetEngine", &AI_Lua::ShipGetEngine},
		{"GetHull", &AI_Lua::ShipGetHull},
		{"GetShield", &AI_Lua::ShipGetShield},
		{"GetWeapons", &AI_Lua::ShipGetWeapons},
		{"GetState", &AI_Lua::ShipGetState},
		{"GetCredits", &AI_Lua::ShipGetCredits},
		{"GetCargo", &AI_Lua::ShipGetCargo},
		{"GetOutfits", &AI_Lua::ShipGetOutfits},
		{"GetTotalCost", &AI_Lua::ShipGetTotalCost},
		{"IsDisabled", &AI_Lua::ShipIsDisabled},

		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_SHIP);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, shipMethods, 0);

	luaL_openlib(L, EPIAR_SHIP, shipFunctions, 0);

	// This may not be the best place for this line,
	// preloading this animation prevents an FPS
	// drop the first time that a ship explodes.
	Ani::Get("Resources/Animations/explosion1.ani");
}

/**\brief Validates Ship in Lua.
 */
AI* AI_Lua::checkShip(lua_State *L, int index){
	int* idptr = (int*)luaL_checkudata(L, index, EPIAR_SHIP);
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_SHIP' expected");
	Sprite* s;
	s = SpriteManager::Instance()->GetSpriteByID(*idptr);
	/*
	if ((s) == NULL) luaL_typerror(L, index, EPIAR_SHIP);
	if (0==((s)->GetDrawOrder() & DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER)){
		luaL_typerror(L, index, EPIAR_SHIP);
	}
	*/
	return (AI*)s;
}

/**\brief Spawns a new AI ship for Lua.
 */
int AI_Lua::newShip(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5)
		return luaL_error(L, "Got %d arguments expected 5 (x, y, model, engine, script)", n);

	double x = luaL_checknumber (L, 1);
	double y = luaL_checknumber (L, 2);
	string modelname = luaL_checkstring (L, 3);
	string enginename = luaL_checkstring (L, 4);
	string statemachine = luaL_checkstring (L, 5);

	//LogMsg(INFO,"Creating new Ship (%f,%f) (%s) (%s) (%s)",x,y,modelname.c_str(),enginename.c_str(),statemachine.c_str());

	// Allocate memory for a pointer to object
	AI* s;
	s = new AI(statemachine);
	s->SetWorldPosition( Coordinate(x, y) );
	s->SetModel( Models::Instance()->GetModel(modelname) );
	s->SetEngine( Engines::Instance()->GetEngine(enginename) );
	Lua::pushSprite(L,s);

	// Add this ship to the SpriteManager
	SpriteManager::Instance()->Add((Sprite*)(s));

	return 1;
}

// Ship Functions

// Ship Actions

/**\brief Lua callable function to accelerate the ship.
 * \sa Ship::Accelerate()
 */
int AI_Lua::ShipAccelerate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		luaL_argcheck(L, ai != NULL, 1, "`array' expected");
		(ai)->Accelerate();
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n);

	return 0;
}

/**\brief Lua callable function to rotate the ship.
 * \sa Ship::Rotate(float)
 */
int AI_Lua::ShipRotate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		float dir = static_cast<float>( luaL_checknumber(L, 2) );
		(ai)->Rotate(dir);
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n);

	return 0;
}

/**\brief Lua callable function to set ship's radar color.
 * \sa Sprite::SetRadarColor
 */
int AI_Lua::ShipRadarColor(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 4) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int red = (int) luaL_checknumber (L, 2);
		int green = (int) luaL_checknumber (L, 3);
		int blue = (int) luaL_checknumber (L, 4);
		(ai)->SetRadarColor(Color::Get(red,green,blue));
	} else {
		luaL_error(L, "Got %d arguments expected 4 (self, red, green, blue)", n);
	}
	return 0;
}

/**\brief Lua callable function to fire ship's weapons.
 * \sa Ship::Fire()
 */
int AI_Lua::ShipFire(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	int target = -1;
	if( (n == 1) || (n == 2) ) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		if(n == 2)
		{
			target = luaL_checkinteger(L,2);
		}
		FireStatus result = (ai)->Fire(target);
		lua_pushinteger(L, (int)(result) );
		return 1;
	} else {
		luaL_error(L, "Got %d arguments expected 1 or 2 (ship, [target])", n);
	}
	return 0;
}

/**\brief Lua callable function to add damage to ship.
 * \sa Ship::Damage(short int)
 */
int AI_Lua::ShipDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int damage = (int) luaL_checknumber (L, 2);
		(ai)->Damage( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}

/**\brief Lua callable function to repair the ship.
 */
int AI_Lua::ShipRepair(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int damage = luaL_checkint (L, 2);
		(ai)->Repair( damage );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, damage)", n);
	}
	return 0;
}

/**\brief Lua callable function to explode the ship.
 * \sa Effect
 */
int AI_Lua::ShipExplode(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		LogMsg(INFO,"A %s Exploded!",(ai)->GetModelName().c_str());
		// Play explode sound
		Sound *explodesnd = Sound::Get("Resources/Audio/Effects/18384__inferno__largex.wav.ogg");
		if(OPTION(int, "options/sound/explosions"))
			explodesnd->Play(
				(ai)->GetWorldPosition() - Camera::Instance()->GetFocusCoordinate());
		SpriteManager::Instance()->Add(
			new Effect((ai)->GetWorldPosition(), "Resources/Animations/explosion1.ani", 0) );
		SpriteManager::Instance()->Delete((Sprite*)(ai));
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}
	return 0;
}

/**\brief Lua callable function to remove this ship
 *
 *  This does not create an explosion, the ship is simply removed instantly.
 */
int AI_Lua::ShipRemove(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		SpriteManager::Instance()->Delete((Sprite*)(ai));
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}
	return 0;
}

/**\brief Lua callable function to add weapon to ship.
 * \sa Ship::addShipWeapon(string)
 */
int AI_Lua::ShipAddWeapon(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		(ai)->AddShipWeapon(weaponName);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, weaponName)", n);
	}
	return 0;
}	

/**\brief Lua callable function to change a ship's weapon.
 * \sa Ship::ChangeWeapon()
 */
int AI_Lua::ShipChangeWeapon(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		(ai)->ChangeWeapon();
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n);
	}
	return 0;
}

/**\brief Lua callable function to retrieve shield boost data
 *\sa Ship::GetShieldBoost
 */
int AI_Lua::ShipGetShieldBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		double s=(double) (ai)->GetShieldBoost();
		printf( "GetShieldBooster= %f \n", (double) (ai)->GetShieldBoost());
		lua_pushnumber (L, s);

	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to retrieve damage boost data
 *\sa Ship::GetDamageBoost
 */
int AI_Lua::ShipGetDamageBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushnumber (L, (double) (ai)->GetDamageBoost());
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to retrieve engine boost data
 *\sa Ship::GetEngineBoost
 */
int AI_Lua::ShipGetEngineBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==1){
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushnumber (L, (double) (ai)->GetEngineBoost());
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 argument (ship)",n);
	}
	return 1;
}

/**\brief Lua callable function to set shield boost data
 *\sa Ship::SetShieldBoost
 */
int AI_Lua::ShipSetShieldBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		AI* ai = checkShip( L, 1 );
		if(ai==NULL) return 0;
		float shield=(float) luaL_checknumber( L, 2 );
		(ai)->SetShieldBoost(shield);
	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship, shield boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to set damage boost data
 *\sa Ship::SetDamageBoost
 */
int AI_Lua::ShipSetDamageBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		float damage=(float) luaL_checknumber( L, 2 ); 
		(ai)->SetDamageBoost(damage);
	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship, damage boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to retrieve engine boost data
 *\sa Ship::SetEngineBoost
 */
int AI_Lua::ShipSetEngineBooster(lua_State* L){
	int n = lua_gettop(L); //Number of arguments
	if (n==2){
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		
		float engine=(float) luaL_checknumber( L, 2 ); 
		(ai)->SetEngineBoost(engine);

	}
	else {
		luaL_error(L, "Got %d arguments expected 2 arguments (ship ,engine boost)",n);
	}
	return 0;
}

/**\brief Lua callable function to add ammo to a ship's weapon.
 * \sa Ship::addAmmo
 * \todo This should be passed an Ammo Type, not a weapon name
 */
int AI_Lua::ShipAddAmmo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 3) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string weaponName = luaL_checkstring (L, 2);
		int qty = (int) luaL_checknumber (L, 3);

		Weapon* weapon = Weapons::Instance()->GetWeapon(weaponName);
		if(weapon==NULL){
			return luaL_error(L, "There is no such weapon as a '%s'", weaponName.c_str());
		}
		(ai)->AddAmmo(weapon->GetAmmoType(),qty);
	} else {
		luaL_error(L, "Got %d arguments expected 3 (ship, weaponName, qty)", n);
	}
	return 0;
}	

/**\brief Lua callable function to set the ship model.
 * \sa Ship::SetModel
 */
int AI_Lua::ShipSetModel(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string modelname = luaL_checkstring (L, 2);
		(ai)->SetModel( Models::Instance()->GetModel(modelname) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, modelname)", n);
	}
	return 0;

}

/**\brief Lua callable function to set the ship engine.
 * \sa Ship::SetEngine(Engine*)
 */
int AI_Lua::ShipSetEngine(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string engineName = luaL_checkstring (L, 2);
		(ai)->SetEngine( Engines::Instance()->GetEngine(engineName) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, engineName)", n);
	}
	return 0;
}

/**\brief Lua callable function to add an Outfit to a ship.
 * \sa Ship::addOutfit(Outfit*)
 */
int AI_Lua::ShipAddOutfit(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		string outfitName = luaL_checkstring (L, 2);
		(ai)->AddOutfit( Outfits::Instance()->GetOutfit(outfitName) );
		printf("Adding outfit: %s\n",outfitName.c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, outfitName)", n);
	}
	return 0;
}

/**\brief Lua callable function to set the credits for this ship
 * \sa Ship::SetCredits()
 */
int AI_Lua::ShipSetCredits(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		int credits = luaL_checkint (L, 2);
		(ai)->SetCredits( credits );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, engineName)", n);
	}
	return 0;
}

/**\brief Lua callable function to Store a number of Commodities on this ship
 * \sa Ship::StoreCommodities()
 */
int AI_Lua::ShipStoreCommodities(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 3) {
		return luaL_error(L, "Got %d arguments expected 2 (ship, commodityName, count)", n);
	}

	// Get the Inputs
	AI* ai = checkShip(L,1);
	string commodityName = luaL_checkstring (L, 2);
	int count = luaL_checkint (L, 3);

	LogMsg(INFO, "Storing %d tons of %s.", count, commodityName.c_str());

	// Check Inputs
	if(ai==NULL) { return 0; }
	if(0==Commodities::Instance()->GetCommodity(commodityName)){
		return luaL_error(L, "There is no Commodity by the name of '%s'", commodityName.c_str());
	}

	// Store the Commodity
	int actuallyStored = (ai)->StoreCommodities( commodityName, count );
	lua_pushinteger(L, actuallyStored );
	return 1;
}

/**\brief Lua callable function to Discard a number of Commodities from this ship
 * \sa Ship::DiscardCommodities()
 */
int AI_Lua::ShipDiscardCommodities(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 3) {
		return luaL_error(L, "Got %d arguments expected 2 (ship, commodityName, count)", n);
	}

	// Get the Inputs
	AI* ai = checkShip(L,1);
	string commodityName = luaL_checkstring (L, 2);
	int count = luaL_checkint (L, 3);

	LogMsg(INFO, "Discarding %d tons of %s.", count, commodityName.c_str());

	// Check Inputs
	if(ai==NULL) { return 0; }
	if(0==Commodities::Instance()->GetCommodity(commodityName)){
		return luaL_error(L, "There is no Commodity by the name of '%s'", commodityName.c_str());
	}

	// Discard the Commodity
	int actuallyDiscarded = (ai)->DiscardCommodities( commodityName, count );
	lua_pushinteger(L, actuallyDiscarded );
	return 1;
}

int AI_Lua::ShipAcceptMission(lua_State *L){
	int n=lua_gettop(L);
	if(n!=3){
		return luaL_error(L, "%d arguments provided, but expected 2 (self, MissionType, MissionTable)");
	}

	// Check that only players accept missions
	Ship* ship = checkShip(L,1);
	if( ship->GetDrawOrder() != DRAW_ORDER_PLAYER ) {
		return luaL_error(L, "Only Players may accept Missions");
	}
	Player *player = (Player*)ship;
	
	// Get and Validate the Mission Information
	string missionType = (string) luaL_checkstring(L,2);
	int missionTable = luaL_ref(L, LUA_REGISTRYINDEX); // Gets and pops the top of the stack, which should have the the missionTable.
	if( Mission::ValidateMission( missionType, missionTable ) ) {
		Mission *mission = new Mission( missionType, missionTable );
		player->AcceptMission( mission );
	} else {
		return luaL_error(L, "The Mission Type '%s' or the Mission Table is invalid.", missionType.c_str() );
	}
	return 0;
}


/**\brief Lua callable function to get the Type of this ship
 *
 * This is usually used by Lua prior to casting a generic sprite as a Ship.
 *
 * \sa Sprite::GetDrawOrder()
 */

int AI_Lua::ShipGetType(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL) return 0;
		lua_pushinteger(L, (ai)->GetDrawOrder() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's ID
 * \sa Sprite::GetID()
 */
int AI_Lua::ShipGetID(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushinteger(L, (ai)->GetID() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's angle.
 * \sa Sprite::GetAngle()
 */
int AI_Lua::ShipGetAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) normalizeAngle( (ai)->GetAngle() ) );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's position.
 * \sa Coordinate::GetWorldPosition()
 */
int AI_Lua::ShipGetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L,0);
			lua_pushnumber(L,0);
		} else {
			lua_pushnumber(L, (double) (ai)->GetWorldPosition().GetX() );
			lua_pushnumber(L, (double) (ai)->GetWorldPosition().GetY() );
		}
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Lua callable function to get the ship's momentum angle.
 * \sa Sprite::GetMomentum()
 * \sa Sprite::GetAngle()
 */
int AI_Lua::ShipGetMomentumAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai= checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (double) (ai)->GetMomentum().GetAngle() );
		}
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's momentum speed.
 * \sa Sprite::GetMomentum()
 * \sa Coordinate::GetMagnitude()
 */
int AI_Lua::ShipGetMomentumSpeed(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
		lua_pushnumber(L, (double) (ai)->GetMomentum().GetMagnitude() );
		}
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's direction towards target.
 * \sa Ship::directionTowards
 */
int AI_Lua::ShipGetDirectionTowards(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) { // Angle
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
		float angle = static_cast<float>( luaL_checknumber(L, 2) );
		lua_pushnumber(L, (double) (ai)->GetDirectionTowards(angle) );
		}
	}
	else if(n==3){ // Coordinate
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
		double x = static_cast<float>( luaL_checknumber(L, 2) );
		double y = static_cast<float>( luaL_checknumber(L, 3) );
		lua_pushnumber(L, (double) (ai)->GetDirectionTowards(Coordinate(x,y)) );
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's weapons.
 * \sa Ship::getWeaponsAndAmmo()
 */
int AI_Lua::ShipGetWeapons(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}

	map<Weapon*,int> weaponPack = (ai)->GetWeaponsAndAmmo();
	map<Weapon*,int>::iterator it = weaponPack.begin();

	lua_createtable(L, weaponPack.size(), 0);
	int newTable = lua_gettop(L);
	while( it!=weaponPack.end() ) {
		lua_pushfstring(L, ((*it).first)->GetName().c_str() ); // KEY
		lua_pushinteger(L, (*it).second ); // Value
		lua_settable(L,newTable);
		++it;
	}
	return 1;
}

/**\brief Lua callable function to get the current weapon.
 * \sa Ship::getCurrentWeapon()
 */
int AI_Lua::ShipGetCurrentWeapon(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}
	Weapon* cur = (ai)->GetCurrentWeapon();
	lua_pushfstring(L, cur?cur->GetName().c_str():"" );
	return 1;
}

/**\brief Lua callable function to get the current ammo.
 * \sa Ship::getCurrentAmmo()
 */
int AI_Lua::ShipGetCurrentAmmo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}
	lua_pushnumber(L, (ai)->GetCurrentAmmo() );
	return 1;
}

/**\brief Lua callable function to get the ship's model name.
 * \sa Ship::GetModelName()
 */
int AI_Lua::ShipGetModelName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}
	lua_pushfstring(L, ((ai)->GetModelName()).c_str() );
	return 1;
}

/**\brief Lua callable function to get the ship's engine name.
 * \sa Ship::GetEngine()
 */
int AI_Lua::ShipGetEngine(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}
	lua_pushfstring(L, ((ai)->GetEngine())->GetName().c_str() );
	return 1;
}

/**\brief Lua callable function to get the hull status (in %).
 * \sa Ship::getHullIntegrityPct
 */
int AI_Lua::ShipGetHull(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			// The ship doesn't exist (anymore?) so it's probably dead
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) (ai)->GetHullIntegrityPct() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the shield status (in %).
 * \sa Ship::GetShieldIntegrityPct
 */
int AI_Lua::ShipGetShield(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			// The ship doesn't exist (anymore?) so it's probably dead
			lua_pushnumber(L, 0 );
			return 1;
		}
		lua_pushnumber(L, (double) (ai)->GetShieldIntegrityPct() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the State Machine of an AI.
 * \sa AI::GetStateMachine
 */
int AI_Lua::ShipGetState(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
			return 1;
		} else if( ai->GetDrawOrder() & DRAW_ORDER_PLAYER ) {
			// We need to do this since the Player doesn't have a StateMachine.
			// Warning! these are not actually valid States or StateMachines
			lua_pushstring(L, "Player" ); // State Machine
			lua_pushstring(L, "Playing" ); // State
		} else {
			lua_pushstring(L, (ai)->GetStateMachine().c_str() );
			lua_pushstring(L, (ai)->GetState().c_str() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Lua callable function to get the current credits.
 * \sa AI::GetStateMachine
 */
int AI_Lua::ShipGetCredits(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->GetCredits() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's stored Commodities.
 * \sa Ship::getCargo()
 */
int AI_Lua::ShipGetCargo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}

	map<Commodity*,unsigned int> cargo = (ai)->GetCargo();
	map<Commodity*,unsigned int>::iterator it = cargo.begin();

	// Create a Lua table and populate it using the form:
	// {"Foo": 10, "Bar": 20}
	lua_createtable(L, cargo.size(), 0);
	int newTable = lua_gettop(L);
	while( it!=cargo.end() ) {
		lua_pushfstring(L, ((*it).first)->GetName().c_str() ); // KEY
		lua_pushinteger(L, (*it).second ); // Value
		lua_settable(L,newTable);
		++it;
	}

	// Push Cargo statistics
	lua_pushinteger(L, (ai)->GetCargoSpaceUsed() ); // Total Tons Stored
	lua_pushinteger(L, Models::Instance()->GetModel((ai)->GetModelName())->GetCargoSpace() ); // Maximum Tons Storable
		
	return 3;
}

/**\brief Lua callable function to get the ship's Outfits.
 *
 * Outfits are returned to Lua as a table of outfit names.
 *
 * \sa Ship::getOutfit()
 */
int AI_Lua::ShipGetOutfits(lua_State* L){
	list<Outfit*>::iterator iter;
	int newTable, tableIndex;
	int n = lua_gettop(L);  // Number of arguments

	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI* ai = checkShip(L,1);
	if(ai==NULL){
		lua_pushnumber(L, 0 );
		return 1;
	}

	list<Outfit*>* outfits = (ai)->GetOutfits();
	lua_createtable(L, outfits->size(), 0);
	newTable = lua_gettop(L);
	for(iter=outfits->begin(), tableIndex=1; iter!=outfits->end(); ++iter, ++tableIndex)
	{
		lua_pushinteger(L, tableIndex ); // Key
		lua_pushfstring(L, (*iter)->GetName().c_str() ); // Value
		lua_settable(L,newTable);
	}

	return 1;
}

/**\brief Lua callable function to get the total cost of the ship.
 * \sa Ship::GetTotalCost
 */
int AI_Lua::ShipGetTotalCost(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->GetTotalCost() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to determine if ship is disabled.
 * \sa Ship::IsDisabled
 */
int AI_Lua::ShipIsDisabled(lua_State* L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI* ai = checkShip(L,1);
		if(ai==NULL){
			lua_pushnumber(L, 0 );
		} else {
			lua_pushnumber(L, (ai)->IsDisabled() );
		}
	} else {
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

