/*
 * Filename      : lua.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Monday, November 16 2009
 * Purpose       : Provides abilities to load, store, and run Lua scripts
 * Notes         : To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

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
#include "Input/input.h"

bool Lua::luaInitialized = false;
lua_State *Lua::L = NULL;
SpriteManager *Lua::my_sprites= NULL;
vector<string> Lua::buffer;
map<char, string> Lua::keyMappings;

bool Lua::Load( string filename ) {
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	// Start the lua script
	
	if( luaL_dofile(L,filename.c_str()) ){
		Log::Error("Could not run lua file '%s'",filename.c_str());
		Log::Error("%s", lua_tostring(L, -1));
		cout << lua_tostring(L, -1) << endl;
	} else {
		Log::Message("Loaded the universe");
	}
	

	return( false );
}

bool Lua::Update(){
    // Tell the Lua State to update itself
    lua_getglobal(L, "Update");
    if( lua_pcall(L,0,0,0) != 0 ){
		Log::Error("Could not call lua function Update");
	    Log::Error("%s", lua_tostring(L, -1));
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

	error = luaL_loadbuffer(L, line.c_str(), line.length(), "line") || lua_pcall(L, 0, 1, 0);
	if( error ) {
		Console::InsertResult(lua_tostring(L, -1));
		lua_pop(L, 1);  /* pop error message from the stack */
	}

	return( false );
}

void Lua::HandleInput( list<InputEvent> & events ) {
	for( list<InputEvent>::iterator i = events.begin(); i != events.end(); ++i) {
		if(i->type==KEY && i->kstate == KEYUP ) {
			map<char,string>::iterator val = keyMappings.find( i->key );
			if( val != keyMappings.end() ){
				Run( val->second );
			}
		}
	}
}

void Lua::RegisterKeyInput( char key, string command ) {
	keyMappings.insert(make_pair(key, command));
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
	
	L = lua_open();

	if( !L ) {
		Log::Warning( "Could not initialize Lua VM." );
		return( false );
	}

	luaL_openlibs( L );

	RegisterFunctions();
	
	luaInitialized = true;
	
	return( true );
}

bool Lua::Close() {
	if( luaInitialized ) {
		lua_close( L );
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
		{"RegisterKey", &Lua::RegisterKey},
		{NULL, NULL}
	};
	luaL_register(L,"Epiar",EngineFunctions);


	// Register these functions to their own lua namespaces
	AI_Lua::RegisterAI(L);
	UI_Lua::RegisterUI(L);
	Planets_Lua::RegisterPlanets(L);
}

int Lua::console_echo(lua_State *L) {
	const char *str = lua_tostring(L, 1); // get argument

	if(str == NULL)
		Console::InsertResult("nil");
	else
		Console::InsertResult(str);

	return 0;
}

int Lua::pause(lua_State *L){
		Simulation::pause();
	return 0;
}

int Lua::unpause(lua_State *L){
	Simulation::unpause();
	return 0;
}

int Lua::ispaused(lua_State *L){
	lua_pushnumber(L, (int) Simulation::isPaused() );
	return 1;
}

int Lua::getPlayer(lua_State *L){
	Player **player = (Player**)AI_Lua::pushShip(L);
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
	int n = lua_gettop(L);  // Number of arguments

	list<Sprite *> *sprites = NULL;
	if( n==3 ){
		double x = luaL_checknumber (L, 1);
		double y = luaL_checknumber (L, 2);
		double r = luaL_checknumber (L, 3);
		sprites = my_sprites->GetSpritesNear(Coordinate(x,y),r);
	} else {
		sprites = my_sprites->GetSprites();
	}
	
	// Collect only the Sprites of this type
	list<Sprite *>::iterator i;
	list<Sprite *> filtered;
	for( i = sprites->begin(); i != sprites->end(); ++i ) {
		if( (*i)->GetDrawOrder() == type){
			filtered.push_back( (*i) );
		}
	}

	// Populate a Lua table with Sprites
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
                s = (Sprite **)AI_Lua::pushShip(L);
                break;
            case DRAW_ORDER_PLANET:
                s = (Sprite **)Planets_Lua::pushPlanet(L);
                break;
            default:
                Log::Error("Unexpected Sprite Type '%d'",type);
                s = (Sprite **)lua_newuserdata(L, sizeof(Sprite*));
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

int Lua::RegisterKey(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if(n == 2) {
		char key = (char)(luaL_checkstring(L,1)[0]);
		string command = (string)luaL_checkstring(L,2);
		RegisterKeyInput(key,command);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (Key, Command)", n); 
	}
	return 0;
}
