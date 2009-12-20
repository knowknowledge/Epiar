/*
 * Filename      : resource.cpp
 * Author(s)     : Matt Zweig
 * Date Created  : Saturday, December 19, 2009
 * Last Modified : Saturday, December 19, 2009
 * Purpose       : 
 * Notes         :
 */

#include <map>
#include "includes.h"
#include "Utilities/resource.h"

map<string, Resource*> Resource::values;

Resource::Resource(string path) {
	filepath = path;
}

void Resource::Store(Resource *res) {
	values.insert(make_pair(res->GetPath(),res));
}

Resource* Resource::Get(string path) {
	map<string,Resource*>::iterator val = values.find( path );
	if( val != values.end() ){
		return val->second;
	} 
	return NULL;
}
