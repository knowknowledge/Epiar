/**\file		xml.cpp
 * \author		Chris Thielen (chris@luethy.net)
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

	if( xmlfile.Open( filename ) == false ) {
		Log::Error( "Could not find file %s", filename.c_str() );
		return( false );
	}

	buf = xmlfile.Read();
	bufSize = xmlfile.GetLength();
	if( buf == NULL ) {
		Log::Error( "Could not load XML from archive. Buffer failed to allocate." );
		return( NULL );
	}

	xmlPtr = xmlParseMemory( buf, bufSize );

	this->filename.assign( filename );

	return( true );
}

XMLFile::~XMLFile() {
	Close();
}

bool XMLFile::Close() {
	if( xmlPtr ) xmlFreeDoc( xmlPtr );
	xmlPtr = NULL;

	return( true );
}

string XMLFile::Get( const string& path ) {
	xmlNodePtr cur;

	// take apart the path and put it in a queue
	queue<string> pathTree;
	string nodeBuffer;
	for( unsigned int i = 0; i < path.length(); i++ ) {
		if(path[i] != '/') {
			// normal character. we record it to the nodeBuffer so we know the full node name when a '/' is found
			nodeBuffer += path[i];
		} else {
			// we are leaving the parent node, so push it to the queue
			pathTree.push( nodeBuffer );
			nodeBuffer = "";
		}
	}
	if( nodeBuffer.length() ) pathTree.push( nodeBuffer ); // our last node won't end in a '/', so we can't forget it here

	// initialize the xml navigation cursor
	cur = xmlDocGetRootElement( xmlPtr );

	if( cur == NULL ) {
		Log::Warning( "XML file () appears to be empty." );
		return( string() );
	}

	// which node are we looking for?
	string nodeToLocate = pathTree.front();

	while( !pathTree.empty() ) {
		if( cur == NULL ) {
			break; // at the end of the tree and didn't find anything
		}

		if( !xmlStrcmp( cur->name, (const xmlChar *)nodeToLocate.c_str() ) ) {
			// found it

			// is there any else in the path? if not, we retrieve the value
			// else, we move on into the child tree
			if( nodeToLocate == pathTree.back() ) {
				// nothing left to look for, return this value
				xmlChar *subKey = xmlNodeListGetString( xmlPtr, cur->xmlChildrenNode, 1 );
				return( string( (char *)subKey ) );
			} else {
				// more path, explore the child tree
				cur = cur->xmlChildrenNode;
				pathTree.pop(); // remove the element we just found off the tree and get the next
				nodeToLocate = pathTree.front();
			}
		}

		cur = cur->next;
	}

	// didn't find it
	return( string() );
}
