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

class Technology {
	public:
		bool parserCB( string sectionName, string subName, string value );
		void _dbg_PrintInfo( void );

		string GetName() const { return name; }
		list<Model*> GetModels() { return models; }
		list<Engine*> GetEngines() { return engines; }
		list<Weapon*> GetWeapons() { return weapons; }

	private:
		string name;
		list<Model*> models;
		list<Engine*> engines;
		list<Weapon*> weapons;
};

// Class that holds list of all technologies; manages them
class Technologies {
	public:
		static Technologies *Instance();
		bool Load( string& filename );
		Technology *GetTechnology( string& techname );

		list<string> *GetTechNames();

	protected:
		Technologies() {};
		Technologies( const Technologies & );
		Technologies& operator= (const Technologies&);

	private:
		static Technologies *pInstance;
		list<Technology*> technologies;
};

#endif // __h_technologies
