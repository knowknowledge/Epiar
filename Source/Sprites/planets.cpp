/**\file			planets.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Monday, November 16 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Sprites/planets.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"
#include "Utilities/lua.h"
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Engine/alliances.h"
#include "Sprites/spritemanager.h"

/**\class Planet
 * \brief A Planet.
 *
 * Planets are where ships congregate.  They have things to buy and sell once you land on them.
 *
 *\see Planets
 *\see Planets_Lua
 */

/**\brief Blank Constructor
 */
Planet::Planet(){
	SetRadarColor(Color::Get(48, 160, 255));
}

/**\brief Copy Constructor
 */
Planet& Planet::operator=(const Planet& other) {
	// Check the other Sprite
	assert( other.GetImage() );

	name = other.name;
	alliance = other.alliance;
	landable = other.landable;
	traffic = other.traffic;
	militiaSize = other.militiaSize;
	sphereOfInfluence = other.sphereOfInfluence;
	technologies = other.technologies;

	// Set the Sprite Stuff
	Coordinate pos;
	pos.SetX(other.GetWorldPosition().GetX());
	pos.SetY(other.GetWorldPosition().GetY());
	SetWorldPosition( pos );
	SetImage( other.GetImage() );
	Image::Store(name,GetImage());

	return *this;
}

/**\brief Constructor using a full Full Description
 */
Planet::Planet( string _name, float _x, float _y, Image* _image, string _alliance, bool _landable, int _traffic, int _militiaSize, int _sphereOfInfluence, list<Technology*> _technologies):
	alliance(_alliance),
	landable(_landable),
	traffic(_traffic),
	militiaSize(_militiaSize),
	sphereOfInfluence(_sphereOfInfluence),
	technologies(_technologies)
{
	// Check the inputs
	assert(_image);

	Coordinate pos;
	pos.SetX(_x);
	pos.SetY(_y);
	SetWorldPosition( pos );
	SetName(_name);
	SetImage(_image);
	Image::Store(name,GetImage());
	SetRadarColor(Color::Get(48, 160, 255));
}

/**\brief Destructor
 */
Planet::~Planet() {
}

/**\brief Parse one player out of an xml node.
 */
bool Planet::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "alliance" ) {
		alliance = value;
	} else PPA_MATCHES( "x" ) {
		Coordinate pos = GetWorldPosition();
		pos.SetX( (double)atof( value.c_str() ) );
		SetWorldPosition( pos );
	} else PPA_MATCHES( "y" ) {
		Coordinate pos = GetWorldPosition();
		pos.SetY( (double)atof( value.c_str() ) );
		SetWorldPosition( pos );
	} else PPA_MATCHES( "landable" ) {
		landable = (atoi( value.c_str() ) !=0);
	} else PPA_MATCHES( "traffic" ) {
		traffic = (short int)atoi( value.c_str() );
	} else PPA_MATCHES( "image" ) {
		Image *image = Image::Get( value );
		Image::Store(name,Image::Get(value));
		SetImage( image );
	} else PPA_MATCHES( "militia" ) {
		militiaSize = (short int)atoi( value.c_str() );
	} else PPA_MATCHES( "sphereOfInfluence" ) {
		sphereOfInfluence = atoi( value.c_str() );
	} else PPA_MATCHES( "technology" ) {
		Technology *tech = Technologies::Instance()->GetTechnology( value );
		technologies.push_back(tech);
		technologies.unique();
	}
	return true;
}

/**\brief Debug Printing
 */
void Planet::_dbg_PrintInfo( void ) {
	//cout << "Planet: " << name << " at (" << GetWorldPosition() << ") under alliance " << alliance << " with landable option set to " << landable << " and average traffic count of " << traffic << " ships" << endl;
}

/**\brief List of the Models that are available at this Planet
 */
list<Model*> Planet::GetModels() {
	list<Model*> models;
	list<Technology*>::iterator techiter;
	list<Model*>::iterator listiter;
	for(techiter=technologies.begin(); techiter!=technologies.end(); ++techiter) {
		list<Model*> model_list = (*techiter)->GetModels();
		for(listiter=model_list.begin(); listiter!=model_list.end(); ++listiter) {
			models.push_back( *listiter );
		}
	}
	models.unique();
	return models;
}

/**\brief List of the Engines that are available at this Planet
 */
list<Engine*> Planet::GetEngines() {
	list<Engine*> engines;
	list<Technology*>::iterator techiter;
	list<Engine*>::iterator listiter;
	for(techiter=technologies.begin(); techiter!=technologies.end(); ++techiter) {
		list<Engine*> engine_list = (*techiter)->GetEngines();
		for(listiter=engine_list.begin(); listiter!=engine_list.end(); ++listiter) {
			engines.push_back( *listiter );
		}
	}
	engines.unique();
	return engines;
}

/**\brief List of the Weapons that are available at this Planet
 */
