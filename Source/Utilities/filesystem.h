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
		static int AddPath( const string &archivename );
		static unsigned char *Filesystem::CopyFileToMem( const string &filename,int* bytesRead );
		static int DeInit( void );
};

#endif // __H_FILESYSTEM__
