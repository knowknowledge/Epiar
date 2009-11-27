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
#include <physfs.h>

class File {
	public:
		File( void );
		File( const string& filename );
		~File();
		bool OpenRead( const string& filename );
		bool OpenWrite( const string& filename );
		bool Read( long numBytes, char *buffer );
		char *Read( void );
		bool Write( char *buffer, const long bufsize );
		long Tell( void );
		bool Seek( long pos );
		long GetLength( void );
		int SetBuffer( int bufSize );
		bool Close();

	private:
		PHYSFS_file *fp;		/** File pointer.  */
		long contentSize;		/** Number of bytes in the file. */
		string validName;		/** Name of the file referenced (exists).*/
};

#endif // __H_XML__
