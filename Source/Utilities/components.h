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
#include "common.h"
#include "Utilities/xml.h"

class Component {
	public:
		Component();
		string GetName() const { return name; }
		void SetName(string _name) { name = _name; }
		virtual bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) = 0;
		virtual xmlNodePtr ToXMLNode(string componentName) = 0;
	protected:
		string name;
	private:
};

class Components {
	public:
		void Add(Component* component);
		void AddOrReplace(Component* component);
		Component* Get(string name);
		list<string>* GetNames();
		int Size() { return (int)names.size(); }

		bool Load(string filename, bool fileoptional=false, bool skipcorrupt=false);
		bool Save();

		void SetFileName( const string& filename ) { this->filename = filename; }
		string GetFileName( ) { return filename; }
	protected:

		Components() {};  ///< Protected default constuctor
		Components( const Components & ); ///< Protected copy constuctor
		Components& operator= (const Components&); ///< Protected copy constuctor

		virtual Component* newComponent() = 0;
		bool ParseXMLNode( xmlDocPtr doc, xmlNodePtr node );
		string filename;
		string rootName;
		string componentName;
		map<string,Component*> components;
		list<string> names;
};

#endif // __h_components__
