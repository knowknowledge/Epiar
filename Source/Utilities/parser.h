/*
 * Filename      : parser.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_parser__
#define __h_parser__

#include "includes.h"

template<class T>
class Parser {
	public:
		Parser() {};
		list<T *> Parse( string filename, string rootName, string unitName );
	
	private:
};

#include "parser.cxx"

#endif // __h_parser__
