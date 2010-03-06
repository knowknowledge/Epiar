/*
 * Filename      : resource.h
 * Author(s)     : Matt Zweig
 * Date Created  : Saturday, December 19, 2009
 * Last Modified : Saturday, December 19, 2009
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"

#ifndef __H_RESOURCE_CLASS
#define __H_RESOURCE_CLASS

class Resource{
	public:
		Resource();
		static void Store(string key, Resource* res);
		static Resource* Get(string path);
	private:
		static map<string,Resource*> values;
};

#endif // __H_RESOURCE__
