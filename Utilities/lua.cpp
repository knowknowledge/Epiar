/*
 * Filename      : lua.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 5, 2008
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "Engine/console.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"
#include "AI/ai.h"
#include "Sprites/spriteManager.h"

bool Lua::luaInitialized = false;
lua_State *Lua::luaVM = NULL;
SpriteManager *Lua::my_sprites= NULL;
vector<string> Lua::buffer;

// Exported functions
static int lua_echo(lua_State *L);

bool Lua::Load( string filename ) {
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	// Start the lua script
	
	if( luaL_dofile(luaVM,filename.c_str()) ){
		Log::Error("Could not run lua file '%s'",filename.c_str());
		Log::Error("%s", lua_tostring(luaVM, -1));
	} else {
		Log::Message("Loaded the universe");
	}
	

	return( false );
}

bool Lua::Run( string line ) {
	int error = 0;

	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	error = luaL_loadbuffer(luaVM, line.c_str(), line.length(), "line") || lua_pcall(luaVM, 0, 1, 0);
	if( error ) {
		Console::InsertResult(lua_tostring(luaVM, -1));
		lua_pop(luaVM, 1);  /* pop error message from the stack */
	}

	return( false );
}

// returns the output from the last lua script and deletes it from internal buffer
vector<string> Lua::GetOutput() {
	vector<string> ret = buffer;

	buffer.clear();

	return( ret );
}

bool Lua::SetSpriteList(SpriteManager* the_sprites){
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}
	
	my_sprites = the_sprites;
	return( true );
}

bool Lua::Init() {
	if( luaInitialized ) {
		Log::Warning( "Cannot initialize Lua. It is already initialized." );
		return( false );
	}
	
	luaVM = luaL_newstate();
	luaL_openlibs( luaVM );
		
	if( !luaVM ) {
		Log::Warning( "Could not initialize Lua VM." );
		return( false );
	}

	luaL_openlibs( luaVM );

	RegisterFunctions();
	
	luaInitialized = true;
	
	return( true );
}

bool Lua::Close() {
	if( luaInitialized ) {
		lua_close( luaVM );
	} else {
		Log::Warning( "Cannot deinitialize Lua. It is either not initialized or a script is still loaded." );
		return( false );
	}
	
	return( true );
}

void Lua::RegisterFunctions() {

	// These are the Ship Functions we're supporting in Lua
	static const luaL_Reg shipFunctions[] = {
		// Creation
		{"new", &newShip},
		// Accelerate
		// rotate
		// get location
		// get angle
		{NULL, NULL}
	};

	// Export the Ship functions to Lua
	luaL_register(luaVM, "Ship", shipFunctions);  

	lua_pushcfunction(luaVM, lua_echo);
	lua_setglobal(luaVM, "echo");
}

static int lua_echo(lua_State *L) {
	const char *str = lua_tostring(L, 1); // get argument

	if(str == NULL)
		Console::InsertResult("nil");
	else
		Console::InsertResult(str);

	return 0;
}


int Lua::newShip(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 5)
		return luaL_error(luaVM, "Got %d arguments expected 5 (class, x, y, model, script)", n);

	// First argument is now a table that represent the class to instantiate
	luaL_checktype(luaVM, 1, LUA_TTABLE);

	// Create table to represent instance
	lua_newtable(luaVM);

	// Set first argument of new to metatable of instance
	lua_pushvalue(luaVM,1);
	lua_setmetatable(luaVM, -2);

	// Do function lookups in metatable
	lua_pushvalue(luaVM,1);
	lua_setfield(luaVM, 1, "__index");

	// Allocate memory for a pointer to object
	AI **s = (AI **)lua_newuserdata(luaVM, sizeof(AI*));

	double x = luaL_checknumber (luaVM, 2);
	double y = luaL_checknumber (luaVM, 3);
	string modelname = luaL_checkstring (luaVM, 4);
	string scriptname = luaL_checkstring (luaVM, 5);

	Log::Message("Creating new Ship (%f,%f) (%s) (%s)",x,y,modelname.c_str(),scriptname.c_str());

	*s = new AI();
	(*s)->SetWorldPosition( Coordinate(x, y) );
	(*s)->SetModel( Models::Instance()->GetModel(modelname) );
	(*s)->SetScript( scriptname );

	// Add this ship to the SpriteManager
	if( !my_sprites){
		Log::Error("Can't add the ship to the main Sprite List.");
	} else {
		my_sprites->Add((Sprite*)(*s));
	}

	// Get metatable 'Ship' store in the registry
	luaL_getmetatable(luaVM, "Ship");

	// Set user data for Sprite to use this metatable
	lua_setmetatable(luaVM, -2);

	// Set field '__self' of instance table to the sprite user data
	lua_setfield(luaVM, -2, "__self");

	return 1;
}


