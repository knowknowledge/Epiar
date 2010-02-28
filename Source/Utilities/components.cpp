/**\file			components.cpp
 * \author			Matt Zweig
 * \date			Created: Friday, February 26, 2010
 * \date			Modified: Friday, February 26, 2010
 * \brief
 * \details
 */

#include "Utilities/log.h"
#include "Utilities/file.h"
#include "Utilities/components.h"
#include "includes.h"

list<string>* Components::GetNames() {
	return new list<string>(names);
}



void Components::Add(Component* component) {
	string name = component->GetName();
	names.push_back( name );
	components[name] = component;
}

Component* Components::Get(string name) {
	map<string,Component*>::iterator val = components.find( name );
	if( val == components.end() ) return NULL;
	return val->second;
}

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
		Log::Error( "Could not load '%s' for parsing.", filename.c_str() );
		return false;
	}
	
	cur = xmlDocGetRootElement( doc );
	
	if( cur == NULL ) {
		Log::Error( "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return false;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)rootName.c_str() ) ) {
		Log::Error( "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return false;
	} else {
		Log::Message( "'%s' file found and valid, parsing...", filename.c_str() );
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
	
	Log::Message( "Parsing of file '%s' done, found %d objects. File is version %d.%d.%d.", filename.c_str(), numObjs, versionMajor, versionMinor, versionMacro );
	return true;
}

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
