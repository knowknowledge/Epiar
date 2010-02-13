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
#include "AI/ai_lua.h"
#include "Audio/sound.h"
#include "Utilities/camera.h"

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
		{"ChangeWeapon", &AI_Lua::ShipChangeWeapon},
		{"AddWeapon", &AI_Lua::ShipAddWeapon},
		{"AddAmmo", &AI_Lua::ShipAddAmmo},
		{"SetModel", &AI_Lua::ShipSetModel},
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
		{"GetHull", &AI_Lua::ShipGetHull},
		{"GetWeapons", &AI_Lua::ShipGetWeapons},

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

/**\brief Allocates Ship in Lua.
 * \sa newShip
 */
AI **AI_Lua::pushShip(lua_State *L){
	AI **s = (AI **)lua_newuserdata(L, sizeof(AI*));
	*s = NULL;
	luaL_getmetatable(L, EPIAR_SHIP);
	lua_setmetatable(L, -2);
	return s;
}

/**\brief Validates Ship in Lua.
 */
AI **AI_Lua::checkShip(lua_State *L, int index){
	AI **ai = (AI**)luaL_checkudata(L, index, EPIAR_SHIP);
	//luaL_argcheck(L, ai != NULL, index, "`EPIA_SHIP' expected");
	//if (ai == NULL) luaL_typerror(L, index, EPIAR_SHIP);
	return ai;
}

/**\brief Spawns a new AI ship for Lua.
 */
int AI_Lua::newShip(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 4)
		return luaL_error(L, "Got %d arguments expected 4 (x, y, model, script)", n);

	double x = luaL_checknumber (L, 1);
	double y = luaL_checknumber (L, 2);
	string modelname = luaL_checkstring (L, 3);
	string scriptname = luaL_checkstring (L, 4);

	//Log::Message("Creating new Ship (%f,%f) (%s) (%s)",x,y,modelname.c_str(),scriptname.c_str());

	// Allocate memory for a pointer to object
	AI **s = pushShip(L);
	*s = new AI();
	(*s)->SetWorldPosition( Coordinate(x, y) );
	(*s)->SetModel( Models::Instance()->GetModel(modelname) );
	(*s)->SetScript( scriptname );

	// Add this ship to the SpriteManager
	SpriteManager::Instance()->Add((Sprite*)(*s));

	return 1;
}

// Ship Functions

/**\brief Lua callable function to accelerate the ship.
 * \sa Ship::Accelerate()
 */
int AI_Lua::ShipAccelerate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai= (AI**)lua_touserdata(L, 1);
		luaL_argcheck(L, ai != NULL, 1, "`array' expected");
		(*ai)->Accelerate();
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
		AI** ai = checkShip(L,1);
		float dir = static_cast<float>( luaL_checknumber(L, 2) );
		(*ai)->Rotate(dir);
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n); 

	return 0;
}

/**\brief Lua callable function to set ship's radar color.
 * \sa Ship::SetRadarColor
 */
int AI_Lua::ShipRadarColor(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 4) {
		AI** ai = checkShip(L,1);
		int red = (int) luaL_checknumber (L, 2);
		int green = (int) luaL_checknumber (L, 3);
		int blue = (int) luaL_checknumber (L, 4);
		(*ai)->SetRadarColor(Color::Get(red,green,blue));
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
	if (n == 1) {
		AI** ai = checkShip(L,1);
		FireStatus result = (*ai)->Fire();
		lua_pushinteger(L, (int)(result) );
		return 1;
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n); 
	}
	return 0;
}

/**\brief Lua callable function to add damage to ship.
 * \sa Ship::Damage(short int)
 */
