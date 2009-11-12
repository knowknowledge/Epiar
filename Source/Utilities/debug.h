/*
 * Filename      : debug.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, October 3, 2008
 * Last Modified : Friday, October 3, 2008
 * Purpose       : Code debugging
 * Notes         : 
 */

#ifndef __H_DEBUG__
#define __H_DEBUG__

#include "includes.h"

class Debug {
	public:
		static void Set();
		static void Unset();
		static void Print(const char *message, ...);
		
		static bool active;
};

#endif // __H_DEBUG__

