/**\file			lua.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 5, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Provides abilities to load, store, and run Lua scripts
 * \details
 * To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "includes.h"
#include "Engine/console.h"
#include "Engine/simulation.h"
#include "Engine/models.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"
#include "AI/ai_lua.h"
#include "UI/ui_lua.h"
#include "UI/ui.h"
#include "UI/ui_window.h"
#include "UI/ui_label.h"
#include "UI/ui_button.h"
#include "Sprites/player.h"
#include "Sprites/sprite.h"
#include "Utilities/camera.h"
#include "Utilities/file.h"

/**\class Lua
 * \brief Lua subsystem. */

bool Lua::luaInitialized = false;
lua_State *Lua::luaVM = NULL;
SpriteManager *Lua::my_sprites= NULL;
vector<string> Lua::buffer;

bool Lua::Load( const string& filename ) {
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	// Start the lua script
	File luaFile = File( filename );
	char *buffer = luaFile.Read();
	if ( buffer == NULL ){
		Log::Error("Error reading Lua file: %s", filename.c_str());
		return false;
	}
	long bufsize = luaFile.GetLength();
	if( (luaL_loadbuffer(luaVM, buffer, bufsize, filename.c_str())) ||
			lua_pcall(luaVM, 0, LUA_MULTRET, 0)){
		Log::Error("Could not run lua file '%s'",filename.c_str());
		Log::Error("%s", lua_tostring(luaVM, -1));
		cout << lua_tostring(luaVM, -1) << endl;
		return false;
	}
	Log::Message("Loaded the universe");

	return( true );
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
		{"ispaused", &Lua::ispaused},
		{"player", &Lua::getPlayer},
		{"shakeCamera", &Lua::shakeCamera},
		{"models", &Lua::getModelNames},
		{"ships", &Lua::getShips},
		{"planets", &Lua::getPlanets},
		{NULL, NULL}
	};
	luaL_register(luaVM,"Epiar",EngineFunctions);


	// Register these functions to their own lua namespaces
	AI_Lua::RegisterAI(luaVM);
	UI_Lua::RegisterUI(luaVM);
	Planets_Lua::RegisterPlanets(luaVM);
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

int Lua::ispaused(lua_State *L){
	lua_pushnumber(L, (int) Simulation::isPaused() );
	return 1;
}

int Lua::getPlayer(lua_State *luaVM){
	Player **player = (Player**)AI_Lua::pushShip(luaVM);
	*player = Player::Instance();
	return 1;
}

//Allow camera shaking from Lua
int Lua::shakeCamera(lua_State *L){
	if (lua_gettop(L) == 4) {
		Camera *pInstance = Camera::Instance();
		pInstance->Shake(int(luaL_checknumber(L, 1)), int(luaL_checknumber(L,
						2)),  new Coordinate(luaL_checknumber(L, 3),luaL_checknumber(L, 2)));
	}
	return 0;
}

int Lua::getModelNames(lua_State *L){
	Models *models = Models::Instance();
	list<string> *names = models->GetModelNames();

    lua_createtable(L, names->size(), 0);
    int newTable = lua_gettop(L);
    int index = 1;
    list<string>::const_iterator iter = names->begin();
    while(iter != names->end()) {
        lua_pushstring(L, (*iter).c_str());
        lua_rawseti(L, newTable, index);
        ++iter;
        ++index;
    }
	delete names;
    return 1;
}

int Lua::getSprites(lua_State *L, int type){
	list<Sprite *> filtered;
	list<Sprite *> sprites = my_sprites->GetSprites();
	
	// Collect only the ships
	list<Sprite *>::iterator i;
	for( i = sprites.begin(); i != sprites.end(); ++i ) {
		if( (*i)->GetDrawOrder() == type){
			filtered.push_back( (*i) );
		}
	}

	// Populate a Lua table with ships
    lua_createtable(L, filtered.size(), 0);
    int newTable = lua_gettop(L);
    int index = 1;
    Sprite **s;
    list<Sprite *>::const_iterator iter = filtered.begin();
    while(iter != filtered.end()) {
		// push userdata
        switch(type){
            case DRAW_ORDER_PLAYER:
            case DRAW_ORDER_SHIP:
                s = (Sprite **)AI_Lua::pushShip(luaVM);
                break;
            case DRAW_ORDER_PLANET:
                s = (Sprite **)Planets_Lua::pushPlanet(luaVM);
                break;
            default:
                Log::Error("Unexpected Sprite Type '%d'",type);
                s = (Sprite **)lua_newuserdata(luaVM, sizeof(Sprite*));
                break;
        }
    
		*s = *iter;
        lua_rawseti(L, newTable, index);
        ++iter;
        ++index;
    }
    return 1;
}


int Lua::getShips(lua_State *L){
	return Lua::getSprites(L,DRAW_ORDER_SHIP);
}

int Lua::getPlanets(lua_State *L){
	return Lua::getSprites(L,DRAW_ORDER_PLANET);
}
