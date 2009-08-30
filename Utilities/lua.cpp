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

bool Lua::Update(){
    // Tell the Lua State to update itself
    lua_getglobal(luaVM, "Update");
    if( lua_pcall(luaVM,0,0,0) != 0 ){
		Log::Error("Could not call lua function Update");
	    Log::Error("%s", lua_tostring(luaVM, -1));
        return (false);
    }
	return (true);
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

SpriteManager* Lua::GetSpriteList(){
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}
	return my_sprites;
}

bool Lua::Init() {
	if( luaInitialized ) {
		Log::Warning( "Cannot initialize Lua. It is already initialized." );
		return( false );
	}
	
	luaVM = lua_open();

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
	AI::RegisterAI(luaVM);

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

