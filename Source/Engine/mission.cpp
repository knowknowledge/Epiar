/**\file			mission.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Tuesday, August 24, 2010
 * \date			Modified: Tuesday, August 24, 2010
 * \brief			
 * \details
 */

#include "includes.h"
#include "Engine/mission.h"
#include "Utilities/lua.h"
#include "Utilities/log.h"
#include "Utilities/components.h"

/**\brief Mission Constructor
 */
Mission::Mission(string _type, int _tableReference)
	:type(_type)
	,tableReference(_tableReference)
{
}

/**\brief Mission Destructor
 */
Mission::~Mission()
{
	lua_State *L = Lua::CurrentState();
	lua_pushinteger(L, tableReference);
	luaL_ref(L, LUA_REGISTRYINDEX);
}


bool Mission::ValidateMission( string type, int tableReference){
	int i;
	
	// The Functions that every mission type must have
	const char *requiredFunctions[] = {
		"Create",
		"Accept",
		"Reject",
		"Update",
		"Success",
		"Failure",
	};
	const int NUM_FUNCTIONS = sizeof(requiredFunctions) / sizeof(requiredFunctions[0]);

	// The Attributes that every mission Table must have
	const char *requiredInformation[] = {
		"Name",
		"Description",
	};
	const int NUM_INFORMATION = sizeof(requiredInformation) / sizeof(requiredInformation[0]);

	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	// Check that this mission Type exists
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L, lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'!", type.c_str() );
		lua_settop(L, initialStackTop);
		return false;
	}

	// Check that this Mission defines each of the required functions
	for( i=0; i<NUM_FUNCTIONS; ++i )
	{
		lua_pushstring(L, requiredFunctions[i] );
		lua_gettable(L, initialStackTop + 1);
		if( ! lua_isfunction(L, lua_gettop(L)) )
		{
			LogMsg(ERR, "The Mission '%s' doesn't have a %s function!", type.c_str(), requiredFunctions[i] );
			Lua::stackDump(L);
			lua_settop(L, initialStackTop );
			return false;
		}
		lua_pop(L,1);
	}

	// Check that the Mission Table exists
	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
	if( ! lua_istable(L, lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Table at reference '%d'!", tableReference );
		Lua::stackDump(L);
		lua_settop(L, initialStackTop );
		Lua::stackDump(L);
		return false;
	}

	// Check that the Mission Table has a Name and Description
	for( i=0; i<NUM_INFORMATION; ++i )
	{
		// Check that this Mission defines each of the required functions
		lua_pushstring(L, requiredInformation[i] );
		lua_gettable(L, initialStackTop + 2);
		if( ! lua_isstring(L, lua_gettop(L)) )
		{
			LogMsg(ERR, "The Mission '%s' doesn't have a %s!", type.c_str(), requiredInformation[i] );
			Lua::stackDump(L);
			lua_settop(L, initialStackTop );
			return false;
		} else {
			LogMsg(INFO, "The Mission %s: %s", requiredInformation[i], luaL_checkstring(L, lua_gettop(L)) );
		}
		lua_pop(L,1);
	}

	lua_settop(L, initialStackTop );
	return true;
}

/**\brief 
 * \returns True on Error
 */
bool Mission::Reject()
{
	LogMsg(INFO, "Rejecting");
	return (false == RunFunction( "Reject", true ) );
}

bool Mission::Accept()
{
	LogMsg(INFO, "Accepting");
	return (false == RunFunction( "Accept", true ) );
}

/**\brief 
 * \returns True if the Mission is over (success, failure, or error) and should be deleted.
 */
bool Mission::Update()
{
	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	if(! RunFunction( "Update", false ) )
	{
		lua_settop(L,initialStackTop);
		return true; // The Update failed, delete this Mission
	}

	// A return value is optional
	if( lua_isboolean(L, lua_gettop(L)) )
	{
		int success = lua_toboolean(L, lua_gettop(L));
		const char* completionFunctions[] = {
			"Failure",
			"Success",
		};

		LogMsg(INFO, "The Mission '%s' is a %s", GetName().c_str(), completionFunctions[ success ] );

		if( ! RunFunction( completionFunctions[ success ], true ) )
		{
			LogMsg(ERR,"Failed to run %s.%s: %s\n", type.c_str(), completionFunctions[success], lua_tostring(L, -1));
		}
		lua_settop(L,initialStackTop);
		return true;
	}

	lua_settop(L,initialStackTop);
	return false;
}

