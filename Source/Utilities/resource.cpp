/**\file			resource.cpp
 * \author			Matt Zweig
 * \date			Created: Saturday, December 19, 2009
 * \date			Modified: Saturday, December 19, 2009
 * \brief
 * \details
 */

#include <map>
#include "includes.h"
#include "Utilities/resource.h"

map<string, Resource*> Resource::values;

Resource::Resource() {
}

void Resource::Store(string key,Resource *res) {
	values.insert(make_pair(key,res));
}

Resource* Resource::Get(string path) {
	map<string,Resource*>::iterator val = values.find( path );
	if( val != values.end() ){
		return val->second;
	} 
	return NULL;
}
