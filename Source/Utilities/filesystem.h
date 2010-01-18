/**\file			filesystem.h
 * \author			Maoserr
 * \date			Created: Wednesday, November 18, 2009
 * \date			Modified: Wednesday, November 18, 2009
 * \brief			Provides an abstraction to the file system
 * \details
 */

#ifndef __H_FILESYSTEM__
#define __H_FILESYSTEM__

#ifdef USE_PHYSICSFS
#include "includes.h"
#include <physfs.h>

class Filesystem {
	public:
		static int Init( const char* argv0 );
		static int Init( const char* argv0, const string &extension );
		static int AppendPath( const string &archivename );
		static int PrependPath( const string &archivename );
		static int Enumerate( const string &path );
		static void Version( void );
		static void OutputArchivers( void );
		static int DeInit( void );
};

#endif

#endif // __H_FILESYSTEM__