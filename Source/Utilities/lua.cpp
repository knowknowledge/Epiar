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
#include "Sprites/planets.h"
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
bool Lua::Call(const char *func, const char *sig, ...) {
	va_list vl;
	int narg, nres;  /* number of arguments and results */

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
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg, nres, 0) != 0)  /* do the call */
		luaL_error(L, "error running function `%s': %s",
				func, lua_tostring(L, -1));

	/* retrieve results */
	nres = -nres;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {

			case 'd':  /* double result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result type");
				*va_arg(vl, double *) = lua_tonumber(L, nres);
				break;

			case 'i':  /* int result */
				if (!lua_isnumber(L, nres))
					luaL_error(L, "wrong result type");
				*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
				break;

			case 's':  /* string result */
				if (!lua_isstring(L, nres))
					luaL_error(L, "wrong result type");
				*va_arg(vl, const char **) = lua_tostring(L, nres);
				break;

			default:
				luaL_error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
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
		{"player", &Lua::getPlayer},
		{"shakeCamera", &Lua::shakeCamera},
		{"focusCamera", &Lua::focusCamera},
		{"models", &Lua::getModelNames},
		{"weapons", &Lua::getWeaponNames},
		{"getSprite", &Lua::getSpriteByID},
		{"ships", &Lua::getShips},
		{"planets", &Lua::getPlanets},
		{"nearestShip", &Lua::getNearestShip},
		{"RegisterKey", &Input::RegisterKey},  
		{"UnRegisterKey", &Input::UnRegisterKey},  
		{"getModelInfo", &Lua::getModelInfo},
		{"getPlanetInfo", &Lua::getPlanetInfo},
		{"getWeaponInfo", &Lua::getWeaponInfo},
		{"getEngineInfo", &Lua::getEngineInfo},
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

int Lua::focusCamera(lua_State *L){
	if (lua_gettop(L) == 1) {
		int id = (int)(luaL_checkint(L,1));
		Camera *pInstance = Camera::Instance();
		SpriteManager *sprites= SpriteManager::Instance();
		Sprite* target = sprites->GetSpriteByID(id);
		if(target!=NULL)
			pInstance->Focus( target );
	}
	return 0;
}

int Lua::getWeaponNames(lua_State *L){
	Weapons *weapons= Weapons::Instance();
	list<string> *names = weapons->GetWeaponNames();
	pushNames(L,names);
	delete names;
    return 1;
}

int Lua::getModelNames(lua_State *L){
	Models *models = Models::Instance();
	list<string> *names = models->GetModelNames();
	pushNames(L,names);
	delete names;
    return 1;
}

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
		sprites = SpriteManager::Instance()->GetSpritesNear(Coordinate(x,y),static_cast<float>(r));
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

int Lua::getNearestShip(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=2 ){
		return luaL_error(L, "Got %d arguments expected 1 (ship, range)", n);
	}
	AI** ai = AI_Lua::checkShip(L,1);
	double r = luaL_checknumber (L, 2);
	Sprite **s;
	Sprite *closest = SpriteManager::Instance()->GetNearestSprite((*ai),r);
		if(closest!=NULL  && closest->GetDrawOrder()==DRAW_ORDER_SHIP){
		s = (Sprite **)AI_Lua::pushShip(L);
		*s = closest;
		cout<<"Lua pushed Sprite #"<<closest->GetID()<<endl;
		return 1;
	} else {
		return 0;
	}
}

int Lua::getModelInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (modelName)", n);
	string modelName = (string)luaL_checkstring(L,1);
	Model *model = Models::Instance()->GetModel(modelName);

    lua_newtable(L);
	setField("Name", model->GetName().c_str());
	//setField("mass", model->GetMass()); Why isn't there a GetMass function?
	setField("Thrust", model->GetThrustOffset());
	setField("Engine", model->GetEngine()->GetName().c_str() );
	setField("Rotation", model->GetRotationsPerSecond());
	setField("MaxSpeed", model->GetMaxSpeed());
	setField("MaxHull", model->getMaxEnergyAbsorption());
	return 1;
}

int Lua::getPlanetInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (planetID)", n);
	int id = luaL_checkinteger(L,1);
	Sprite* sprite = SpriteManager::Instance()->GetSpriteByID(id);
	if( sprite->GetDrawOrder() != DRAW_ORDER_PLANET)
		return luaL_error(L, "ID #%d does not point to a Planet", id);

	cPlanet* p = (cPlanet*)(sprite);

    lua_newtable(L);
	setField("Name", p->GetName().c_str());
	setField("Alliance", p->GetAlliance().c_str());
	setField("Traffic", p->GetTraffic());
	setField("Militia", p->GetMilitiaSize());
	setField("Landable", p->GetLandable());
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
	setField("Payload", weapon->GetPayload());
	setField("Velocity", weapon->GetVelocity());
	setField("FireDelay", weapon->GetFireDelay());
	setField("Lifetime", weapon->GetLifetime());
	return 1;
}

int Lua::getEngineInfo(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 )
		return luaL_error(L, "Got %d arguments expected 1 (weaponName)", n);
	string engineName = (string)luaL_checkstring(L,1);
	Engine* engine = Engines::Instance()->LookUp(engineName);
	if( engine == NULL)
		return luaL_error(L, "There is no engine named '%s'.", engineName.c_str());

    lua_newtable(L);
	setField("Name", engine->GetName().c_str());
	setField("Force", engine->GetForceOutput());
	return 1;
}

int Lua::setModelInfo(lua_State *L) {
	return 0; // TODO
}

int Lua::setPlanetInfo(lua_State *L) {
	return 0; // TODO
}

int Lua::setWeaponInfo(lua_State *L) {
	return 0; // TODO
}

int Lua::setEngineInfo(lua_State *L) {
	return 0; // TODO
}
