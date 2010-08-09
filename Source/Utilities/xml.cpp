/**\file		xml.cpp
 * \author		Chris Thielen (chris@epiar.net)
 * \date		Created: Monday, April 21, 2008
 * \date		Modified: Saturday, November 21, 2009
 * \brief       Interface with XML files
 * \details
 *
 */

#include "includes.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"
#include "Utilities/components.h"

/**\class XMLFile
 * \brief XML handling. */

XMLFile::XMLFile() {
	xmlPtr = NULL;
}

XMLFile::XMLFile( const string& filename ) {
	xmlPtr = NULL;
	Open( filename );
}

bool XMLFile::Open( const string& filename ) {
	char *buf = NULL;
	long bufSize = 0;
	File xmlfile;

	if( xmlfile.OpenRead( filename ) == false ) {
		LogMsg(ERR, "Could not find file %s", filename.c_str() );
		return( false );
	}

	buf = xmlfile.Read();
	bufSize = xmlfile.GetLength();
	if( buf == NULL ) {
		LogMsg(ERR, "Could not load XML from archive. Buffer failed to allocate." );
		return( NULL );
	}

	xmlPtr = xmlParseMemory( buf, bufSize );
	delete [] buf;

	this->filename.assign( filename );

	return( true );
}

XMLFile::~XMLFile() {
	Close();
}

bool XMLFile::Save() {
	LogMsg(INFO, "Saving XML File '%s'.",filename.c_str() );
	xmlSaveFormatFileEnc( filename.c_str(), xmlPtr, "ISO-8859-1", 1);
	return true;
}

bool XMLFile::Close() {
	if( xmlPtr ) xmlFreeDoc( xmlPtr );
	xmlPtr = NULL;

	return( true );
}

string XMLFile::Get( const string& path ) {
	xmlNodePtr cur;

	map<string,string>::iterator val = values.find( path );
	if( val != values.end() ){
		//LogMsg(INFO,"Found that key '%s' of XML file '%s' is '%s'",path.c_str(),filename.c_str(),(val->second).c_str());
		return val->second;
	}

	// Look for the Node
	cur = FindNode( path );
	if( cur ) { // Found the path
		xmlChar *subKey = xmlNodeListGetString( xmlPtr, cur->xmlChildrenNode, 1 );
		string result = string( (char *)subKey );
		values.insert(make_pair(path,result));
		LogMsg(INFO,"Populating key '%s' of XML file '%s' as '%s'",path.c_str(),filename.c_str(),result.c_str());
		xmlFree(subKey);
		return result;
	} else { // Did not find the path
		values.insert(make_pair(path,"")); // Insert dummy value so that we don't need to search for it again
	}

	LogMsg(WARN,"Attempted to Get non-existant value '%s' from XML file '%s'",path.c_str(),filename.c_str());
	values.insert(make_pair(path,"")); // Insert dummy value so that we don't need to search for it again

	// didn't find it
	return( "" );
}

void XMLFile::Set( const string& path, const string& value ) {
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),value.c_str());
	InsertNode(path,value);
	values[path] = value;
	assert( value == Get(path));
}

void XMLFile::Set( const string& path, const float value ) {
	// Convert the float to a string before saving it.
	string stringvalue;
	stringstream val_ss;
	val_ss << value;
	val_ss >> stringvalue;
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),stringvalue.c_str());
	InsertNode(path,stringvalue);
	values[path] = stringvalue;
	assert( stringvalue == Get(path));
}

void XMLFile::Set( const string& path, const int value ) {
	// Convert the int to a string before saving it.
	string stringvalue;
	stringstream val_ss;
	val_ss << value;
	val_ss >> stringvalue;
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),stringvalue.c_str());
	InsertNode(path,stringvalue);
	values[path] = stringvalue;
	assert( stringvalue == Get(path));
}

queue<string> XMLFile::TokenizedPath( const string& path ) {
	string partialPath;
	size_t pos, prevpos, len;
	const char* tokens = "/";
	queue<string> tokenized;

	// Tokenize the path
	prevpos = 0;
	do {
		pos = path.find_first_of(tokens, prevpos); // Get the next token position.
		len = (pos==string::npos)?pos:pos-prevpos;
		partialPath = path.substr(prevpos, len); // Get the substring until the next token.
		tokenized.push( partialPath ); // Record the substring
		//printf("Path: '%s'\n", partialPath.c_str() );
		prevpos = pos+1; // record where to start next time.
	} while ( pos != string::npos );
	
	return tokenized;
}

xmlNodePtr XMLFile::FindNode( const string& path ) {
	xmlNodePtr cur;
	queue<string> tokenized;
	string partialPath;

	// Initialize the xml navigation cursor
	cur = xmlDocGetRootElement( xmlPtr );
	if( cur == NULL ) {
		LogMsg(WARN, "XML file (%s) appears to be empty.",filename.c_str() );
		return( (xmlNodePtr)NULL );
	}
	
	tokenized = TokenizedPath(path);

	// The root is optional since it isn't a Child.
	if( !xmlStrcmp(cur->name, (const xmlChar *)(tokenized.front().c_str()) ) ) {
		tokenized.pop();
	}

	// Walk the tokenized path
	// If FirstChildNamed() doesn't find the path, it will return NULL
	while( !tokenized.empty() && cur != NULL) {
		partialPath = tokenized.front();
		//printf("XML: '%s' @ '%s'\n", partialPath.c_str(), cur->name);
		tokenized.pop();
		cur = FirstChildNamed(cur, partialPath.c_str());
	}

	return( cur );
}

void XMLFile::InsertNode( const string& path, const string& value ) {
	xmlNodePtr cur = NULL;
	xmlNodePtr parent = NULL;
	queue<string> tokenized;
	string partialPath;

	// Initialize the xml navigation cursor
	parent = xmlDocGetRootElement( xmlPtr );
	if( parent == NULL ) {
		LogMsg(WARN, "XML file (%s) appears to be empty.",filename.c_str() );
		return;
	}
	
	tokenized = TokenizedPath(path);

	// skip root is optional since it isn't a Child.
	if( !xmlStrcmp(parent->name, (const xmlChar *)(tokenized.front().c_str()) ) ) {
		tokenized.pop();
	}

	// Walk the tokenized path
	// If FirstChildNamed() doesn't find the path, we create it.
	while( !tokenized.empty() ) {
		partialPath = tokenized.front();
		tokenized.pop();
		cur = FirstChildNamed(parent, partialPath.c_str());
		if( cur == NULL ) {
			LogMsg(INFO,"Creating node '%s'.",partialPath.c_str() );
			cur = xmlNewChild(parent, NULL, BAD_CAST partialPath.c_str(), BAD_CAST "" );
		}
		parent = cur;
	}

	// Save the value
	assert(cur);
	xmlNodeSetContent(cur, BAD_CAST value.c_str() );
	LogMsg(INFO,"Set node at '%s' to '%s'",path.c_str(),value.c_str());
}
