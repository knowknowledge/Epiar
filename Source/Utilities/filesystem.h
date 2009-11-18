/*
 * Filename      : filesystem.h
 * Author(s)     : Maoserr
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Provides an abstraction to the file system
 * Notes         :
 */

#ifndef __H_FILESYSTEM__
#define __H_FILESYSTEM__

#include "includes.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <physfs.h>
#ifdef __cplusplus
}
#endif

class Filesystem {
	public:
		static int Init( void );
		static int AddArchive( const std::string &archivename );
		static unsigned char* Filesystem::CopyFileToMem( const std::string &filename,
			int* bytesRead );
		static int DeInit( void );
};

#endif // __H_FILESYSTEM__
