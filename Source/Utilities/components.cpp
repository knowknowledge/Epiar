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

/**\brief Load an XML file
 */
bool Components::Load(string filename) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	int versionMajor = 0, versionMinor = 0, versionMacro = 0;
	Component *obj = NULL;
	int numObjs = 0;
	
	File xmlfile = File (filename);
	long filelen = xmlfile.GetLength();
	char *buffer = xmlfile.Read();
	doc = xmlParseMemory( buffer, static_cast<int>(filelen) );
	delete [] buffer;

	if( doc == NULL ) {
		LogMsg(ERROR, "Could not load '%s' for parsing.", filename.c_str() );
		return false;
	}
	
	cur = xmlDocGetRootElement( doc );
	
	if( cur == NULL ) {
		LogMsg(ERROR, "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return false;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)rootName.c_str() ) ) {
		LogMsg(ERROR, "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return false;
	} else {
		LogMsg(INFO, "'%s' file found and valid, parsing...", filename.c_str() );
	}
	
	cur = cur->xmlChildrenNode;
	while( cur != NULL ) {
		// Parse for the version information and any children nodes
		if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-major" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMajor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-minor" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMinor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-macro" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMacro = atoi( (char *)key );
			xmlFree( key );
		} else {
			// Any node here is a child of <rootNode> and therefore is a section, i.e. <ui>
			char *sectionName = (char *)cur->name;
			xmlNodePtr subCur = cur->xmlChildrenNode;

			if( strcmp( sectionName, "text" ) ) {
				assert( obj == NULL );
				obj = newComponent();
				
				// Parse the children of a section
				while( subCur != NULL ) {
					char *subName = (char *)subCur->name;
					
					// Every opening/closing parent tag for libxml2 is called 'text', but we don't care about those
					if( strcmp( subName, "text" ) ) {
						// Extract the key's value
						xmlChar *subKey = xmlNodeListGetString( doc, subCur->xmlChildrenNode, 1 );
						
						// On cases (comments?) subKey will be NULL
						if( subKey )					
						{
							obj->parserCB( sectionName, subName, (const char*) subKey );
							//cout<<"["<<sectionName<<"] ["<<subName<<"] = "<<((const char*) subKey)<<endl;
						}
						
						xmlFree( subKey );
					}
					
					subCur = subCur->next;
				}
				
				Add(obj);
				obj = NULL;
				numObjs++;
			}
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc( doc );
	
	LogMsg(INFO, "Parsing of file '%s' done, found %d objects. File is version %d.%d.%d.", filename.c_str(), numObjs, versionMajor, versionMinor, versionMacro );
	return true;
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
