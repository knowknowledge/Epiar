/*
 * Filename      : resource.h
 * Author(s)     : Matt Zweig
 * Date Created  : Saturday, December 19, 2009
 * Last Modified : Saturday, December 19, 2009
 * Purpose       : 
 * Notes         :
 */

#include <map>
#include "includes.h"

#ifndef __H_RESOURCE_CLASS
#define __H_RESOURCE_CLASS

class Resource{
	public:
		Resource(string path="");
		static void Store(Resource* res);
		static Resource* Get(string path);
		void SetPath(string path){ filepath = path; }
		string GetPath() { return filepath; }
	private:
		string filepath;
		static map<string,Resource*> values;
};

#endif // __H_RESOURCE__
