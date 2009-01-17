/*
 * Filename      : lua.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "Utilities/log.h"
#include "Utilities/lua.h"

bool LuaScript::luaInitialized = false;
lua_State *LuaScript::luaVM = NULL;
int LuaScript::numScriptsLoaded = 0; // when destructor decrements this to zero, we de-init the Lua VM

LuaScript::LuaScript() {
	// luaVM == null? load it and luaInitialized = true
}

LuaScript::LuaScript( string filename ) {
	Load( filename );
}

LuaScript::~LuaScript() {
	// numSciptsLoaded--. does it equal 0? deinit luaVM and luaInitialized = false
}

bool LuaScript::Load( string filename ) {
	if( ! luaInitialized ) {
		if( InitLua() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}
	
	Log::Message( "Function not finished." );
	// numScriptsLoaded++;
	
	return( false );
}

bool LuaScript::Run() {
	Log::Message( "STUB" );
	
	return( false );
}

bool LuaScript::InitLua() {
	if( luaInitialized ) {
		Log::Warning( "Cannot initialize Lua. It is already initialized." );
		return( false );
	}
	
	luaVM = luaL_newstate();
		
	if( !luaVM ) {
		Log::Warning( "Could not initialize Lua VM." );
		return( false );
	}

	luaL_openlibs( luaVM );
	
	luaInitialized = true;
	
	return( true );
}

bool LuaScript::CloseLua() {
	if( luaInitialized && numScriptsLoaded == 0 ) {
		lua_close( luaVM );
	} else {
		Log::Warning( "Cannot deinitialize Lua. It is either not initialized or a script is still loaded." );
		return( false );
	}
	
	return( true );
}
