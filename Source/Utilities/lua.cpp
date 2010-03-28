/**\file			lua.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 5, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Provides abilities to load, store, and run Lua scripts
 * \details
 * To be used in conjunction with various other subsystems, A.I., GUI, etc.
 */

#include "includes.h"
#include "common.h"
#include "Audio/audio.h"
#include "Audio/audio_lua.h"
#include "Engine/console.h"
#include "Engine/simulation.h"
#include "Engine/models.h"
#include "Engine/alliances.h"
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
#include "Sprites/planets.h"
#include "Utilities/camera.h" 
#include "Input/input.h"
#include "Utilities/file.h"
#include "Utilities/filesystem.h"

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



// If the function is known at compile time, use 'Call' instead of 'Run'
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

// This function is from the Lua PIL
// http://www.lua.org/pil/25.3.html
// It was originally named "call_va"
//
// WARNING: any s as a return must be a pointer to a string (not a c str)
//          This allows Lua::Call to clear the stack when we're done.
bool Lua::Call(const char *func, const char *sig, ...) {
	va_list vl;
	int narg, nres,resultcount;  /* number of arguments and results */

	va_start(vl, sig);
	lua_getglobal(L, func);  /* get function */

	/* push arguments */
	narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {

			case 'd':  /* double argument */
				lua_pushnumber(L, va_arg(vl, double));
				break;

			case 'i':  /* int argument */
				lua_pushnumber(L, va_arg(vl, int));
				break;

			case 's':  /* string argument */
				lua_pushstring(L, va_arg(vl, char *));
				break;

			case '>':
				goto endwhile;

			default:
				luaL_error(L, "invalid option (%c)", *(sig - 1));
		}
		narg++;
		luaL_checkstack(L, 1, "too many arguments");
	} endwhile:

	/* do the call */
	resultcount = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg, resultcount, 0) != 0)  /* do the call */
		luaL_error(L, "error running function `%s': %s",
				func, lua_tostring(L, -1));

	/* retrieve results */
	nres = -resultcount;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {

			case 'd':  /* double result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, double *) = lua_tonumber(L, nres);
				break;

			case 'i':  /* int result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
				break;

			case 's':  /* string result */
				if (!lua_isstring(L, nres))
					luaL_error(L, "wrong result kind");
				*va_arg(vl, string*) = lua_tostring(L, nres);
				break;

			default:
				luaL_error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
	lua_pop(L,resultcount);
	return true;
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
		{"getoption", &Lua::getoption},
		{"setoption", &Lua::setoption},
		{"setsoundvol", &Lua::setsoundvol},
		{"setmusicvol", &Lua::setmusicvol},
		{"loadPlayer", &Lua::loadPlayer},
		{"newPlayer", &Lua::newPlayer},
		{"players", &Lua::getPlayerNames},
		{"player", &Lua::getPlayer},
		{"getCamera", &Lua::getCamera},
		{"moveCamera", &Lua::moveCamera},
		{"shakeCamera", &Lua::shakeCamera},
		{"focusCamera", &Lua::focusCamera},
		{"alliances", &Lua::getAllianceNames},
		{"models", &Lua::getModelNames},
		{"weapons", &Lua::getWeaponNames},
		{"engines", &Lua::getEngineNames},
		{"technologies", &Lua::getTechnologyNames},
		{"planetNames", &Lua::getPlanetNames},
		{"getSprite", &Lua::getSpriteByID},
		{"getMSRP", &Lua::getMSRP},
		{"ships", &Lua::getShips},
		{"planets", &Lua::getPlanets},
		{"nearestShip", &Lua::getNearestShip},
		{"nearestPlanet", &Lua::getNearestPlanet},
		{"RegisterKey", &Input::RegisterKey},  
		{"UnRegisterKey", &Input::UnRegisterKey},  
		{"getAllianceInfo", &Lua::getAllianceInfo},
		{"getModelInfo", &Lua::getModelInfo},
		{"getPlanetInfo", &Lua::getPlanetInfo},
		{"getWeaponInfo", &Lua::getWeaponInfo},
		{"getEngineInfo", &Lua::getEngineInfo},
		{"getTechnologyInfo", &Lua::getTechnologyInfo},
		{"setInfo", &Lua::setInfo},
		{"saveComponents", &Lua::saveComponents},
		{"listImages", &Lua::listImages},
		{NULL, NULL}
	};
	luaL_register(L,"Epiar",EngineFunctions);


	// Register these functions to their own lua namespaces
	AI_Lua::RegisterAI(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
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

int Lua::getoption(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (option)", n);
	string path = (string)lua_tostring(L, 1);
	string value = OPTION(string,path);
	lua_pushstring(L, value.c_str());
	return 1;
}

int Lua::setoption(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 1 (option,value)", n);
	string path = (string)lua_tostring(L, 1);
	string value = (string)lua_tostring(L, 2);
	SETOPTION(path,value);
	return 0;
}

int Lua::setsoundvol(lua_State *L){
	int n = lua_gettop(L);	// Number of arguments
	if (n !=  1)
		return luaL_error(L, "Sound volume: Got %d arguments expected 1 (volume)", n);
	float volume = TO_FLOAT(luaL_checknumber(L, 1));
	Audio::Instance().SetSoundVol(volume);
	SETOPTION("options/sound/soundvolume",volume);
	return 0;
}

int Lua::setmusicvol(lua_State *L){
	int n = lua_gettop(L);	// Number of arguments
	if (n !=  1)
		return luaL_error(L, "Music volume: Got %d arguments expected 1 (volume)", n);
	float volume = TO_FLOAT(luaL_checknumber(L, 1));
	Audio::Instance().SetMusicVol(volume);
	SETOPTION("options/sound/musicvolume",volume);
	return 0;
}

int Lua::getPlayerNames(lua_State *L) {
	list<string> *names = Players::Instance()->GetNames();
	pushNames(L,names);
	delete names;
	return 1;
}

int Lua::loadPlayer(lua_State *L) {
    int n = lua_gettop(L);
	if (n != 1) {
		return luaL_error(L, "Loading a Player expects a name");
    }
	string playerName = (string) luaL_checkstring(L,1);
	cout<<"Loading Player: "<<playerName<<endl;
	Player* newPlayer = Players::Instance()->GetPlayer( playerName );
	if( newPlayer==NULL ) {
		return luaL_error(L, "There is no Player by the name '%s'",playerName.c_str());
	}
	bool alreadyLoaded = Player::IsLoaded();
	Player::Load(playerName);
	if(!alreadyLoaded) {
        // Set player model based on simulation xml file settings
        SpriteManager::Instance()->Add( Player::Instance() );

        // Focus the camera on the sprite
        Camera::Instance()->Focus( Player::Instance() );
	}
	return 0;
}

int Lua::newPlayer(lua_State *L) {
    int n = lua_gettop(L);
	if (n != 1) {
		return luaL_error(L, "Loading a Player expects a name");
    }

	string playerName = (string) luaL_checkstring(L,1);
	cout<<"Creating Player: "<<playerName<<endl;

	Player::CreateNew(playerName);

	// Set player model based on simulation xml file settings
	SpriteManager::Instance()->Add( Player::Instance() );

	// Focus the camera on the sprite
	Camera::Instance()->Focus( Player::Instance() );
	return 0;
}

int Lua::getPlayer(lua_State *L){
	Lua::pushSprite(L,Player::Instance() );
	return 1;
}

int Lua::getCamera(lua_State *L){
    int n = lua_gettop(L);
	if (n != 0) {
		return luaL_error(L, "Getting the Camera Coordinates didn't expect %d arguments. But thanks anyway", n);
    }
    Coordinate c = Camera::Instance()->GetFocusCoordinate();
    lua_pushinteger(L,static_cast<lua_Integer>(c.GetX()));
    lua_pushinteger(L,static_cast<lua_Integer>(c.GetY()));
	return 2;
}

int Lua::moveCamera(lua_State *L){
    int n = lua_gettop(L);
	if (n != 2) {
		return luaL_error(L, "Moving the Camera needs 2 arguments (X,Y) not %d arguments", n);
    }
    int x = luaL_checkinteger(L,1);
    int y = luaL_checkinteger(L,2);
    Camera::Instance()->Focus((Sprite*)NULL); // This unattaches the Camera from the focusSprite
    Camera::Instance()->Move(-x,y);
	return 0;
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

int Lua::focusCamera(lua_State *L){
    int n = lua_gettop(L);
	if (n == 1) {
		int id = (int)(luaL_checkint(L,1));
		SpriteManager *sprites= SpriteManager::Instance();
		Sprite* target = sprites->GetSpriteByID(id);
		if(target!=NULL)
            Camera::Instance()->Focus( target );
	} else if (n == 2) {
		double x,y;
        x = (luaL_checknumber(L,1));
        y = (luaL_checknumber(L,2));
        Camera::Instance()->Focus((Sprite*)NULL);
        Camera::Instance()->Focus(x,y);
    } else {
		return luaL_error(L, "Got %d arguments expected 1 (SpriteID) or 2 (X,Y)", n);
    }
	return 0;
}

int Lua::getAllianceNames(lua_State *L){
	list<string> *names = Alliances::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}
int Lua::getWeaponNames(lua_State *L){
	list<string> *names = Weapons::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}

int Lua::getModelNames(lua_State *L){
	list<string> *names = Models::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}

int Lua::getEngineNames(lua_State *L){
	list<string> *names = Engines::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}

int Lua::getTechnologyNames(lua_State *L){
	list<string> *names = Technologies::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}

int Lua::getPlanetNames(lua_State *L){
	list<string> *names = Planets::Instance()->GetNames();
	pushNames(L,names);
	delete names;
    return 1;
}

void Lua::pushSprite(lua_State *L,Sprite* s){
	int* id = (int*)lua_newuserdata(L, sizeof(int*));
	*id = s->GetID();
	assert(s->GetDrawOrder() & (DRAW_ORDER_SHIP | DRAW_ORDER_PLAYER | DRAW_ORDER_PLANET) );
	switch(s->GetDrawOrder()){
	case DRAW_ORDER_SHIP:
	case DRAW_ORDER_PLAYER:
		luaL_getmetatable(L, EPIAR_SHIP);
		lua_setmetatable(L, -2);
		break;
	case DRAW_ORDER_PLANET:
		luaL_getmetatable(L, EPIAR_PLANET);
		lua_setmetatable(L, -2);
		break;
	default:
		Log::Error("Accidentally pushing sprite #%d with invalid kind: %d",s->GetID(),s->GetDrawOrder());
	}
}

/*
Sprite* Lua::checkSprite(lua_State *L,int id ){
	int* idptr = (int*)luaL_checkudata(L, index, EPIAR_SHIP);
	cout<<"Checking ID "<<(*idptr)<<endl;
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_SHIP' expected");
	Sprite* s;
	s = SpriteManager::Instance()->GetSpriteByID(*idptr);
	return s;
}
*/

void Lua::pushNames(lua_State *L, list<string> *names){
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
}

void Lua::pushComponents(lua_State *L, list<Component*> *components){
    lua_createtable(L, components->size(), 0);
    int newTable = lua_gettop(L);
    int index = 1;
    list<Component*>::const_iterator iter = components->begin();
    while(iter != components->end()) {
        lua_pushstring(L, (*iter)->GetName().c_str());
        lua_rawseti(L, newTable, index);
        ++iter;
        ++index;
    }
}

void Lua::setField(const char* index, int value) {
	lua_pushstring(L, index);
	lua_pushinteger(L, value);
	lua_settable(L, -3);
}

void Lua::setField(const char* index, float value) {
	lua_pushstring(L, index);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void Lua::setField(const char* index, const char* value) {
	lua_pushstring(L, index);
	lua_pushstring(L, value);
	lua_settable(L, -3);
}

int Lua::getIntField(int index, const char* name) {
	int val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L,index);
	val = luaL_checkint(L,lua_gettop(L));
	lua_pop(L,1);
	return val;
}

float Lua::getNumField(int index, const char* name) {
	float val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = static_cast<float>(luaL_checknumber(L,lua_gettop(L)));
	lua_pop(L,1);
	return val;
}

string Lua::getStringField(int index, const char* name) {
	string val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = luaL_checkstring(L,lua_gettop(L));
	lua_pop(L,1);
	return val;
}

list<string> Lua::getStringListField(int index) {
	list<string> results;

	// Go to the nil element of the array
	lua_pushnil(L);
	// While there are elements in the array
	// push the "next" one to the top of the stack
	while(lua_next(L, index)) {
		string val =  lua_tostring(L, -1);
		results.push_back(val);
		// Pop off this value
		lua_pop(L, 1);
	}
	return results;
}

list<string> Lua::getStringListField(int index, const char* name) {
	list<string> results;

	// Get the value from lua_stack[index][name]
	lua_pushstring(L, name);
	lua_gettable(L, index);
	// This needs to be a list of strings
	int stringTable = lua_gettop(L);
	assert(lua_istable(L,stringTable));
	

	// Go to the nil element of the array
	lua_pushnil(L);
	// While there are elements in the array
	// push the "next" one to the top of the stack
	while(lua_next(L, stringTable)) {
		string val =  lua_tostring(L, -1);
		results.push_back(val);
		// Pop off this value
		lua_pop(L, 1);
	}
	return results;
}

//can be found here  http://www.lua.org/pil/24.2.3.html
void Lua::stackDump (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
	int t = lua_type(L, i);
	switch (t) {

	  case LUA_TSTRING:  /* strings */
		printf("[%d]`%s'", i,lua_tostring(L, i));
		break;

	  case LUA_TBOOLEAN:  /* booleans */
		printf("[%d]%s",i,lua_toboolean(L, i) ? "true" : "false");
		break;

	  case LUA_TNUMBER:  /* numbers */
		printf("[%d]%g",i, lua_tonumber(L, i));
		break;

	  default:  /* other values */
		printf("[%d]%s",i, lua_typename(L, t));
		break;

	}
	printf("  ");  /* put a separator */
  }
  printf("\n");  /* end the listing */
}


int Lua::getSpriteByID(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (SpriteID)", n);

	// Get the Sprite using the ID
	int id = (int)(luaL_checkint(L,1));
	Sprite* sprite = SpriteManager::Instance()->GetSpriteByID(id);

	// Return nil if the sprite no longer exists
	if(sprite==NULL){
		return 0;
	}

	Lua::pushSprite(L,sprite);
	return 1;
}

int Lua::getSprites(lua_State *L, int kind){
	int n = lua_gettop(L);  // Number of arguments

	list<Sprite *> *sprites = NULL;
	if( n==3 ){
		double x = luaL_checknumber (L, 1);
		double y = luaL_checknumber (L, 2);
		double r = luaL_checknumber (L, 3);
		sprites = SpriteManager::Instance()->GetSpritesNear(Coordinate(x,y),static_cast<float>(r),kind);
	} else {
		sprites = SpriteManager::Instance()->GetSprites(kind);
	}

	// Populate a Lua table with Sprites
    lua_createtable(L, sprites->size(), 0);
    int newTable = lua_gettop(L);
    int index = 1;
    list<Sprite *>::const_iterator iter = sprites->begin();
    while(iter != sprites->end()) {
		// push userdata
		pushSprite(L,(*iter));
        lua_rawseti(L, newTable, index);
        ++iter;
        ++index;
    }
    return 1;
}

int Lua::getMSRP(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (  )", n);
	string name = (string)luaL_checkstring(L,1);

	// Is there a priced Component named 'name'?
	Component* comp = NULL;
	if( (comp = Models::Instance()->Get(name)) != NULL )
		lua_pushinteger(L,((Model*)comp)->GetMSRP() );
	else if( (comp = Engines::Instance()->Get(name)) != NULL )
		lua_pushinteger(L,((Engine*)comp)->GetMSRP() );
	else if( (comp = Weapons::Instance()->Get(name)) != NULL )
		lua_pushinteger(L,((Weapon*)comp)->GetMSRP() );
	else {
		return luaL_error(L, "Couldn't find anything by the name: '%s'", name.c_str());
	}
	// One of those should have worked or we would have hit the above else
	assert(comp!=NULL);
	return 1;
}

int Lua::getShips(lua_State *L){
	return Lua::getSprites(L,DRAW_ORDER_SHIP);
}

int Lua::getPlanets(lua_State *L){
	Planets *planets = Planets::Instance();
	list<string>* planetNames = planets->GetNames();

    lua_createtable(L, planetNames->size(), 0);
    int newTable = lua_gettop(L);
    int index = 1;
	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		pushSprite(L,planets->GetPlanet(*pname));
        lua_rawseti(L, newTable, index);
        ++index;
	}
	return 1;
}

