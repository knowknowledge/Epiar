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
 * \brief XML handling.
 */

XMLFile::XMLFile() {
	xmlPtr = NULL;
}

XMLFile::XMLFile( const string& filename ) {
	xmlPtr = NULL;
	Open( filename );
}

bool XMLFile::New( const string& _filename, const string& rootName ) {
	char buff[256];
	assert( xmlPtr == NULL );
	filename = _filename;

	LogMsg(INFO, "New XML File: %s", filename.c_str());

	xmlPtr = xmlNewDoc( BAD_CAST "1.0" );

	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST rootName.c_str() );
	xmlDocSetRootElement(xmlPtr, root_node);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MAJOR);
	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MINOR);
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST buff);
	snprintf(buff, sizeof(buff), "%d", EPIAR_VERSION_MICRO);
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST buff);

	return true;
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

	xmlChar *xmlbuff;
	int buffersize;
	xmlDocDumpFormatMemory( xmlPtr, &xmlbuff, &buffersize, 1 );
	File saved = File( filename.c_str(), true );
	if(saved.Write( (char *)xmlbuff, buffersize ) != true) {
		LogMsg(ERR, "Could not save XML\n");
	}
	xmlFree(xmlbuff);

	//xmlSaveFormatFileEnc( filename.c_str(), xmlPtr, "ISO-8859-1", 1);
	return true;
}

bool XMLFile::Save( const string& filename ) {
	LogMsg(INFO, "Saving XML File '%s'.",filename.c_str() );

	xmlChar *xmlbuff;
	int buffersize;
	xmlDocDumpFormatMemory( xmlPtr, &xmlbuff, &buffersize, 1 );
	File saved = File( filename.c_str(), true );
	if(saved.Write( (char *)xmlbuff, buffersize ) != true) {
		LogMsg(ERR, "Could not save XML\n");
	}
	xmlFree(xmlbuff);

	//xmlSaveFormatFileEnc( filename.c_str(), xmlPtr, "ISO-8859-1", 1);
	return true;
}

bool XMLFile::Close() {
	if( xmlPtr ) xmlFreeDoc( xmlPtr );
	xmlPtr = NULL;

	return( true );
}

string XMLFile::Get( const string& path ) {
	xmlNodePtr cur;
	if( xmlPtr == NULL ) {
		return "";
	}

	// Look for the Node
	cur = FindNode( path );
	if( cur ) { // Found the path
		return NodeToString(xmlPtr,cur);;
	} else {
		// FIXME: when optionsfile is being created, this line causes a race condition
		//LogMsg(WARN, "Could not find XML path '%s'.", path.c_str() );
		return "";
	}
}

void XMLFile::Set( const string& path, const string& value ) {
	LogMsg(INFO,"Overriding Option['%s'] from '%s' to '%s'",path.c_str(),Get(path).c_str(),value.c_str());
	xmlNodePtr p =  FindNode(path,true);
	xmlNodeSetContent(p, BAD_CAST value.c_str() );
	LogMsg(INFO,"Done Overriding Option['%s'] to '%s'",path.c_str(),Get(path).c_str());
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
	xmlNodePtr p =  FindNode(path,true);
	xmlNodeSetContent(p, BAD_CAST stringvalue.c_str() );
	assert( stringvalue == Get(path));
}

vector<string> TokenizedString( const string& path, const string& tokens ) {
	string partialString;
	size_t pos, prevpos, len;
	vector<string> tokenized;

	// Tokenize the string
	prevpos = 0;
	do {
		pos = path.find_first_of(tokens, prevpos); // Get the next token position.
		len = (pos==string::npos)?pos:pos-prevpos;
		partialString = path.substr(prevpos, len); // Get the substring until the next token.
		tokenized.push_back( partialString ); // Record the substring
		if( pos!=string::npos ) {
			tokenized.push_back( path.substr(pos, 1) ); // Record the substring
		}
		prevpos = pos+1; // record where to start next time.
	} while ( pos != string::npos );
	
	return tokenized;
}

xmlNodePtr XMLFile::FindNode( const string& path, bool createIfMissing ) {
	xmlNodePtr cur,parent;
	string tokens = "/";
	vector<string> tokenized;
	vector<string>::iterator iter;
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

	if( xmlPtr == NULL ) {
		return( (xmlNodePtr)NULL );
	}

	// Initialize the xml navigation cursor
	cur = xmlDocGetRootElement( xmlPtr );
	if( cur == NULL ) {
		LogMsg(WARN, "XML file (%s) appears to be empty.", filename.c_str() );
		return( (xmlNodePtr)NULL );
	}
	
	tokenized = TokenizedString(path, tokens);
	iter = tokenized.begin();

	// The root is optional since it isn't a Child.
	if( !xmlStrcmp(cur->name, (const xmlChar *)(tokenized.front().c_str()) ) ) {
		++iter;
	}

	// Walk the tokenized path
	// If FirstChildNamed() doesn't find the path, it will return NULL
	for(; iter != tokenized.end() && cur != NULL; ++iter) {
		partialPath = *iter;
		if( partialPath.find_first_of(tokens) != string::npos) {
			continue;
		}
		parent = cur;
		cur = FirstChildNamed(parent, partialPath.c_str());
		if( (createIfMissing) && (cur==NULL) )
		{
			cur = xmlNewChild(parent, NULL, BAD_CAST partialPath.c_str(), NULL );
		}
	}

	// Memoize this result for later
	values[path] = cur;

	return( cur );
}

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
 */

xmlNodePtr FirstChildNamed( xmlNodePtr node, const char* text )
{
	assert( node != NULL );
	assert( text != NULL );
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
 */
xmlNodePtr NextSiblingNamed( xmlNodePtr child, const char* text )
{
	assert( child != NULL );
	assert( text != NULL );
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
 */
string NodeToString( xmlDocPtr doc, xmlNodePtr node )
{
	assert( doc != NULL );
	assert( node != NULL );
	string value;
	xmlChar *xmlString;
	xmlString = xmlNodeGetContent( node->xmlChildrenNode );
	if( xmlString ) {
		value = (const char *)xmlString;
	} else {
		value = "";
	}
	xmlFree( xmlString );
	return value;
}

/**\brief Extract the text from a XML Node as an int.
 * \param doc The document that contains this node.
 * \param node The node itself
 * \sa NodeToString, NodeToFloat
 */
int NodeToInt( xmlDocPtr doc, xmlNodePtr node )
{
	assert( doc != NULL );
	assert( node != NULL );
	int value;
	xmlChar *xmlString;
	xmlString = xmlNodeGetContent( node->xmlChildrenNode );
	if( xmlString ) {
		value = atoi( (const char *)xmlString );
	} else {
		value = 0;
	}
	xmlFree( xmlString );
	return value;
}

/**\brief Extract the text from a XML Node as a float.
 * \param doc The document that contains this node.
 * \param node The node itself
 * \sa NodeToString, NodeToInt
 */
float NodeToFloat( xmlDocPtr doc, xmlNodePtr node )
{
	assert( doc != NULL );
	assert( node != NULL );
	float value;
	xmlChar *xmlString;
	xmlString = xmlNodeGetContent( node->xmlChildrenNode );
	if( xmlString ) {
		value = atof( (const char *)xmlString );
	} else {
		value = 0.0f;
	}
	xmlFree( xmlString );
	return value;
}
