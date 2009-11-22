/**\file			file.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Low level interface for file access.
 * \details
 * Use filesystem for higher level access.*/

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
		File( const string& filename );
		~File();
		bool Open( const string& filename );
		bool Read( long numBytes, char *buffer );
		char *Read( void );
		long Tell( void );
		bool Seek( long pos );
		long GetLength( void );
		bool Close();

	private:
		PHYSFS_file *fp;		/** File pointer.  */
		long contentSize;		/** Number of bytes in the file. */
		PHYSFS_uint64 bufSize;	/** Buffer for read/write.*/
		char *fBuffer;			/** Pointer to any currently allocated buffer.*/
};

#endif // __H_XML__
