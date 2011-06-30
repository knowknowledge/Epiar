/**\file			file.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Low level interface for file access.
 * \details
 * Use filesystem for higher level access.*/

#ifndef __H_FILE__
#define __H_FILE__

#include "includes.h"

#ifdef USE_PHYSICSFS
#include <physfs.h>
#else
#include <sys/stat.h>
#endif

class File {
	public:
		File( void );
		File( const string& filename, bool writable = false );
		~File();
		bool Read( long numBytes, char *buffer );
		long GetLength( void );
		bool Close();

		bool OpenRead( const string& filename );
		bool OpenWrite( const string& filename );
		char *Read( void );
		bool Write( char *buffer, const long bufsize );
		long Tell( void );
		bool Seek( long pos );
		int SetBuffer( int bufSize );

		static bool Exists( const string& filename );
		static bool IsDir( const string& filename );

		string GetRelativePath();
		string GetAbsolutePath();

	private:
#ifdef USE_PHYSICSFS
		PHYSFS_file *fp;		/** File pointer.  */
#else
		FILE *fp;
#endif
        static string LastErrorMessage( void );

		long contentSize;		/** Number of bytes in the file. */
		string validName;		/** Name of the file referenced (exists).*/
};

bool IsBigEndian();

#endif // __H_XML__
