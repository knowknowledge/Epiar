/**\file			ai_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Monday, November 16 2009
 * \brief			Lua Bridge for AI objects
 * \details
 */

#include "includes.h"
#include "Utilities/lua.h"
#include "Sprites/effects.h"
#include "AI/ai_lua.h"

/**\class AI_Lua
 * \brief Lua bridge for AI.*/

void AI_Lua::RegisterAI(lua_State *L){
	// These are the Ship Functions we're supporting in Lua
	static const luaL_Reg shipFunctions[] = {
		// Creation
		{"new", &AI_Lua::newShip},
		// Actions
		{"Accelerate", &AI_Lua::ShipAccelerate},
		{"Rotate", &AI_Lua::ShipRotate},
		{"SetRadarColor", &AI_Lua::ShipRadarColor},
		{"Fire", &AI_Lua::ShipFire},
		{"Damage", &AI_Lua::ShipDamage},
		{"Explode", &AI_Lua::ShipExplode},
		{"ChangeWeapon", &AI_Lua::ShipChangeWeapon},
		{"AddWeapon", &AI_Lua::ShipAddWeapon},
		{"AddAmmo", &AI_Lua::ShipAddAmmo},
		{"SetModel", &AI_Lua::ShipSetModel},
		// Current State
		{"GetID", &AI_Lua::ShipGetID},
		{"GetAngle", &AI_Lua::ShipGetAngle},
		{"GetPosition", &AI_Lua::ShipGetPosition},
		{"GetMomentumAngle", &AI_Lua::ShipGetMomentumAngle},
		{"GetMomentumSpeed", &AI_Lua::ShipGetMomentumSpeed},
		{"directionTowards", &AI_Lua::ShipGetDirectionTowards},
		// General State
		{"GetModelName", &AI_Lua::ShipGetModelName},
		{"GetHull", &AI_Lua::ShipGetHull},
		{NULL, NULL}
	};
	luaL_newmetatable(L, EPIAR_SHIP);
	luaL_openlib(L, EPIAR_SHIP, shipFunctions,0);  
}

AI **AI_Lua::pushShip(lua_State *L){
	AI **s = (AI **)lua_newuserdata(L, sizeof(AI*));
    *s = NULL;
    luaL_getmetatable(L, EPIAR_SHIP);
    lua_setmetatable(L, -2);
    return s;
}

AI **AI_Lua::checkShip(lua_State *L, int index){
  AI **ai;
  luaL_checktype(L, index, LUA_TUSERDATA);
  ai = (AI**)luaL_checkudata(L, index, EPIAR_SHIP);
  if (ai == NULL) luaL_typerror(L, index, EPIAR_SHIP);
  return ai;
}

int AI_Lua::newShip(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5)
		return luaL_error(L, "Got %d arguments expected 5 (class, x, y, model, script)", n);

	double x = luaL_checknumber (L, 2);
	double y = luaL_checknumber (L, 3);
	string modelname = luaL_checkstring (L, 4);
	string scriptname = luaL_checkstring (L, 5);

	//Log::Message("Creating new Ship (%f,%f) (%s) (%s)",x,y,modelname.c_str(),scriptname.c_str());

	// Allocate memory for a pointer to object
	AI **s = pushShip(L);
	*s = new AI();
	(*s)->SetWorldPosition( Coordinate(x, y) );
	(*s)->SetModel( Models::Instance()->GetModel(modelname) );
	(*s)->SetScript( scriptname );

	// Add this ship to the SpriteManager
	Lua::GetSpriteList()->Add((Sprite*)(*s));

	return 1;
}

// Ship Functions

int AI_Lua::ShipAccelerate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ai= checkShip(L,1);
        (*ai)->Accelerate();
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n); 

	return 0;
}

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

int AI_Lua::ShipFire(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI** ai = checkShip(L,1);
		(*ai)->Fire();
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n); 
	}
	return 0;
}

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

int AI_Lua::ShipExplode(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI** ai = checkShip(L,1);
		Log::Message("A %s Exploded!",(*ai)->GetModelName().c_str());
		Lua::GetSpriteList()->Add(
			new Effect((*ai)->GetWorldPosition(), "Resources/Animations/explosion1.ani", 0) );
		Lua::GetSpriteList()->Delete((Sprite*)(*ai));
	} else {
		luaL_error(L, "Got %d arguments expected 1 (ship)", n); 
	}
	return 0;
}

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

int AI_Lua::ShipGetModelName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		AI** ai = checkShip(L,1);
		lua_pushfstring(L, ((*ai)->GetModelName()).c_str() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

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
