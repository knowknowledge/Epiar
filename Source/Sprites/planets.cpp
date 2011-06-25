/**\file			planets.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Monday, November 16 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Sprites/planets.h"
#include "Utilities/log.h"
#include "Utilities/components.h"
#include "Utilities/lua.h"
#include "Utilities/timer.h"
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Engine/alliances.h"
#include "Engine/simulation_lua.h"
#include "Sprites/spritemanager.h"

/** \addtogroup Sprites
 * @{
 */

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
	Component::SetName("");
	SetRadarColor(Color(48, 160, 255));
	alliance = NULL;
	surface = NULL;
	landable = true;
	forbidden = false;
	traffic = 0;
	militiaSize = 0;
	sphereOfInfluence = 0;
	lastTrafficTime = 0;
}

/**\brief Copy Constructor
 */
Planet& Planet::operator=(const Planet& other) {
	// Check the other Sprite
	assert( other.GetImage() );
	assert( other.GetAlliance() );

	name = other.name;
	alliance = other.alliance;
	landable = other.landable;
	traffic = other.traffic;
	militiaSize = other.militiaSize;
	sphereOfInfluence = other.sphereOfInfluence;
	technologies = other.technologies;
	surface = other.surface;
	summary = other.summary;

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
Planet::Planet( string _name, float _x, float _y, Image* _image, Alliance* _alliance, bool _landable, int _traffic, int _militiaSize, int _sphereOfInfluence, list<Technology*> _technologies):
	alliance(_alliance),
	landable(_landable),
	traffic(_traffic),
	militiaSize(_militiaSize),
	sphereOfInfluence(_sphereOfInfluence),
	technologies(_technologies),
	surface(NULL)
{
	// Check the inputs
	assert(_image);
	assert(_alliance);

	Coordinate pos;
	pos.SetX(_x);
	pos.SetY(_y);
	SetWorldPosition( pos );
	SetName(_name);
	SetImage(_image);
	Image::Store(name,GetImage());
	SetRadarColor(Color(48, 160, 255));
}

/**\brief Destructor
 */
Planet::~Planet() {
}

/**\brief Parse one planet out of an xml node.
 */
bool Planet::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;
	Coordinate pos;

	if( (attr = FirstChildNamed(node,"alliance")) ){
		value = NodeToString(doc,attr);
		alliance = Alliances::Instance()->GetAlliance(value);
		if(alliance==NULL)
		{
			LogMsg(ERR, "Could not create Planet '%s'. Unknown Alliance '%s'.", this->GetName().c_str(), value.c_str());
			return false;
		}
	} else return false;

	if( (attr = FirstChildNamed(node,"x")) ){
		value = NodeToString(doc,attr);
		pos.SetX( atof( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"y")) ){
		value = NodeToString(doc,attr);
		pos.SetY( atof( value.c_str() ));
	} else return false;

	SetWorldPosition( pos );

	if( (attr = FirstChildNamed(node,"landable")) ){
		value = NodeToString(doc,attr);
		landable = ( atoi( value.c_str() ) != 0);
	} else return false;

	if( (attr = FirstChildNamed(node,"traffic")) ){
		value = NodeToString(doc,attr);
		traffic = (short int) atoi( value.c_str() );
	} else return false;

	if( (attr = FirstChildNamed(node,"image")) ){
		Image* image = Image::Get( NodeToString(doc,attr) );
		Image::Store(name, image);
		SetImage(image);
	} else return false;

	if( (attr = FirstChildNamed(node,"surface-image")) ){
		this->surface = Image::Get( NodeToString(doc,attr) );
	} else return false;

	if( (attr = FirstChildNamed(node,"summary")) ){
		summary = NodeToString(doc,attr);
	} else return false;

	if( (attr = FirstChildNamed(node,"militia")) ){
		value = NodeToString(doc,attr);
		militiaSize = (short int) atoi( value.c_str() );
	} else return false;

	if( (attr = FirstChildNamed(node,"sphereOfInfluence")) ){
		value = NodeToString(doc,attr);
		sphereOfInfluence = atoi( value.c_str() );
	} else return false;

	for( attr = FirstChildNamed(node,"technology"); attr!=NULL; attr = NextSiblingNamed(attr,"technology") ){
		value = NodeToString(doc,attr);
		Technology *tech = Technologies::Instance()->GetTechnology( value );
		technologies.push_back(tech);
	}
	technologies.sort();
	technologies.unique();

	return true;
}

void Planet::Update( lua_State *L ) {
	if( lastTrafficTime + 120 < Timer::GetLogicalFrameCount() ) {
		GenerateTraffic( L );
	}
	Sprite::Update( L );
}

void Planet::GenerateTraffic( lua_State *L ) {
	SpriteManager *sprites = Simulation_Lua::GetSimulation(L)->GetSpriteManager();
	list<Sprite*> *nearbySprites = sprites->GetSpritesNear( GetWorldPosition(), TO_FLOAT(sphereOfInfluence), DRAW_ORDER_SHIP | DRAW_ORDER_PLAYER);

	if( nearbySprites->size() < traffic ) {
		Lua::Call( "createRandomShipForPlanet", "i", GetID() );
	}
	delete nearbySprites;
	lastTrafficTime = Timer::GetLogicalFrameCount();
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
	models.sort();
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
	engines.sort();
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
	weapons.sort();
	weapons.unique();
	return weapons;
}

/**\brief List of the Outfits that are available at this Planet
 */
list<Outfit*> Planet::GetOutfits() {
	list<Outfit*> outfits;
	list<Technology*>::iterator techiter;
	list<Outfit*>::iterator listiter;
	for(techiter=technologies.begin(); techiter!=technologies.end(); ++techiter) {
		list<Outfit*> outfit_list = (*techiter)->GetOutfits();
		for(listiter=outfit_list.begin(); listiter!=outfit_list.end(); ++listiter) {
			outfits.push_back( *listiter );
		}
	}
	outfits.sort();
	outfits.unique();
	return outfits;
}

/**\brief Save this Planet to an xml node
 */
xmlNodePtr Planet::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str() );

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "alliance", BAD_CAST GetAlliance()->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%d", (int)GetWorldPosition().GetX() );
	xmlNewChild(section, NULL, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", (int)GetWorldPosition().GetY() );
	xmlNewChild(section, NULL, BAD_CAST "y", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "landable", BAD_CAST (GetLandable()?"1":"0") );
	snprintf(buff, sizeof(buff), "%d", GetTraffic() );
	xmlNewChild(section, NULL, BAD_CAST "traffic", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST GetImage()->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "surface-image", BAD_CAST surface->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "summary", BAD_CAST summary.c_str() );
	snprintf(buff, sizeof(buff), "%d", GetMilitiaSize() );
	xmlNewChild(section, NULL, BAD_CAST "militia", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", GetInfluence() );
	xmlNewChild(section, NULL, BAD_CAST "sphereOfInfluence", BAD_CAST buff );
	list<Technology*> techs = GetTechnologies();
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
		pInstance = new Planets; // create the solid instance
		pInstance->rootName = "planets";
		pInstance->componentName = "planet";
	}
	return( pInstance );
}

/** @} */

