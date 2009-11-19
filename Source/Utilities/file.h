/*
 * Filename      : file.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Wednesday, November 18, 2009
 * Purpose       : Filesystem abstraction
 * Notes         : This is an interface to the physicsFS file
 */

#ifndef __H_FILE__
#define __H_FILE__

#include "includes.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <physfs.h>
#ifdef __cplusplus
}
#endif

class File {
	public:
		File( void );
		File( const std::string& filename );
		~File();
		bool Open( string filename );
		bool Read( int numBytes, unsigned char *buffer );
		long GetLength( void );
		bool Close();

	private:
		PHYSFS_file *fp;			/**<File pointer.  */
		long contentsSize;			/**<Number of bytes in the file. */
};

#endif // __H_XML__
