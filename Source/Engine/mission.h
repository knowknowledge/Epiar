/**\file			mission.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Tuesday, August 24, 2010
 * \date			Modified: Tuesday, August 24, 2010
 * \brief			
 * \details
 */

#ifndef __H_MISSION__
#define __H_MISSION__

#include "includes.h"
#include "common.h"

class Mission{
	public:
		Mission( lua_State *L, string _type, int _tableReference);
		~Mission();

		static bool ValidateMission( lua_State *L, string type, int tableReference, int expectedVersion );

		bool Accept();
		bool Reject();
		bool Update();
		bool Land();

		int GetVersion();
		string GetName() { return GetStringAttribute("Name"); }
		string GetDescription() { return GetStringAttribute("Description"); }

		void PushMissionTable();

		static Mission* FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode();
		
	private:
		lua_State *L; ///< Lua Pointer
		string type; ///< The Mission Type
		int tableReference; ///< A Lua table to hold

		bool RunFunction(string functionName, bool checkCompletion);
		string GetStringAttribute(string attribute);
		static int GetMissionType( lua_State *L, string type );
};

#endif //__H_MISSION__
