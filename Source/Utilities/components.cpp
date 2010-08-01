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

/**\brief Find the first child node that matches a specific name
 * \param node The parent node that we are searching.
 * \param text The text name that we are searching for.
 * \sa NextSiblingNamed
 * \Example
 * \code
 * xmlNodePtr parent,node;
 * ... // get the parent node that contains the children we want to iterate over.
 * for(node = FirstChildNamed(parent,"FOO"); node != NULL; node = NextSiblingNamed(node, "FOO"))
 * {
 * ...
 * }
 * \endcode
 * \todo This applies to all XML files, not just component files.  It should be moved somewhere else.
 */

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

/**\brief Find the first sibliing node that matches a specific name
 * \param child The previous child node that we are searching.
 * \param text The text name that we are searching for.
 * \sa FirstChildNamed
 * \todo This applies to all XML files, not just component files.  It should be moved somewhere else.
 */
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

/**\brief Extract the text from a XML Node as a String.
 * \param doc The document that contains this node.
 * \param node The node itself
 * 
 * It's easier to deal with C++ strings than xmlStrings, since the C++ strings have destructors.
 *
 * \sa NodeToInt, NodeToFloat
 * \todo This applies to all XML files, not just component files.  It should be moved somewhere else.
 */
string NodeToString( xmlDocPtr doc, xmlNodePtr node )
{
	string value;
	xmlChar *xmlString;
	xmlString = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	value = (const char *)xmlString;
	xmlFree( xmlString );
	return value;
}

/**\brief Extract the text from a XML Node as an int.
 * \param doc The document that contains this node.
 * \param node The node itself
 * \sa NodeToString, NodeToFloat
 * \todo This applies to all XML files, not just component files.  It should be moved somewhere else.
 */
int NodeToInt( xmlDocPtr doc, xmlNodePtr node )
{
	int value;
	xmlChar *xmlString;
	xmlString = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	value = atoi( (const char *)xmlString );
	xmlFree( xmlString );
	return value;
}

/**\brief Extract the text from a XML Node as a float.
 * \param doc The document that contains this node.
 * \param node The node itself
 * \sa NodeToString, NodeToInt
 * \todo This applies to all XML files, not just component files.  It should be moved somewhere else.
 */
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
 *
 **\var name 
 * \brief The unique name of this Component.
 * \details The name is used to identify and fetch a Component.
 *          It may be sent to Lua as the sole identifier.
 *          It will likely be visible to a player.
 *
 **\fn SetName
 * \brief Change the Name of this Component.
 * \warn This may not be safe.
 *       It may not play well with the Components class list of names.
 *       If the name is set after the Component is added to the Components
 *       collection, this Component may become lost.
 * \todo Make this Private
 *
 **\fn GetName
 * \brief Get the name of this Component
 *
 **\fn FromXMLNode
 * \brief Parse an XML Node into a Component.
 *
 **\fn ToXMLNode
 * \brief Create an XML Node from this Component.
 */

/**\class Components
 * \brief A collection for all similar Component Instances
 *
 * \var rootName
 * \brief The name of the expected root node in an XML document for this kind of Component.
 *
 * \var componentName
 * \brief The name of the expected subnode that designates this kind of Component.
 *
 * \var components
 * \brief A hash table of Components indexed by the name of the Component.
 *
 * \var names
 * \brief A list of all of the names of the Components that are in the components hashtable.
 * \todo Is there a fast way to get this information out of the hash table rather than storing it explicitely?
 *
 * \fn newComponent
 * \brief A virtual constuctor for the Component Class being stored in this Components Instance.
 * \details This virtual function is used while parseing an XML file.
 *          We use this rather than "new Component()" because the actual
 *          Component class being stored is not going to be the same size as a
 *          pure Component.
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

/**\brief Attempt to parse an XML Node into a Component.
 * \details This will do some basic validation to check that the XML Node is
 * the right type.
 *
 * \todo Some Components instances should override this function to allow
 *       multiple Component types to be parsed depending on some
 *       characteristic.  For example, the planets xml might want to contain
 *       planets, stations, and gates.
 *
 */

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
