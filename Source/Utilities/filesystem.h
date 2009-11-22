/*
 * Filename      : filesystem.h
 * Author(s)     : Maoserr
 * Date Created  : Wednesday, November 18, 2009
 * Last Modified : Wednesday, November 18, 2009
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

/** Filesystem class is an interface to deal with file system operations.
  * see the File class for dealing with specific files.
  */
class Filesystem {
	public:
		static int Init( void );
		static int AddPath( const string &archivename );
		static unsigned char *Filesystem::CopyFileToMem( const string &filename,int* bytesRead );
		static int DeInit( void );
};

#endif // __H_FILESYSTEM__
