/**\file		xml.h
 * \author		Chris Thielen (chris@epiar.net)
 * \date		Created: Monday, April 21, 2008
 * \date		Modified: Saturday, November 21, 2009
 * \brief       Interface with XML files
 * \details
 *
 */


#ifndef __H_XML__
#define __H_XML__

#include "includes.h"
#include <zlib.h>

class XMLFile {
	public:
		XMLFile();
		XMLFile( const string& filename );
		~XMLFile();
		bool New( const string& filename, const string& rootName );
		bool Open( const string& filename );
		bool Save( void );
		bool Save( const string& filename );
		bool Close();

		void SetFileName( const string& _filename ) { filename = _filename; }
		string GetFileName( ) { return filename; }

		string Get( const string& path ); // cast/convert this to whatever return value you need
		void Set( const string& path, const string& value ); // cast/convert this to whatever return value you need
		void Set( const string& path, const float value ); // cast/convert this to whatever return value you need
		void Set( const string& path, const int value ); // cast/convert this to whatever return value you need

	protected:
		string filename;

	private:
		xmlDocPtr xmlPtr;
		map<string,xmlNodePtr> values;

		xmlNodePtr FindNode( const string& path, bool createIfMissing=false );
};

vector<string> TokenizedString(const string& path, const string& tokens);

#define PPA_MATCHES( text ) if( !strcmp( subName.c_str(), text ) )
#define NodeNameIs( node, text ) ( !xmlStrcmp( ((node)->name), (const xmlChar *)(text) ) )

xmlNodePtr FirstChildNamed( xmlNodePtr node, const char* text );
xmlNodePtr NextSiblingNamed( xmlNodePtr child, const char* text );
string NodeToString( xmlDocPtr doc, xmlNodePtr node );
int NodeToInt( xmlDocPtr doc, xmlNodePtr node );
float NodeToFloat( xmlDocPtr doc, xmlNodePtr node );

#endif // __H_XML__
