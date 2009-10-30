/*
 * Filename      : ai_lua.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Thursday, October 29, 2009
 * Purpose       : Lua Bridge for AI objects
 * Notes         :
 */

#include "Utilities/lua.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "AI/ai.h"
#include "AI/ai_lua.h"

void AI_Lua::RegisterAI(lua_State *luaVM){
	// These are the Ship Functions we're supporting in Lua
	static const luaL_Reg shipFunctions[] = {
		// Creation
		{"new", &AI_Lua::newShip},
		{"Accelerate", &AI_Lua::ShipAccelerate},
		{"Rotate", &AI_Lua::ShipRotate},
		{"GetAngle", &AI_Lua::ShipGetAngle},
		{"GetPosition", &AI_Lua::ShipGetPosition},
		{"GetMomentumAngle", &AI_Lua::ShipGetMomentumAngle},
		{"GetMomentumSpeed", &AI_Lua::ShipGetMomentumSpeed},
		{"directionTowards", &AI_Lua::ShipGetDirectionTowards},
		{NULL, NULL}
	};
	luaL_newmetatable(luaVM, "EpiarLua.Ship");
	luaL_openlib(luaVM, "EpiarLua.Ship", shipFunctions,0);  
}

int AI_Lua::newShip(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 5)
		return luaL_error(luaVM, "Got %d arguments expected 5 (class, x, y, model, script)", n);

	double x = luaL_checknumber (luaVM, 2);
	double y = luaL_checknumber (luaVM, 3);
	string modelname = luaL_checkstring (luaVM, 4);
	string scriptname = luaL_checkstring (luaVM, 5);

	Log::Message("Creating new Ship (%f,%f) (%s) (%s)",x,y,modelname.c_str(),scriptname.c_str());

	// Allocate memory for a pointer to object
	AI **s = (AI **)lua_newuserdata(luaVM, sizeof(AI*));
	*s = new AI();
	(*s)->SetWorldPosition( Coordinate(x, y) );
	(*s)->SetModel( Models::Instance()->GetModel(modelname) );
	(*s)->SetScript( scriptname );

	// Add this ship to the SpriteManager
	Lua::GetSpriteList()->Add((Sprite*)(*s));

	return 1;
}

AI* checkAI(lua_State* L, int index)
{
	void* ud = 0;
	luaL_checktype(L, index, LUA_TTABLE); 
	lua_getfield(L, index, "__self");
	ud = luaL_checkudata(L, index, "EpiarLua.Ship");
	luaL_argcheck(L, ud != NULL, index, "`EpiarLua.Ship' expected");  

	return *((AI**)ud);
}


// Ship Functions

int AI_Lua::ShipAccelerate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		if(realAI){
			realAI->Accelerate();
		} else {
			Log::Error("Failed to find the Associated AI");
			exit(100);
		}
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n); 

	return 0;
}

int AI_Lua::ShipRotate(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 2) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		Direction dir = (Direction)luaL_checknumber(L, 2);
		if(realAI){
			realAI->Rotate(dir);
		} else {
			Log::Error("Failed to find the Associated AI");
			exit(100);
		}
	}
	else
		luaL_error(L, "Got %d arguments expected 2 (self, direction)", n); 

	return 0;
}

int AI_Lua::ShipGetAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		lua_pushnumber(L, (double) realAI->GetAngle() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}

int AI_Lua::ShipGetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		lua_pushnumber(L, (double) realAI->GetWorldPosition().GetX() );
		lua_pushnumber(L, (double) realAI->GetWorldPosition().GetY() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 2;
}

int AI_Lua::ShipGetMomentumAngle(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		lua_pushnumber(L, (double) realAI->GetMomentum().GetAngle() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}

int AI_Lua::ShipGetMomentumSpeed(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		AI** ptrAI= (AI**)lua_touserdata(L,1);
		AI* realAI = *ptrAI;
		lua_pushnumber(L, (double) realAI->GetMomentum().GetMagnitude() );
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}

int AI_Lua::ShipGetDirectionTowards(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	AI** ptrAI;
	AI* realAI;

	if (n > 1) {
		ptrAI = (AI**)lua_touserdata(L,1);
		realAI = *ptrAI;
	}

	if (n == 2) { // Angle
		double angle = (Direction)luaL_checknumber(L, 2);
		lua_pushnumber(L, (double) realAI->directionTowards(angle) );
	}
	else if(n==3){ // Coordinate
		double x = (Direction)luaL_checknumber(L, 2);
		double y = (Direction)luaL_checknumber(L, 3);
		lua_pushnumber(L, (double) realAI->directionTowards(Coordinate(x,y)) );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n); 
	}
	return 1;
}
