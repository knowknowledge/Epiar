/*
 * Filename      : common.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Provide access to common areas of the tree
 * Notes         :
 */

#ifndef __H_COMMON__
#define __H_COMMON__

#include "Graphics/font.h"
#include "Utilities/archive.h"
#include "Utilities/xml.h"

// FORPRODUCTION: switch this to 0
#define DEBUG 1

// defined in main.cpp. used through the codebase
extern Archive *epiardata;
extern XMLFile *optionsfile;
extern Font *Vera8, *Vera10, *Visitor10, *VeraMono10;

// to simply accessing the options file
#define OPTION(T, path) (convertTo<T>( optionsfile->Get(path) ))

struct create_delete_functor
{
	template <typename T>
	void operator()(T* _p)
	{
		delete _p;
	}
};

#endif // __H_COMMON__
