/**\file			parser.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_parser__
#define __h_parser__

#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/parser.h"
#include "Utilities/file.h"

template<class T>
class Parser {
	public:
		Parser() {};
		list<T *> Parse( string filename, string rootName, string unitName );

	private:
};

template< class T >
list<T *> Parser<T>::Parse( string filename, string rootName, string unitName ) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	int versionMajor = 0, versionMinor = 0, versionMacro = 0;
	list<T *> parsedObjs;
	T *obj = NULL;
	int numObjs = 0;
	
	File xmlfile = File( filename.c_str() );
	long filelen = xmlfile.GetLength();
	char *buffer = xmlfile.Read();
	doc = xmlParseMemory( buffer, static_cast<int>(filelen) );
	delete [] buffer;

	if( doc == NULL ) {
		Log::Error( "Could not load '%s' for parsing.", filename.c_str() );
		return parsedObjs;
	}
	
	cur = xmlDocGetRootElement( doc );
	
	if( cur == NULL ) {
		Log::Error( "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return parsedObjs;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)rootName.c_str() ) ) {
		Log::Error( "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return parsedObjs;
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
				obj = new T;
				
				// Parse the children of a section
				while( subCur != NULL ) {
					char *subName = (char *)subCur->name;
					
					// Every opening/closing parent tag for libxml2 is called 'text', but we don't care about those
					if( strcmp( subName, "text" ) ) {
						// Extract the key's value
						xmlChar *subKey = xmlNodeListGetString( doc, subCur->xmlChildrenNode, 1 );
						
						// On cases (comments?) subKey will be NULL
						if( subKey )						
							obj->parserCB( sectionName, subName, (const char*) subKey );
						
						xmlFree( subKey );
					}
					
					subCur = subCur->next;
				}
				
				parsedObjs.push_back( obj );
				obj = NULL;
				numObjs++;
			}
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc( doc );
	
	Log::Message( "Parsing of file '%s' done, found %d objects. File is version %d.%d.%d.", filename.c_str(), numObjs, versionMajor, versionMinor, versionMacro );
	
	return parsedObjs;
}


#endif // __h_parser__
