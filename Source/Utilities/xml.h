/*
 * Filename      : xml.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Monday, April 21, 2008
 * Purpose       : Interface for working with XML files
 * Notes         :
 */

#ifndef __H_XML__
#define __H_XML__

#include "Graphics/image.h"
#include "includes.h"
#include <map>
#include <zlib.h>

class XMLFile {
	public:
		XMLFile();
		XMLFile( string filename );
		~XMLFile();
		bool Open( string filename );
		bool Close();
		string Get( string path ); // cast/convert this to whatever return value you need
		
	private:
		xmlDocPtr xmlPtr;
		map<string,string> values;
		string filename;
};

#endif // __H_XML__
