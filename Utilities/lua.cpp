/*
 * Filename      : lua.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 5, 2008
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "Utilities/log.h"
#include "Utilities/lua.h"

bool Lua::luaInitialized = false;
lua_State *Lua::luaVM = NULL;
vector<string> Lua::buffer;

bool Lua::Load( string filename ) {
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}
	
	Log::Message( "Function not finished." );
	
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

	error = luaL_loadbuffer(luaVM, line.c_str(), line.length(), "line") || lua_pcall(luaVM, 0, 0, 0);
	if( error ) {
		fprintf(stderr, "%s", lua_tostring(luaVM, -1));
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

