/**\file			commodities.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Wednesday, April 21, 2010
 * \date			Modified:
 * \brief
 * \details
 */

#ifndef __h_commodities__
#define __h_commodities__

#include "includes.h"
#include "Utilities/components.h"

class Commodity : public Component {
	public:
		Commodity();
		Commodity& operator =(const Commodity&);
		Commodity(string _name, int _msrp);
		~Commodity(void);

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		void _dbg_PrintInfo( void );

		int GetMSRP(void) {return msrp;}
	private:
		int msrp;
};

class Commodities : public Components{
	public:
		static Commodities *Instance();
		Commodity* GetCommodity(string name) { return (Commodity*) this->Get(name); }
		Component* newComponent() { return new Commodity(); }

	protected:
		Commodities() {};
		Commodities( const Commodities & );
		Commodities& operator= (const Commodities&);

	private:
		static Commodities *pInstance;
};

#endif // __h_commodities__
