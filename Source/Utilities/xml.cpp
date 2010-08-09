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

	// Look for the Node
	cur = FindNode( path );
	if( cur ) { // Found the path
		return NodeToString(xmlPtr,cur);;
	} else {
		return "";
	}
}

void XMLFile::Set( const string& path, const string& value ) {
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),value.c_str());
	xmlNodeSetContent(FindNode(path,true), BAD_CAST value.c_str() );
	assert( value == Get(path));
}

void XMLFile::Set( const string& path, const float value ) {
	// Convert the float to a string before saving it.
	string stringvalue;
	stringstream val_ss;
	val_ss << value;
	val_ss >> stringvalue;
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),stringvalue.c_str());
	xmlNodeSetContent(FindNode(path,true), BAD_CAST stringvalue.c_str() );
	assert( stringvalue == Get(path));
}

void XMLFile::Set( const string& path, const int value ) {
	// Convert the int to a string before saving it.
	string stringvalue;
	stringstream val_ss;
	val_ss << value;
	val_ss >> stringvalue;
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),stringvalue.c_str());
	xmlNodeSetContent(FindNode(path,true), BAD_CAST stringvalue.c_str() );
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

xmlNodePtr XMLFile::FindNode( const string& path, bool createIfMissing ) {
	xmlNodePtr cur,parent;
	queue<string> tokenized;
	string partialPath;

	// Check previously memoized values
	map<string,xmlNodePtr>::iterator val = values.find( path );
	if( val != values.end() ){ // If we found it
		// Check that we don't return memoized NULL values when instructed to createIfMissing.
		if( !((val->second==NULL) && createIfMissing) )
		{
			return val->second;
		}
	}

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
		parent = cur;
		cur = FirstChildNamed(parent, partialPath.c_str());
		if( (createIfMissing) && (cur==NULL) )
		{
			cur = xmlNewChild(parent, NULL, BAD_CAST partialPath.c_str(), BAD_CAST "" );
		}
	}

	// Memoize this result for later
	values.insert(make_pair(path,cur));

	return( cur );
}
