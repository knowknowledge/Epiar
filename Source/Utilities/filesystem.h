/**\file			filesystem.h
 * \author			Maoserr
 * \date			Created: Wednesday, November 18, 2009
 * \date			Modified: Wednesday, November 18, 2009
 * \brief			Provides an abstraction to the file system
 * \details
 */

#ifndef __H_FILESYSTEM__
#define __H_FILESYSTEM__

#include "includes.h"

#ifdef USE_PHYSICSFS
#include <physfs.h>
#else
#ifndef _WIN32
#include <dirent.h>
#endif
#endif

class Filesystem {
	public:
		static int Init( const char* argv0 );
		static int Init( const char* argv0, const string &extension );
		static int AppendPath( const string &archivename );
		static int PrependPath( const string &archivename );
		static list<string> Enumerate( const string &path, const string &suffix="");
		static void Version( void );
		static void OutputArchivers( void );
		static int Close( void );
	private:
		static list<string> paths;
};

#endif // __H_FILESYSTEM__

