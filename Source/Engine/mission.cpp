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
	
}


bool Mission::ValidateMission( string type, int tableReference){
	int i;
	const int NUM_FUNCTIONS = 5;
	const char *requiredFunctions[NUM_FUNCTIONS] = {
		"Create",
		"Accept",
		"Update",
		"Success",
		"Failure",
	};

	const int NUM_INFORMATION = 2;
	const char *requiredInformation[NUM_INFORMATION] = {
		"Name",
		"Description",
	};

	lua_State *L = Lua::CurrentState();
	int initialStackTop = lua_gettop(L);

	// Check that this mission Type exists
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L, lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'!", type.c_str() );
		lua_settop(L, initialStackTop);
		return false;
	}

	for( i=0; i<NUM_FUNCTIONS; ++i )
	{
		// Check that this Mission defines each of the required functions
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
bool Mission::Accept()
{
	lua_State *L = Lua::CurrentState();
	int initialStackTop = lua_gettop(L);

	// Get the Mission
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'!", type.c_str() );
		return true; // Invalid Mission
	}

	// Get the Accept Function
	lua_pushstring(L, "Accept" );
	lua_gettable(L, initialStackTop + 1);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "The Mission Type named '%s' cannot update!", type.c_str() );
		lua_settop(L, initialStackTop);
		return true; // Invalid Mission
	}

	// Pass Argument(s)
	// The Mission Table 
	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
	
	// Call the Update function
	if( lua_pcall(L, 1, 0, 0) != 0)
	{
		LogMsg(ERR,"Failed to Update %s: %s\n", type.c_str(), lua_tostring(L, -1));
		lua_settop(L, initialStackTop);
		return true; // Invalid Mission
	}

	return false;
}

/**\brief 
 * \returns True if the Mission is over (success, failure, or error) and should be deleted.
 */
bool Mission::Update()
{
	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	// Get the Mission
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'!", type.c_str() );
		return true; // Invalid Mission, Delete it
	}

	// Get the Update 
	lua_pushstring(L, "Update" );
	lua_gettable(L,initialStackTop + 1);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(ERR, "The Mission Type named '%s' cannot update!", type.c_str() );
		return true; // Invalid Mission, Delete it
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
	
	// Call the Update function
	if( lua_pcall(L, 1, LUA_MULTRET, 0) != 0)
	{
		LogMsg(ERR,"Failed to Update %s: %s\n", type.c_str(), lua_tostring(L, -1));
		lua_settop(L,initialStackTop);
		return true; // Invalid Mission, Delete it
	}

	// A return value is optional
	// 
	if( lua_isboolean(L, lua_gettop(L)) )
	{
		int success = lua_toboolean(L, lua_gettop(L));
		const char* completionFunctions[2] = {
			"Failure",
			"Success",
		};

		LogMsg(INFO, "The Mission '%s' is a %s", GetName().c_str(), completionFunctions[ success ] );

		// Get the correct completion Function
		lua_pushstring(L, completionFunctions[ success ] );
		lua_gettable(L,1);

		// Check that it's a callable function
		if( ! lua_isfunction(L,lua_gettop(L)) )
		{
			LogMsg(ERR, "The Mission '%s' does not have a '%s' Function!", type.c_str(), completionFunctions[success] );
			lua_settop(L,initialStackTop);
			return true; // Invalid Mission, Delete it
		}

		lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
		
		// Call the Completion Function
		// This will give the player the reward or punishment.
		if( lua_pcall(L, 1, 0, 0) != 0)
		{
			LogMsg(ERR,"Failed to run %s.%s: %s\n", type.c_str(), completionFunctions[success], lua_tostring(L, -1));
			lua_settop(L,initialStackTop);
			return true; // Invalid Mission, Delete it
		}
		
		lua_settop(L,initialStackTop);
		return true; // Mission Complete
	}
	lua_settop(L,initialStackTop);
	return false;
}

void Mission::PushMissionTable()
{
	lua_State *L = Lua::CurrentState();
	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
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
