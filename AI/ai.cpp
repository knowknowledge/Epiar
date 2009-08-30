/*
 * Filename      : ai.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "AI/ai.h"
#include "Utilities/lua.h"

#include "Sprites/Player.h"
#include "Sprites/spriteManager.h"

AI::AI() {
	
}

void AI::Update(){
	// Decide
	
	// Now act like a normal ship
	this->Ship::Update();
}

void AI::SetScript(string script){
	my_script = script;
}


void AI::RegisterAI(lua_State *luaVM){
	// These are the Ship Functions we're supporting in Lua
	static const luaL_Reg shipFunctions[] = {
		// Creation
		{"new", &AI::newShip},
		{"Accelerate", &ShipAccelerate},
		{"Rotate", &AI::ShipRotate},
		{"GetAngle", &ShipGetAngle},
		{"GetPosition", &AI::ShipGetPosition},
		{"GetMomentumAngle", &ShipGetMomentumAngle},
		{"GetMomentumSpeed", &ShipGetMomentumSpeed},
		{NULL, NULL}
	};
	luaL_newmetatable(luaVM, "EpiarLua.Ship");
	luaL_openlib(luaVM, "EpiarLua.Ship", shipFunctions,0);  
}

int AI::newShip(lua_State *luaVM){
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

int AI::ShipAccelerate(lua_State* L){
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

int AI::ShipRotate(lua_State* L){
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

int AI::ShipGetAngle(lua_State* L){
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

int AI::ShipGetPosition(lua_State* L){
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

int AI::ShipGetMomentumAngle(lua_State* L){
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

int AI::ShipGetMomentumSpeed(lua_State* L){
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


