/**\file			vfl.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Tuesday, October 14, 2008
 * \date			Modified: Tuesday, October 14, 2008
 * \brief			Filesystem abstraction translator for multi-platform data file placement
 * \details
 */

#ifndef __H_VFL__
#define __H_VFL__

#include "includes.h"

class VFL {
	public:
		static void Initalize( void );

		static void realLog( int type, const char *func, char *message, ... );

		static void Close( void );

	protected:
		Log();
		Log( const Log & );
		Log& operator= (const Log&);

	private:
		static list<string> locations; // pointer to the log
};

#endif // __H_VFL__
