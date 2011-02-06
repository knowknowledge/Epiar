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
Mission::Mission( lua_State *_L, string _type, int _tableReference)
	:L(_L)
	,type(_type)
	,tableReference(_tableReference)
{
}

/**\brief Mission Destructor
 */
Mission::~Mission()
{
	luaL_unref(L, LUA_REGISTRYINDEX, tableReference);
}


bool Mission::ValidateMission( lua_State *L, string type, int tableReference, int expectedVersion ){
	int i;
	int currentVersion;
	
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

	const int initialStackTop = lua_gettop(L);

	// Check that this mission Type exists
	if( Mission::GetMissionType(L, type) != 1 ) {
		lua_settop(L, initialStackTop );
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

	// Check the Version Number
	if( expectedVersion > 0 )
	{
		lua_pushstring(L, "Version" );
		lua_gettable(L, initialStackTop + 1);

		// Check that the Version is actually a Number
		if( ! lua_isnumber(L, lua_gettop(L)) )
		{
			lua_pushvalue(L, lua_gettop(L));
			LogMsg(ERR, "This '%s' Mission has a corrupted version '%s'.", type.c_str(), lua_tostring(L, lua_gettop(L)) );
			lua_settop(L, initialStackTop );
			return false;
		}

		// Check that the version numbers match
		currentVersion = lua_tonumber(L, lua_gettop(L));
		if( expectedVersion != currentVersion )
		{
			lua_settop(L, initialStackTop );
			LogMsg(ERR, "This '%s' Mission is from version %d rather than the current version %d.", type.c_str(), currentVersion, expectedVersion);
			return false;
		}
	}

	// Discard the entire stack.
	lua_settop(L, initialStackTop );
	return true;
}

/**\brief 
 * \returns True on Error
 */
bool Mission::Reject()
{
	LogMsg(INFO, "Rejecting Mission '%s'", GetName().c_str());
	return (false == RunFunction( "Reject", true ) );
}

bool Mission::Accept()
{
	LogMsg(INFO, "Accepting Mission '%s'", GetName().c_str());
	return (false == RunFunction( "Accept", true ) );
}

/**\brief 
 * \returns True if the Mission is over (success, failure, or error) and should be deleted.
 */
bool Mission::Update()
{
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
	lua_rawgeti(L, LUA_REGISTRYINDEX, tableReference);
}


int Mission::GetVersion()
{
	int version;
	const int initialStackTop = lua_gettop(L);

	// Get the Mission Type Table
	if( Mission::GetMissionType(L, type) != 1 ) {
		LogMsg(ERR, "The Mission '%s' is missing.", type.c_str() );
		lua_settop(L, initialStackTop );
		return 0;
	}

	// Get the value of "Version" from the Mission Type Table
	lua_pushstring(L, "Version" );
	lua_gettable(L, initialStackTop + 1);

	// Check that the Version is actually a number.
	if( ! lua_isnumber(L, lua_gettop(L)) )
	{
		lua_pushvalue(L, lua_gettop(L));
		LogMsg(ERR, "This '%s' Mission has a corrupted version '%s'.", type.c_str(), lua_tostring(L, lua_gettop(L)) );
		lua_settop(L, initialStackTop );
		return 0;
	}

	version = lua_tonumber(L, lua_gettop(L));

	lua_settop(L, initialStackTop);
	return version;
}

/**\brief
 * \returns Success Boolean if the function was successfully called
 */
bool Mission::RunFunction(string functionName, bool clearStack)
{
	const int initialStackTop = lua_gettop(L);

	if( Mission::GetMissionType(L, type) != 1 ) {
		LogMsg(ERR, "Something bad happened?"); // TODO
		lua_settop(L, initialStackTop );
		return false;
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
	string type;
	int version = 0;;
	int missionTable;
	xmlNodePtr typeNode = FirstChildNamed(node,"type");
	xmlNodePtr versionNode = FirstChildNamed(node,"version");
	xmlNodePtr missionNode = FirstChildNamed(node,"value");

	type = NodeToString(doc, typeNode);
	if( versionNode != NULL ) {
		version = NodeToInt(doc, versionNode);
	}

	lua_State *L = Lua::CurrentState();

	// Turn the XML data into a Table
	Lua::ConvertFromXML(L, doc, missionNode);
	// pop the name off the top of the stack.  It should just be "missionTable" anyway.
	assert(lua_istable(L,lua_gettop(L)));
	// Gets and pops the top of the stack, which should have the the missionTable.
	missionTable = luaL_ref(L, LUA_REGISTRYINDEX); 

	assert(lua_isstring(L,lua_gettop(L)));
	lua_pop(L,1); // Pop the Name

	// Validate this Mission
	if( !Mission::ValidateMission(L, type, missionTable, version) ) {
		LogMsg(ERR, "Something important!");
		return NULL;
	}

	return new Mission(L, type, missionTable);
}

xmlNodePtr Mission::ToXMLNode()
{
	char buff[256];

	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST "Mission" );

	xmlNewChild(section, NULL, BAD_CAST "type", BAD_CAST type.c_str() );

	snprintf(buff, sizeof(buff), "%d", GetVersion() );
	xmlNewChild(section, NULL, BAD_CAST "version", BAD_CAST buff );

	lua_State *L = Lua::CurrentState();

	lua_pushstring(L,"missionTable");
	PushMissionTable();

	xmlAddChild(section, Lua::ConvertToXML(L, lua_gettop(L), lua_gettop(L)-1) );

	lua_pop(L, 1); // Pop Table

	return section;
}

int Mission::GetMissionType( lua_State *L, string type )
{
	lua_getglobal(L, type.c_str() );
	if( ! lua_istable(L, lua_gettop(L)) )
	{
		LogMsg(ERR, "There is no Mission Type named '%s'.", type.c_str() );
		return 0;
	}
	return 1;
}

