/**\file			components.cpp
 * \author			Matt Zweig
 * \date			Created: Friday, February 26, 2010
 * \date			Modified: Friday, February 26, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/file.h"
#include "Utilities/components.h"

xmlNodePtr FirstChildNamed( xmlNodePtr node, const char* text )
{
	xmlNodePtr child = node->xmlChildrenNode;
	while( child != NULL )
	{
		if( !xmlStrcmp( child->name, (const xmlChar *)text ) )
		{
			return child;
		}
		child = child->next;
	}
	return (xmlNodePtr )NULL;
}

xmlNodePtr NextSiblingNamed( xmlNodePtr child, const char* text )
{
	child = child->next;
	while( child != NULL )
	{
		if( !xmlStrcmp( child->name, (const xmlChar *)text ) )
		{
			return child;
		}
		child = child->next;
	}
	
	return (xmlNodePtr )NULL;
}

string NodeToString( xmlDocPtr doc, xmlNodePtr node )
{
	string value;
	xmlChar *xmlString;
	xmlString = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	value = (const char *)xmlString;
	xmlFree( xmlString );
	return value;
}

int NodeToInt( xmlDocPtr doc, xmlNodePtr node )
{
	int value;
	xmlChar *xmlString;
	xmlString = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	value = atoi( (const char *)xmlString );
	xmlFree( xmlString );
	return value;
}

float NodeToFloat( xmlDocPtr doc, xmlNodePtr node )
{
	float value;
	xmlChar *xmlString;
	xmlString = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	value = atof( (const char *)xmlString );
	xmlFree( xmlString );
	return value;
}

/**\class Component
 * \brief A generic entity that is loaded and saved to XML
 */

/**\class Components
 * \brief A collection for all similar Component Instances
 */

/**\brief Get the Names of all Components in this collection
 */
list<string>* Components::GetNames() {
	return &names;
}


/**\brief Add a Component to this collection
 */
void Components::Add(Component* component) {
	string name = component->GetName();
	names.push_back( name );
	components[name] = component;
}

/**\brief Add a Component to this collection
 */
void Components::AddOrReplace(Component* component) {
	string name = component->GetName();
	map<string,Component*>::iterator val = components.find( name );
	if( val == components.end() ) { // new
		LogMsg(INFO,"Creating new Component '%s'",component->GetName().c_str());
		names.push_back( name );
		components[name] = component;
	} else { // old
		LogMsg(INFO,"Saving changes to Component '%s'",component->GetName().c_str());
		val->second = component;
	}
}

/**\brief Fetch a Component by its name
 * \return Component pointer or NULL
 */
Component* Components::Get(string name) {
	map<string,Component*>::iterator val = components.find( name );
	if( val == components.end() ) return NULL;
	return val->second;
}

bool Components::ParseXMLNode( xmlDocPtr doc, xmlNodePtr node )
{
	xmlNodePtr  attr;
	Component* component = newComponent();

	// All Compoents must have names!
	if( (attr = FirstChildNamed(node,"name")) ){
		component->SetName(NodeToString(doc,attr));
	} else {
		LogMsg(ERR,"Failed to find a name attribute for the %s node at line %d.\n", NodeToString(doc,attr).c_str(), xmlGetLineNo(node) );
		return false;
	}

	if( component->FromXMLNode(doc, node) ){
		Add( component );
		return true;
	}
	return false;
}

/**\brief Load an XML file
 * \arg filename The XML file that should be parsed.
 * \arg optional  If this is true, an error is not returned if the file doesn't exist.
 */
bool Components::Load(string filename, bool optional) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	int versionMajor = 0, versionMinor = 0, versionMacro = 0;
	int numObjs = 0;
	bool success = true;
	
	File xmlfile = File (filename);
	long filelen = xmlfile.GetLength();
	char *buffer = xmlfile.Read();
	doc = xmlParseMemory( buffer, static_cast<int>(filelen) );
	delete [] buffer;

	if( doc == NULL ) {
		LogMsg(ERR, "Could not load '%s' for parsing.", filename.c_str() );
		return optional;
	}
	
	cur = xmlDocGetRootElement( doc );
	
	if( cur == NULL ) {
		LogMsg(ERR, "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return false;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)rootName.c_str() ) ) {
		LogMsg(ERR, "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return false;
	} else {
		LogMsg(INFO, "'%s' file found and valid, parsing...", filename.c_str() );
	}
	
	cur = cur->xmlChildrenNode;
	while( success && cur != NULL ) {
		// Parse for the version information and any children nodes
		if( ( !xmlStrcmp( cur->name, BAD_CAST "version-major" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMajor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, BAD_CAST "version-minor" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMinor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, BAD_CAST "version-macro" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMacro = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, BAD_CAST componentName.c_str() ) ) ) {
			// Parse a Component
			success = ParseXMLNode( doc, cur );
			assert(success);
			if(success) numObjs++;
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc( doc );
	
	LogMsg(INFO, "Parsing of file '%s' done, found %d objects. File is version %d.%d.%d.", filename.c_str(), numObjs, versionMajor, versionMinor, versionMacro );
	return success;
}

/**\brief Save all Components to an XML file
 */
bool Components::Save(string filename) {
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST rootName.c_str() );
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( map<string,Component*>::iterator i = components.begin(); i != components.end(); ++i ) {
		section = i->second->ToXMLNode(componentName);
		xmlAddChild(root_node, section);
	}
	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
	return true;
}