int Lua::getNearestSprite(lua_State *L,int kind) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=2 ){
		return luaL_error(L, "Got %d arguments expected 1 (ship, range)", n);
	}
	AI* ai = AI_Lua::checkShip(L,1);
	if( ai==NULL ) {
		return 0;
	}
	float r = static_cast<float>(luaL_checknumber (L, 2));
	Sprite *closest = SpriteManager::Instance()->GetNearestSprite((ai),r,kind);
	if(closest!=NULL){
		assert(closest->GetDrawOrder() & (kind));
		pushSprite(L,(closest));
		return 1;
	} else {
		return 0;
	}
}

int Lua::getNearestShip(lua_State *L) {
	return Lua::getNearestSprite(L,DRAW_ORDER_SHIP|DRAW_ORDER_PLAYER);
}

int Lua::getNearestPlanet(lua_State *L) {
	return Lua::getNearestSprite(L,DRAW_ORDER_PLANET);
}

int Lua::getAllianceInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (AllianceName)", n);
	string name = (string)luaL_checkstring(L,1);
	Alliance *alliance = Alliances::Instance()->GetAlliance(name);

    lua_newtable(L);
	setField("Name", alliance->GetName().c_str());
	setField("AttackSize", alliance->GetAttackSize());
	setField("Aggressiveness", alliance->GetAggressiveness());
	setField("Currency", alliance->GetCurrency().c_str() );

    return 1;
}


