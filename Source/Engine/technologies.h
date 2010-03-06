/**\file			technologies.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, February 13, 2010
 * \date			Modified: Saturday, February 13, 2010
 * \brief
 * \details
 */

#ifndef __h_technologies
#define __h_technologies

#include "includes.h"
#include "Engine/models.h"
#include "Engine/engines.h"
#include "Engine/weapons.h"
#include "Utilities/parser.h"
#include "Utilities/components.h"

class Technology : public Component {
	public:
		Technology();
  		Technology& operator= (const Technology&);
		Technology( string _name, list<Model*> _models, list<Engine*>_engines, list<Weapon*>_weapons);
		bool parserCB( string sectionName, string subName, string value );
		void _dbg_PrintInfo( void );
		xmlNodePtr ToXMLNode(string componentName);

		list<Model*> GetModels() { return models; }
		list<Engine*> GetEngines() { return engines; }
		list<Weapon*> GetWeapons() { return weapons; }

	private:
		list<Model*> models;
		list<Engine*> engines;
		list<Weapon*> weapons;
};

// Class that holds list of all technologies; manages them
class Technologies : public Components {
	public:
		static Technologies *Instance();
		Technology *GetTechnology( string& TechnologyName ) { return (Technology*) this->Get(TechnologyName); }
		Component* newComponent(){ return new Technology; }

	protected:
		Technologies() {};
		Technologies( const Technologies & );
		Technologies& operator= (const Technologies&);

	private:
		static Technologies *pInstance;
};

#endif // __h_technologies
