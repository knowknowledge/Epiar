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
				*va_arg(vl, const char **) = lua_tostring(L, nres);
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
	val = luaL_checkint(L,index+1);
	lua_pop(L,1);
	return val;
}

float Lua::getNumField(int index, const char* name) {
	float val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = static_cast<float>(luaL_checknumber(L,index+1));
	lua_pop(L,1);
	return val;
}

string Lua::getStringField(int index, const char* name) {
	string val;
	assert(lua_istable(L,index));
	lua_pushstring(L, name);
	assert(lua_istable(L,index));
	lua_gettable(L, index);
	val = luaL_checkstring(L,index+1);
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

	if(sprite==NULL){
		Log::Error("Lua requested sprite with unknown id %d",id);
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
	setField("Mass", model->GetMass());
	setField("Thrust", model->GetThrustOffset());
	setField("Engine", model->GetEngine()->GetName().c_str() );
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
	setField("Acceleration", weapon->GetAcceleration());
	setField("FireDelay", weapon->GetFireDelay());
	setField("Lifetime", weapon->GetLifetime());
	setField("MSRP", weapon->GetMSRP());
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
	setField("Force", engine->GetForceOutput());
	setField("Animation", engine->GetFlareAnimation().c_str());
	setField("MSRP", engine->GetMSRP());
	setField("Fold Drive", engine->GetFoldDrive());
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
	
	// The info for a technology is a nested table:

	// Push the Main Table
	int i;
	list<Model*>::iterator iter_m;
	list<Weapon*>::iterator iter_w;
	list<Engine*>::iterator iter_e;

	// Push the Models Table
	list<Model*> models = tech->GetModels();
    lua_createtable(L,models.size(),0);
	int modeltable = lua_gettop(L);
	for(i=1,iter_m=models.begin();iter_m!=models.end();++iter_m,++i) {
		lua_pushstring(L, (*iter_m)->GetName().c_str() );
        lua_rawseti(L, modeltable, i);
	}

	// Push the Weapons Table
	list<Weapon*> weapons = tech->GetWeapons();
    lua_newtable(L);
	int weapontable = lua_gettop(L);
	for(i=1,iter_w=weapons.begin();iter_w!=weapons.end();++iter_w,++i) {
		lua_pushstring(L, (*iter_w)->GetName().c_str() );
        lua_rawseti(L, weapontable, i);
	}

	// Push the Engines Table
	list<Engine*> engines = tech->GetEngines();
    lua_newtable(L);
	int enginetable = lua_gettop(L);
	for(i=1,iter_e=engines.begin();iter_e!=engines.end();++iter_e,++i) {
		lua_pushstring(L, (*iter_e)->GetName().c_str() );
        lua_rawseti(L, enginetable, i);
	}

	return 3;
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
		
        Alliance* oldAlliance = Alliances::Instance()->GetAlliance(name);
        if(oldAlliance==NULL) return 0; // If the name changes then the below doesn't work.
        *oldAlliance = Alliance(name,attack,aggressiveness,currency,oldAlliance->GetIlligalCargos());

	} else if(kind == "Engine"){
		string name = getStringField(2,"Name");
		int force = getIntField(2,"Force");
		string flare = getStringField(2,"Animation");
		int msrp = getIntField(2,"MSRP");
		int foldDrive = getIntField(2,"Fold Drive");

		Engine* oldEngine = Engines::Instance()->GetEngine(name);
		if(oldEngine==NULL) return 0; // If the name changes then the below doesn't work.
		*oldEngine = Engine(name,oldEngine->thrustsound,static_cast<float>(force),msrp,TO_BOOL(foldDrive),flare);

	} else if(kind == "Model"){
		string name = getStringField(2,"Name");
		float mass = getNumField(2,"Mass");
		int thrust = getIntField(2,"Thrust");
		string engine = getStringField(2,"Engine");
		float rot = getNumField(2,"Rotation");
		float speed = getNumField(2,"MaxSpeed");
		int hull = getIntField(2,"MaxHull");
		int msrp = getIntField(2,"MSRP");

		Model* oldModel = Models::Instance()->GetModel(name);
		if(oldModel==NULL) return 0; // If the name changes then the below doesn't work.
		Model newModel(name,oldModel->GetImage(),oldModel->GetEngine(),mass,thrust,rot,speed,hull,msrp);
		*oldModel = newModel;

	} else if(kind == "Planet"){
		string name = getStringField(2,"Name");
		string alliance = getStringField(2,"Alliance");
		int traffic = getIntField(2,"Traffic");
		int militia = getIntField(2,"Militia");
		int landable = getIntField(2,"Landable");

		Planet* oldPlanet = Planets::Instance()->GetPlanet(name);
		if(oldPlanet==NULL) return 0; // If the name changes then the below doesn't work.
		*oldPlanet = Planet(name,alliance,TO_BOOL(landable),traffic,militia,oldPlanet->GetInfluence(), oldPlanet->GetMilitia(), oldPlanet->GetTechnologies());

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

		Technology* oldTechnology = Technologies::Instance()->GetTechnology(name);
		if(oldTechnology==NULL) return 0; // If the name changes then the below doesn't work.
		*oldTechnology = Technology(name,models,engines,weapons);

		return 0;
	} else if(kind == "Weapon"){
		string name = getStringField(2,"Name");
		int payload = getIntField(2,"Payload");
		int velocity = getIntField(2,"Velocity");
		int acceleration = getIntField(2,"Acceleration");
		int fireDelay = getIntField(2,"FireDelay");
		int lifetime = getIntField(2,"Lifetime");
		int msrp = getIntField(2,"MSRP");

		Weapon* oldWeapon = Weapons::Instance()->GetWeapon(name);
		if(oldWeapon==NULL) return 0; // If the name changes then the below doesn't work.
		*oldWeapon = Weapon(name,oldWeapon->GetImage(),oldWeapon->GetPicture(),oldWeapon->GetType(),payload,velocity,acceleration,oldWeapon->GetAmmoType(),oldWeapon->GetAmmoConsumption(),fireDelay,lifetime,oldWeapon->sound,msrp);

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
