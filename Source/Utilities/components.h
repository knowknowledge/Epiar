/**\file			components.cpp
 * \author			Matt Zweig
 * \date			Created: Friday, February 26, 2010
 * \date			Modified: Friday, February 26, 2010
 * \brief
 * \details
 */
#ifndef __h_components__
#define __h_components__

#include "includes.h"

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )

class Component {
	public:
		Component() {};
		string GetName() const { return name; }
		void SetName(string _name) { name = _name; }
		virtual bool parserCB( string sectionName, string subName, string value ) = 0;
		virtual xmlNodePtr ToXMLNode(string componentName) = 0;
	protected:
		string name;
	private:
};

class Components {
	public:
		void Add(Component* component);
		Component* Get(string name);
		list<string>* GetNames();

		bool Load(string filename);
		bool Save(string filename);
	protected:

		Components() {};
		Components( const Components & );
		Components& operator= (const Components&);

		virtual Component* newComponent() = 0;
		string rootName;
		string componentName;
		map<string,Component*> components;
		list<string> names;
};

#endif // __h_components__
