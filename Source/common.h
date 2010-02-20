/**\file		common.h
 * \author		Chris Thielen (chris@luethy.net)
 * \author		and others.
 * \date		Created:	Saturday, January 5, 2008
 * \date		Modified:	Thursday, November 19, 2009
 * \brief		Common variables and defines.
 * \details
 *	This file contains some global variables, defines,
 *	.
 */

#ifndef __H_COMMON__
#define __H_COMMON__

#include "Graphics/font.h"
#include "Utilities/xml.h"

// FORPRODUCTION: switch this to 0
#define DEBUG 1

// defined in main.cpp. used through the codebase
extern XMLFile *optionsfile;
extern Font *SansSerif, *BitType, *Serif, *Mono;

// to simply accessing the options file
#define OPTION(T, path) (convertTo<T>( optionsfile->Get(path) ))
#define SETOPTION(path, value) (optionsfile->Set((path),(value)) )

struct create_delete_functor
{
	template <typename T>
	void operator()(T* _p)
	{
		delete _p;
	}
};

#endif // __H_COMMON__