int Lua::getModelInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (modelName)", n);
	string modelName = (string)luaL_checkstring(L,1);
	Model *model = Models::Instance()->GetModel(modelName);

    lua_newtable(L);
	setField("Name", model->GetName().c_str());
	setField("Image", model->GetImage()->GetPath().c_str());
	setField("Mass", model->GetMass());
	setField("Thrust", model->GetThrustOffset());
	setField("Rotation", model->GetRotationsPerSecond());
	setField("MaxSpeed", model->GetMaxSpeed());
	setField("MaxHull", model->getMaxEnergyAbsorption());
	setField("MSRP", model->GetMSRP());

	return 1;
}

int Lua::getPlanetInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (planetID)", n);

	// Figure out which planet we're fetching
	Planet* p = NULL;
	if( lua_isnumber(L,1)){
		int id = luaL_checkinteger(L,1);
		Sprite* sprite = SpriteManager::Instance()->GetSpriteByID(id);
		if( sprite->GetDrawOrder() != DRAW_ORDER_PLANET)
			return luaL_error(L, "ID #%d does not point to a Planet", id);
		p = (Planet*)(sprite);
	} else if( lua_isstring(L,1)){
		string name = luaL_checkstring(L,1);
		p = Planets::Instance()->GetPlanet(name);
		if( p == NULL )
			return luaL_error(L, "Cannot get Info for nonexistant Planet named '%s'", name.c_str() );
	} else {
		return luaL_error(L, "Cannot get Info Planet because of bad arguments.  Expected Name or ID" );
	}

	// Populate the Info Table.
    lua_newtable(L);
	setField("Name", p->GetName().c_str());
	setField("X", static_cast<float>(p->GetWorldPosition().GetX()));
	setField("Y", static_cast<float>(p->GetWorldPosition().GetY()));
	setField("Image", p->GetImage()->GetPath().c_str());
	setField("Alliance", p->GetAlliance().c_str());
	setField("Traffic", p->GetTraffic());
	setField("Militia", p->GetMilitiaSize());
	setField("Landable", p->GetLandable());
	setField("Influence", p->GetInfluence());
	lua_pushstring(L, "Technologies");
	pushComponents(L, (list<Component*>*) &p->GetTechnologies() );
	lua_settable(L, -3);
	return 1;
}

