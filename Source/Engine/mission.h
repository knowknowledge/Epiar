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
		Mission(string _type, int _tableReference);
		~Mission();

		static bool ValidateMission(  string type, int tableReference );

		bool Accept();
		bool Update();

		string GetName() { return GetStringAttribute("Name"); }
		string GetDescription() { return GetStringAttribute("Description"); }

		// TODO: Write these functions!
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		
	private:
		string type; ///< The Mission Type
		int tableReference; ///< A Lua table to hold

		string GetStringAttribute(string attribute);
};

#endif //__H_MISSION__