int AI_Lua::ShipDamage(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2) {
		AI** ai = checkShip(L,1);
		int damage = (int) luaL_checknumber (L, 2);
		(*ai)->Damage( damage );
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
		AI** ai = checkShip(L,1);
		int damage = luaL_checkint (L, 2);
		(*ai)->Repair( damage );
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
		AI** ai = checkShip(L,1);
		Log::Message("A %s Exploded!",(*ai)->GetModelName().c_str());
		// Play explode sound
		Sound *explodesnd = Sound::Get("Resources/Audio/Effects/18384__inferno__largex.wav.ogg");
		if(OPTION(int, "options/sound/explodes"))
			explodesnd->Play(
				(*ai)->GetWorldPosition() - Camera::Instance()->GetFocusCoordinate());
		SpriteManager::Instance()->Add(
			new Effect((*ai)->GetWorldPosition(), "Resources/Animations/explosion1.ani", 0) );
		SpriteManager::Instance()->Delete((Sprite*)(*ai));
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
		AI** ai = checkShip(L,1);
		string weaponName = luaL_checkstring (L, 2);
		(*ai)->addShipWeapon(weaponName);
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
		AI** ai = checkShip(L,1);
		(*ai)->ChangeWeapon();
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n); 
	}
	return 0;
}

/**\brief Lua callable function to add ammo to a ship's weapon.
 * \sa Ship::addAmmo
 */
int AI_Lua::ShipAddAmmo(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 3) {
		AI** ai = checkShip(L,1);
		string weaponName = luaL_checkstring (L, 2);
		int qty = (int) luaL_checknumber (L, 3);
		(*ai)->addAmmo(weaponName,qty);
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
		AI** ai = checkShip(L,1);
		string modelname = luaL_checkstring (L, 2);
		(*ai)->SetModel( Models::Instance()->GetModel(modelname) );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (ship, modelname)", n); 
	}
	return 0;

}

int AI_Lua::ShipGetType(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushinteger(L, (*ai)->GetDrawOrder() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Lua callable function to get the ship's ID
 * \sa Ship:GetID()
 */
int AI_Lua::ShipGetID(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushinteger(L, (*ai)->GetID() );
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
		AI** ai = checkShip(L,1);
		lua_pushnumber(L, (double) (*ai)->GetAngle() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}

/**\brief Lua callable function to get the ship's position.
 * \sa Ship::GetWorldPosition()
 */
int AI_Lua::ShipGetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushnumber(L, (double) (*ai)->GetWorldPosition().GetX() );
		lua_pushnumber(L, (double) (*ai)->GetWorldPosition().GetY() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 2;
}

/**\brief Lua callable function to get the ship's momentum angle.
 * \sa Ship::GetMomentum()
 * \sa Coordinate::GetAngle()
 */
int AI_Lua::ShipGetMomentumAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai= checkShip(L,1);
		lua_pushnumber(L, (double) (*ai)->GetMomentum().GetAngle() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}

/**\brief Lua callable function to get the ship's momentum speed.
 * \sa Ship::GetMomentum()
 * \sa Coordinate::GetMagnitude()
 */
int AI_Lua::ShipGetMomentumSpeed(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushnumber(L, (double) (*ai)->GetMomentum().GetMagnitude() );
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
		AI** ai = checkShip(L,1);
		float angle = static_cast<float>( luaL_checknumber(L, 2) );
		lua_pushnumber(L, (double) (*ai)->directionTowards(angle) );
	}
	else if(n==3){ // Coordinate
		AI** ai = checkShip(L,1);
		double x = static_cast<float>( luaL_checknumber(L, 2) );
		double y = static_cast<float>( luaL_checknumber(L, 3) );
		lua_pushnumber(L, (double) (*ai)->directionTowards(Coordinate(x,y)) );
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

	AI** ai = checkShip(L,1);

	map<Weapon*,int> weaponPack = (*ai)->getWeaponsAndAmmo();
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

	AI** ai = checkShip(L,1);
	Weapon* cur = (*ai)->getCurrentWeapon();
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

	AI** ai = checkShip(L,1);
	lua_pushnumber(L, (*ai)->getCurrentAmmo() );
	return 1;
}

/**\brief Lua callable function to get the ship's model name.
 * \sa Ship::GetModelName()
 */
int AI_Lua::ShipGetModelName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		luaL_error(L, "Got %d arguments expected 1 (self)", n);

	AI** ai = checkShip(L,1);
	lua_pushfstring(L, ((*ai)->GetModelName()).c_str() );
	return 1;
}

/**\brief Lua callable function to get the hull status (in %).
 * \sa Ship::getHullIntegrityPct
 */
int AI_Lua::ShipGetHull(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushnumber(L, (double) (*ai)->getHullIntegrityPct() );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self)", n);
	}
	return 1;
}
