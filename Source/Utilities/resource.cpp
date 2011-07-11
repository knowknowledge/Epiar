/**\file			resource.cpp
 * \author			Matt Zweig
 * \date			Created: Saturday, December 19, 2009
 * \date			Modified: Saturday, December 19, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Utilities/resource.h"

/** \class Resource
 *  \brief Memory Management Superclass used to prevent duplications
 *  \details The Resource class provides a simple way to use Memory efficiently
 *  without having duplicate instances of the same object.  Resources are
 *  stored using a key (usually a path) and a pointer to the concrete object
 *  allocated on the heap.  This key is then stored into a master lookup table
 *  (an STL map) so that it can be retrieved later.  From then on, any attempt
 *  to access that object will not have to load the object.
 *
 *  All Resource subclasses should implement their own static "Get" function.
 *  This function should first attempt to retieve the Resource from the master
 *  Resource class.  If that fails, the subclass::Get should load the object
 *  and store it into the master Resource map.  This means that only the first
 *  attempt to Get a Resource requires the object to be loaded from the OS, all
 *  later Get attempts should be very fast.
 *
 *  Each key will only point to a single Resource object, but multiple names
 *  can point to the same Resource.  For example, a model image might be stored
 *  as both the relative path and the model's name.
 *
 *  \note Currently, Resources are never freed.  The assumption here is that
 *  all Resouces will be used again in the lifetime of the game.  This may
 *  change in later versions of Epiar.
 *
 *  \warning There is only one main resource lookup table.  If different
 *  Resource subclasses attempt to use the same key for different objects then
 *  errors will occur.
 *
 *  \see Image, Ani, Sound
 */

/** \brief The Master Resource Map.
 *  \warning This map is shared by all Resource subclasses.
 */
map<string, Resource*> Resource::values;

/** \brief Empty Resource constructor.
 */
Resource::Resource() {
}

/** \brief Store a Resource given a Key and pointer.
 *  \warning, attempting to store multiple resources using the same key will
 *  cause the previous object to be lost.
 *  \TODO Ensure that keys are not reused for different Resource objects.
 *  \TODO Fix potential memory leak.
 */
void Resource::Store(string key,Resource *res) {
	assert(key != ""); // No Empty Keys!
	values.insert(make_pair(key,res));
}

/** \brief Retrieve a stored Resource
 *  \returns The Resource pointer or NULL.
 */
Resource* Resource::Get(string path) {
	map<string,Resource*>::iterator val = values.find( path );
	if( val != values.end() ){
		return val->second;
	} 
	return NULL;
}
