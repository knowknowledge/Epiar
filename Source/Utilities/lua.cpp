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
#include "Input/input.h"
#include "Utilities/file.h"

#include "Engine/hud.h"

/**\class Lua
 * \brief Lua subsystem. */

bool Lua::luaInitialized = false;
lua_State *Lua::L = NULL;
vector<string> Lua::buffer;

bool Lua::Load( const string& filename ) {
	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	// Load the lua script
	if( 0 != luaL_loadfile(L, filename.c_str()) ) {
		Log::Error("Error loading '%s': %s", filename.c_str(), lua_tostring(L, -1));
		return false;
	}

	// Execute the lua script
	if( 0 != lua_pcall(L, 0, 0, 0) ) {
		Log::Error("Error Executing '%s': %s", filename.c_str(), lua_tostring(L, -1));
		return false;
	}

	Log::Message("Loaded Lua Script '%s'",filename.c_str());
	return( true );
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
	//Log::Message("Running '%s'", (char *)line.c_str() );

	if( ! luaInitialized ) {
		if( Init() == false ) {
			Log::Warning( "Could not load Lua script. Unable to initialize Lua." );
			return( false );
		}
	}

	if( luaL_dostring(L,line.c_str()) ) {
		Log::Error("Error running '%s': %s", line.c_str(), lua_tostring(L, -1));
		lua_pop(L, 1);  /* pop error message from the stack */
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
		{"getSprite", &Lua::getSpriteByID},
		{"ships", &Lua::getShips},
		{"planets", &Lua::getPlanets},
		{"RegisterKey", &Input::RegisterKey},  
		{NULL, NULL}
	};
	luaL_register(L,"Epiar",EngineFunctions);


	// Register these functions to their own lua namespaces
	AI_Lua::RegisterAI(L);
	UI_Lua::RegisterUI(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);
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

int Lua::getSpriteByID(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (SpriteID)", n);

	// Get the Sprite using the ID
	int id = (int)(luaL_checkint(L,1));
    Sprite **s;
	Sprite* sprite = SpriteManager::Instance()->GetSpriteByID(id);

	// Push Sprite
	switch( sprite->GetDrawOrder() ){
		case DRAW_ORDER_PLAYER:
		case DRAW_ORDER_SHIP:
			s = (Sprite **)AI_Lua::pushShip(L);
			break;
		case DRAW_ORDER_PLANET:
			s = (Sprite **)Planets_Lua::pushPlanet(L);
			break;
		default:
			Log::Error("Unexpected Sprite Type '%d'", sprite->GetDrawOrder() );
			s = (Sprite **)lua_newuserdata(L, sizeof(Sprite*));
			break;
	}
	*s = sprite;
	return 1;
}

int Lua::getSprites(lua_State *L, int type){
	int n = lua_gettop(L);  // Number of arguments

	list<Sprite *> *sprites = NULL;
	if( n==3 ){
		double x = luaL_checknumber (L, 1);
		double y = luaL_checknumber (L, 2);
		double r = luaL_checknumber (L, 3);
		sprites = SpriteManager::Instance()->GetSpritesNear(Coordinate(x,y),r);
	} else {
		sprites = SpriteManager::Instance()->GetSprites();
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