list<Weapon*> Planet::GetWeapons() {
	list<Weapon*> weapons;
	list<Technology*>::iterator techiter;
	list<Weapon*>::iterator listiter;
	for(techiter=technologies.begin(); techiter!=technologies.end(); ++techiter) {
		list<Weapon*> weapon_list = (*techiter)->GetWeapons();
		for(listiter=weapon_list.begin(); listiter!=weapon_list.end(); ++listiter) {
			weapons.push_back( *listiter );
		}
	}
	weapons.unique();
	return weapons;
}

/**\brief Save this Planet to an xml node
 */
xmlNodePtr Planet::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "alliance", BAD_CAST this->GetAlliance().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)this->GetWorldPosition().GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "landable", BAD_CAST (this->GetLandable()?"1":"0") );
	snprintf(buff, sizeof(buff), "%d", this->GetTraffic() );
	xmlNewChild(section, NULL, BAD_CAST "traffic", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST this->GetImage()->GetPath().c_str() );
	snprintf(buff, sizeof(buff), "%d", this->GetMilitiaSize() );
	xmlNewChild(section, NULL, BAD_CAST "militia", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetInfluence() );
	xmlNewChild(section, NULL, BAD_CAST "sphereOfInfluence", BAD_CAST buff );
	list<Technology*> techs = this->GetTechnologies();
	for( list<Technology*>::iterator it = techs.begin(); it!=techs.end(); ++it ){
		xmlNewChild(section, NULL, BAD_CAST "technology", BAD_CAST (*it)->GetName().c_str() );
	}

	return section;
}

/**\class Planets
 * \brief Collection of all Planets
 *
 *\see Planet
 *\see Planets_Lua
 */

Planets *Planets::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Planets instance.
 * \return Pointer to the Planets instance
 */
Planets *Planets::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Planets; // create the sold instance
		pInstance->rootName = "planets";
		pInstance->componentName = "planet";
	}
	return( pInstance );
}

/**\class Planets_Lua
 * \brief Lua Interface for dealing with Planets
 *
 *\see Planet
 *\see Planets
 */

/**\brief Load all Planet related Lua functions
 */
void Planets_Lua::RegisterPlanets(lua_State *L){
	static const luaL_Reg PlanetFunctions[] = {
		// Normally we would put a "new" function here.
		// Lua may not ever need to create planets though.
		{"Get", &Planets_Lua::Get},
		{"NewPlanet", &Planets_Lua::NewPlanet},
		{NULL, NULL}
	};

	static const luaL_Reg PlanetMethods[] = {
		{"GetName", &Planets_Lua::GetName},
		{"GetID", &Planets_Lua::GetID},
		{"GetType", &Planets_Lua::GetType},
		{"GetPosition", &Planets_Lua::GetPosition},
		{"Alliance", &Planets_Lua::GetAlliance},
		{"Traffic", &Planets_Lua::GetTraffic},
		{"MilitiaSize", &Planets_Lua::GetMilitiaSize},
		{"Influence", &Planets_Lua::GetInfluence},
		{"Landable", &Planets_Lua::GetLandable},
		{"GetModels", &Planets_Lua::GetModels},
		{"GetEngines", &Planets_Lua::GetEngines},
		{"GetWeapons", &Planets_Lua::GetWeapons},
		{NULL, NULL}
	};
	luaL_newmetatable(L, EPIAR_PLANET);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, PlanetMethods,0);
	luaL_openlib(L, EPIAR_PLANET, PlanetFunctions,0);
}

/**\brief Get a planet by name or id
 */
int Planets_Lua::Get(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if( n!=1 ){
		return luaL_error(L, "Got %d arguments expected 1 (id or name)", n);
	}
    Planet* p = NULL;
    if(lua_isstring(L,1)) {
        string name = (string)luaL_checkstring(L,1);
        p = (Planet*)Planets::Instance()->GetPlanet(name);
        if (p==NULL){
            return luaL_error(L, "There is no planet by the name of '%s'", name.c_str());
        }
    } else if(lua_isnumber(L,1)) {
        int id = (int)luaL_checkinteger(L,1);
        p = (Planet*)SpriteManager::Instance()->GetSpriteByID(id);
        if (p==NULL || p->GetDrawOrder() != DRAW_ORDER_PLANET){
            return luaL_error(L, "There is no planet with ID %d", id);
        }
    } else {
        return luaL_error(L, "Cannot get planet with these arguments.  Expected id or name.");
    }
    Lua::pushSprite(L,p);
    return 1;
}

/**\brief Create a new Planet
 */
