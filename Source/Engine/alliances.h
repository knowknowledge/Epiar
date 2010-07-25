/**\file			alliances.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_alliance__
#define __h_alliance__

#include "includes.h"
#include "Utilities/components.h"

// Abstraction of a single planet
class Alliance : public Component {
	public:
		Alliance();
  		Alliance& operator= (const Alliance&);
		Alliance( string _name, short int _attackSize, float _aggressiveness, string _currency);
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		short int GetAttackSize(void){ return attackSize; }
		float GetAggressiveness(void){ return aggressiveness; }
		string GetCurrency(void){ return currency; }
		
	private:
		short int attackSize;
		float aggressiveness;
		string currency;
};

// Class that holds list of all planets; manages them
class Alliances : public Components {
	public:
		static Alliances *Instance();
		Alliance* GetAlliance(string name) { return (Alliance*) this->Get(name); }
		Component* newComponent() { return new Alliance(); }

	protected:
		Alliances() {};
		Alliances( const Alliances & );
		Alliances& operator= (const Alliances&);

	private:
		static Alliances *pInstance;
};

#endif // __h_alliances__