int Lua::getWeaponInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (weaponName)", n);
	string weaponName = (string)luaL_checkstring(L,1);
	Weapon* weapon = Weapons::Instance()->GetWeapon(weaponName);
	if( weapon == NULL)
		return luaL_error(L, "There is no weapon named '%s'.", weaponName.c_str());

    lua_newtable(L);
	setField("Name", weapon->GetName().c_str());
	setField("Image", weapon->GetImage()->GetPath().c_str());
	setField("Picture", weapon->GetPicture()->GetPath().c_str());
	setField("Payload", weapon->GetPayload());
	setField("Velocity", weapon->GetVelocity());
	setField("Acceleration", weapon->GetAcceleration());
	setField("FireDelay", weapon->GetFireDelay());
	setField("Lifetime", weapon->GetLifetime());
	setField("MSRP", weapon->GetMSRP());
	setField("Type", weapon->GetType());
	setField("Ammo Type", weapon->GetAmmoType());
	setField("Ammo Consumption", weapon->GetAmmoConsumption());
	setField("Sound", weapon->sound->GetPath().c_str());
	return 1;
}

int Lua::getEngineInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (weaponName)", n);
	string engineName = (string)luaL_checkstring(L,1);
	Engine* engine = Engines::Instance()->GetEngine(engineName);
	if( engine == NULL)
		return luaL_error(L, "There is no engine named '%s'.", engineName.c_str());

    lua_newtable(L);
	setField("Name", engine->GetName().c_str());
	setField("Picture", engine->GetPicture()->GetPath().c_str());
	setField("Force", engine->GetForceOutput());
	setField("Animation", engine->GetFlareAnimation().c_str());
	setField("MSRP", engine->GetMSRP());
	setField("Fold Drive", engine->GetFoldDrive());
	setField("Sound", engine->thrustsound->GetPath().c_str());
	return 1;
}