int Planets_Lua::NewPlanet(lua_State* L){
	int i,n = lua_gettop(L);  // Number of arguments

	// Temporary intermediatary variables
	string imageName, techName;
	Technology* tech;

	// The new planet object and vital attributes
	Planet *p;
	string _name;
	float _x;
	float _y;
	Image* _image;
	string _alliance;
	bool _landable;
	int _traffic;
	int _militiaSize;
	int _sphereOfInfluence;
	list<Technology*> _technologies;

	if(n<9) {
		return luaL_error(L, "Got %d arguments expected at least 9 (name, x, y, image_name, alliance, landable, traffic, militia, influence, [techs...])", n);
	}

	// Capture the required arguments
	_name = (string)luaL_checkstring(L,1);
	_x = luaL_checknumber(L, 2 );
	_y = luaL_checknumber(L, 3 );
	imageName = (string)luaL_checkstring(L,4);
	_image = Image::Get( imageName );
	_alliance = (string)luaL_checkstring(L,5);
	_landable = (bool)luaL_checkint(L, 6 );
	_traffic = luaL_checkint(L, 7 );
	_militiaSize = luaL_checkint(L, 8 );
	_sphereOfInfluence = luaL_checkint(L, 9 );

	// Capture all other arguments as technologies
	for(i=10;i<=n;i++) {
		techName = (string)luaL_checkstring(L, 10 );
		tech = Technologies::Instance()->GetTechnology( techName );
		if(tech==NULL) {
			return luaL_error(L, "No Technology '%s'", techName.c_str());
		}
		_technologies.push_back( tech );
	}

	if(_image==NULL) {
		return luaL_error(L, "No image '%s'", imageName.c_str());
	}

	p = new Planet(
		_name,
		_x,
		_y,
		_image,
		_alliance,
		_landable,
		_traffic,
		_militiaSize,
		_sphereOfInfluence,
		_technologies
		);

	// Save the Planet!
	Image::Store(_name,_image);
	SpriteManager::Instance()->Add((Sprite*)p);
	Planets::Instance()->AddOrReplace((Component*)p);
    Lua::pushSprite(L,p);
	return 1;
}

/*
Planet **Planets_Lua::pushPlanet(lua_State *L){
	Planet **s = (Planet **)lua_newuserdata(L, sizeof(Planet*));
	*s = new Planet();
	luaL_getmetatable(L, EPIAR_PLANET);
	lua_setmetatable(L, -2);
	return s;
}
*/

/**\brief Check that the a Lua value really is a Planet
 */
Planet *Planets_Lua::checkPlanet(lua_State *L, int index){
	int *idptr;
	idptr = (int*)luaL_checkudata(L, index, EPIAR_PLANET);
	luaL_argcheck(L, idptr != NULL, index, "`EPIAR_PLANET' expected");

	Sprite* s;
	s = SpriteManager::Instance()->GetSpriteByID(*idptr);
	if ((s) == NULL) luaL_typerror(L, index, EPIAR_PLANET);
	if (0==((s)->GetDrawOrder() & DRAW_ORDER_PLANET)){
		luaL_typerror(L, index, EPIAR_PLANET);
	}
	return (Planet*)s;
}

/**\brief Get the Name of this planet
 */
int Planets_Lua::GetName(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushstring(L, planet->GetName().c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the ID of this planet
 */
int Planets_Lua::GetID(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushinteger(L, planet->GetID());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Type of this planet
 */
int Planets_Lua::GetType(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushinteger(L, planet->GetDrawOrder());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Position of this planet
 */
int Planets_Lua::GetPosition(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushnumber(L, planet->GetWorldPosition().GetX() );
		lua_pushnumber(L, planet->GetWorldPosition().GetY() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 2;
}

/**\brief Get the Alliance of this planet
 */
int Planets_Lua::GetAlliance(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushstring(L, planet->GetAlliance().c_str());
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Traffic of this planet
 */
int Planets_Lua::GetTraffic(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushnumber(L, planet->GetTraffic() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Militia Size of this planet
 */
int Planets_Lua::GetMilitiaSize(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushnumber(L, planet->GetMilitiaSize() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Sphere of Influence of this planet
 */
int Planets_Lua::GetInfluence(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushnumber(L, planet->GetInfluence() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get Landable boolean of this planet
 */
int Planets_Lua::GetLandable(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		lua_pushboolean(L, planet->GetLandable() );
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Models available at this planet
 */
int Planets_Lua::GetModels(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		list<Model*> models = planet->GetModels();
		list<Model*>::iterator iter;
		lua_createtable(L, models.size(), 0);
		int newTable = lua_gettop(L);
		int index = 1;
		for(iter=models.begin();iter!=models.end();++iter,++index){
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Engines available at this planet
 */
int Planets_Lua::GetEngines(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		list<Engine*> engines = planet->GetEngines();
		list<Engine*>::iterator iter;
		lua_createtable(L, engines.size(), 0);
		int newTable = lua_gettop(L);
		int index = 1;
		for(iter=engines.begin();iter!=engines.end();++iter,++index){
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

/**\brief Get the Weapons available at this planet
 */
int Planets_Lua::GetWeapons(lua_State* L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 1) {
		Planet* planet= checkPlanet(L,1);
		list<Weapon*> weapons = planet->GetWeapons();
		list<Weapon*>::iterator iter;
		lua_createtable(L, weapons.size(), 0);
		int newTable = lua_gettop(L);
		int index = 1;
		for(iter=weapons.begin();iter!=weapons.end();++iter,++index){
			lua_pushstring(L, (*iter)->GetName().c_str() );
			lua_rawseti(L, newTable, index);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 1 (self)", n);
	}
	return 1;
}

