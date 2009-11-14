/*
 * Filename      : lua.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 5, 2008
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "Engine/console.h"
#include "Engine/simulation.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"
#include "AI/ai_lua.h"
#include "UI/ui_lua.h"
#include "UI/ui.h"
#include "UI/ui_window.h"
#include "UI/ui_label.h"
#include "UI/ui_button.h"
#include "Sprites/player.h"

bool Lua::luaInitialized = false;
lua_State *Lua::luaVM = NULL;
SpriteManager *Lua::my_sprites= NULL;
vector<string> Lua::buffer;

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
		cout << lua_tostring(luaVM, -1) << endl;
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
	Log::Message("Running '%s'", (char *)line.c_str() );

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
	// Register these functions to the lua global namespace

	static const luaL_Reg EngineFunctions[] = {
		{"echo", &Lua::console_echo},
		{"pause", &Lua::pause},
		{"unpause", &Lua::unpause},
		{"player", &Lua::getPlayer},
		{NULL, NULL}
	};
	luaL_register(luaVM,"Epiar",EngineFunctions);


	// Register these functions to their own lua namespaces
	AI_Lua::RegisterAI(luaVM);
	UI_Lua::RegisterUI(luaVM);
}

int Lua::console_echo(lua_State *L) {
	const char *str = lua_tostring(L, 1); // get argument

	if(str == NULL)
		Console::InsertResult("nil");
	else
		Console::InsertResult(str);

	return 0;
}

int Lua::pause(lua_State *luaVM){
	Simulation::pause();
	return 0;
}

int Lua::unpause(lua_State *luaVM){
	Simulation::unpause();
	return 0;
}

int Lua::getPlayer(lua_State *luaVM){
	Player **player = (Player**)lua_newuserdata(luaVM, sizeof(Player*));
	*player = Player::Instance();
	return 1;
}