int Lua::getTechnologyInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (techName)", n);
	string techName = (string)luaL_checkstring(L,1);
	Technology* tech = Technologies::Instance()->GetTechnology(techName);
	if( tech == NULL)
		return luaL_error(L, "There is no technology named '%s'.", techName.c_str());

    lua_createtable(L, 3, 0);
    int newTable = lua_gettop(L);
	
	// Push the Models Table
	pushComponents(L, (list<Component*>*) &tech->GetModels() );
	lua_rawseti(L, newTable, 1);

	// Push the Weapons Table
	pushComponents(L, (list<Component*>*) &tech->GetWeapons() );
	lua_rawseti(L, newTable, 2);

	// Push the Engines Table
	pushComponents(L, (list<Component*>*) &tech->GetEngines() );
	lua_rawseti(L, newTable, 3);

	return 1;
}

int Lua::setInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( !(n==2||n==5)  )
		return luaL_error(L, "Got %d arguments expected 1 (infoType,infoTable)", n);
	string kind = luaL_checkstring(L,1);

	if(kind == "Alliance"){
        string name = getStringField(2,"Name");
        int attack = getIntField(2,"AttackSize");
        float aggressiveness = getNumField(2,"Aggressiveness");
        string currency = getStringField(2,"Currency");

		Alliance* thisAlliance = new Alliance(name,attack,aggressiveness,currency);
		Alliances::Instance()->AddOrReplace( thisAlliance );

	} else if(kind == "Engine"){
		string name = getStringField(2,"Name");
		string picture = getStringField(2,"Picture");
		int force = getIntField(2,"Force");
		string flare = getStringField(2,"Animation");
		int msrp = getIntField(2,"MSRP");
		int foldDrive = getIntField(2,"Fold Drive");
		string soundName = getStringField(2,"Sound");

		Engine* thisEngine = new Engine(name,Sound::Get(soundName),static_cast<float>(force),msrp,TO_BOOL(foldDrive),flare,Image::Get(picture));
		Engines::Instance()->AddOrReplace( thisEngine );

	} else if(kind == "Model"){
		string name = getStringField(2,"Name");
		string imageName = getStringField(2,"Image");
		float mass = getNumField(2,"Mass");
		int thrust = getIntField(2,"Thrust");
		float rot = getNumField(2,"Rotation");
		float speed = getNumField(2,"MaxSpeed");
		int hull = getIntField(2,"MaxHull");
		int msrp = getIntField(2,"MSRP");

		Model* thisModel = new Model(name,Image::Get(imageName),mass,thrust,rot,speed,hull,msrp);
		Models::Instance()->AddOrReplace(thisModel);

	} else if(kind == "Planet"){
		string name = getStringField(2,"Name");
		int x = getIntField(2,"X");
		int y = getIntField(2,"Y");
		string imageName = getStringField(2,"Image");
		string alliance = getStringField(2,"Alliance");
		int traffic = getIntField(2,"Traffic");
		int militia = getIntField(2,"Militia");
		int landable = getIntField(2,"Landable");
		int influence = getIntField(2,"Influence");
		list<string> techNames = getStringListField(2,"Technologies");

		// Process the Tech List
		list<Technology*> techs;
		list<string>::iterator i;
		for(i = techNames.begin(); i != techNames.end(); ++i) {
			if( NULL != Technologies::Instance()->GetTechnology(*i) ) {
				 techs.push_back( Technologies::Instance()->GetTechnology(*i) );
			} else {
				return luaL_error(L, "Could not create planet: there is no Technology Group '%s'.",(*i).c_str());
			}
		}

		if(Image::Get(imageName)==NULL){
			return luaL_error(L, "Could not create planet: there is no Image at '%s'.",imageName.c_str());
		}

		Planet thisPlanet(name,TO_FLOAT(x),TO_FLOAT(y),Image::Get(imageName),alliance,TO_BOOL(landable),traffic,militia,influence,techs);

		Planet* oldPlanet = Planets::Instance()->GetPlanet(name);
		if(oldPlanet!=NULL) {
			Log::Message("Saving changes to '%s'",thisPlanet.GetName().c_str());
			*oldPlanet = thisPlanet;
		} else {
			Log::Message("Creating new Planet '%s'",thisPlanet.GetName().c_str());
			Planet* newPlanet = new Planet(thisPlanet);
			Planets::Instance()->Add(newPlanet);
			SpriteManager::Instance()->Add(newPlanet);
		}

	} else if(kind == "Technology"){
		list<string>::iterator iter;
		list<Model*> models;
		list<Weapon*> weapons;
		list<Engine*> engines;

		string name = luaL_checkstring(L,2);

		list<string> modelNames = getStringListField(3);
		for(iter=modelNames.begin();iter!=modelNames.end();++iter){
			if(Models::Instance()->GetModel(*iter))
				models.push_back( Models::Instance()->GetModel(*iter) );
		}

		list<string> weaponNames = getStringListField(4);
		for(iter=weaponNames.begin();iter!=weaponNames.end();++iter){
			if(Weapons::Instance()->GetWeapon(*iter))
				weapons.push_back( Weapons::Instance()->GetWeapon(*iter) );
		}

		list<string> engineNames = getStringListField(5);
		for(iter=engineNames.begin();iter!=engineNames.end();++iter){
			if(Engines::Instance()->GetEngine(*iter))
				engines.push_back( Engines::Instance()->GetEngine(*iter) );
		}

		Technology* thisTechnology = new Technology(name,models,engines,weapons);
		Technologies::Instance()->AddOrReplace( thisTechnology );

		return 0;
	} else if(kind == "Weapon"){
		string name = getStringField(2,"Name");
		string imageName = getStringField(2,"Image");
		string pictureName = getStringField(2,"Picture");
		int payload = getIntField(2,"Payload");
		int velocity = getIntField(2,"Velocity");
		int acceleration = getIntField(2,"Acceleration");
		int fireDelay = getIntField(2,"FireDelay");
		int lifetime = getIntField(2,"Lifetime");
		int msrp = getIntField(2,"MSRP");
		int type = getIntField(2,"Type");
		int ammoType = getIntField(2,"Ammo Type");
		int ammoConsumption = getIntField(2,"Ammo Consumption");
		string soundName = getStringField(2,"Sound");

		Weapon* thisWeaon = new Weapon(name,Image::Get(imageName),Image::Get(pictureName),type,payload,velocity,acceleration,ammoType,ammoConsumption,fireDelay,lifetime,Sound::Get(soundName),msrp);
		Weapons::Instance()->AddOrReplace( thisWeaon );

	} else {
		return luaL_error(L, "Cannot set Info for kind '%s' must be one of {Alliance, Engine, Model, Planet, Technology, Weapon} ",kind.c_str());
	}
	return 0;
}

int Lua::saveComponents(lua_State *L) {
    Alliances::Instance()->Save("Resources/Definitions/alliances-default.xml");
    Models::Instance()->Save("Resources/Definitions/models-default.xml");
    Weapons::Instance()->Save("Resources/Definitions/weapons-default.xml");
    Engines::Instance()->Save("Resources/Definitions/engines-default.xml");
    Planets::Instance()->Save("Resources/Definitions/planets-default.xml");
    Technologies::Instance()->Save("Resources/Definitions/technologies-default.xml");
    return 0;
}

int Lua::listImages(lua_State *L) {
	list<string> pics = Filesystem::Enumerate("Resources/Graphics/",".png");
	pushNames(L,&pics);
	return 1;
}

