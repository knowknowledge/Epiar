/**\file			debug.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, Ocotober 4, 2008
 * \date			Modified: Saturday, October 4, 2008
 * \brief
 * \details
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