void Mission::PushMissionTable()
{
	lua_State *L = Lua::CurrentState();
	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
}

/**\brief
 * \returns Success Boolean if the function was successfully called
 */
bool Mission::RunFunction(string functionName, bool clearStack)
{
	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	// Get the Mission
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'!", type.c_str() );
		if( clearStack ) lua_settop(L,initialStackTop);
		return false; // Invalid Mission, Delete it
	}

	// Get the Update 
	lua_pushstring(L, functionName.c_str() );
	lua_gettable(L,initialStackTop + 1);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "The Mission Type named '%s' cannot %s!", type.c_str(), functionName.c_str() );
		if( clearStack ) lua_settop(L,initialStackTop);
		return false; // Invalid Mission, Delete it
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
	
	// Call the Update function
	if( lua_pcall(L, 1, LUA_MULTRET, 0) != 0)
	{
		LogMsg(ERR,"Failed to run %s.%s: %s\n", type.c_str(), functionName.c_str(), lua_tostring(L, -1));
		if( clearStack ) lua_settop(L,initialStackTop);
		return false; // Invalid Mission, Delete it
	}


	if( clearStack ) lua_settop(L,initialStackTop);
	return true;
}

/**\brief Lookup String Attributes from the Mission Table
 */
string Mission::GetStringAttribute( string attribute )
{
	int MissionTableIndex;
	string value = "";
	lua_State *L = Lua::CurrentState();

	PushMissionTable();
	MissionTableIndex = lua_gettop(L);
	lua_pushstring( L, attribute.c_str() );
	lua_gettable(L,MissionTableIndex);
	if( lua_isstring(L, lua_gettop(L)) )
	{
		value = (string)luaL_checkstring(L, lua_gettop(L));
		printf("%s of this Mission: %s.\n", type.c_str(), value.c_str());
	} else {
		// TODO: Error!
	}
	lua_pop(L, 2); // Table and value

	return value;
}

Mission* Mission::FromXMLNode( xmlDocPtr doc, xmlNodePtr node )
{
	xmlNodePtr  attr;
	string _type;
	int _tableReference;
	lua_State *L = Lua::CurrentState();
	
	if( (attr = FirstChildNamed(node, "type"))){
		_type = NodeToString(doc,attr);
	} else return NULL;

	if( (attr = FirstChildNamed(node, "table"))){
		string serializedTable = NodeToString(doc,attr);
		printf("Serialized Table: %s\n", serializedTable.c_str() );
		Lua::Run( string("print( ") + serializedTable + " )");
		//Lua::Run( serializedTable);
		//_tableReference = luaL_ref(L, LUA_REGISTRYINDEX); // Gets and pops the top of the stack, which should have the the missionTable.
	} else return NULL;


	if( !Mission::ValidateMission(_type, _tableReference) ) {
		return NULL;
	}

	return new Mission( _type, _tableReference);
}

xmlNodePtr Mission::ToXMLNode()
{
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST "Mission" );

	xmlNewChild(section, NULL, BAD_CAST "type", BAD_CAST type.c_str() );

	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	lua_getglobal(L, "serialize" );
	PushMissionTable();
	if( lua_pcall(L, 1, 1, 0) != 0)
	{
		LogMsg(ERR,"Failed to Update %s: %s\n", type.c_str(), lua_tostring(L, -1));
		lua_settop(L, initialStackTop);
		return section; // Invalid Mission
	}

	printf("TABLE: %s",  lua_tostring(L, lua_gettop(L) ) );
	
	xmlNewChild(section, NULL, BAD_CAST "table", BAD_CAST lua_tostring(L, lua_gettop(L) ) );
	
	return section;
}

