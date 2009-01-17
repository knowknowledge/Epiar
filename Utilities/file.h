/*
 * Filename      : file.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Monday, April 21, 2008
 * Purpose       : Filesystem abstraction
 * Notes         : This is needed to implement transparent loading from the FS or from the .tgz file, or other future areas
 *               : This class should be used like FILE pointers. Specifically, you shouldn't leave a File object in existence for
 *               : very long as the entire file is loaded into memory!
 */

#ifndef __H_FILE__
#define __H_FILE__

#include "includes.h"

class File {
	public:
		File();
		File( string filename );
		~File();
		bool Open( string filename );
		bool Close();
		void *Read( long *bytesRead, int len ); // reads 'len' bytes into a buffer. Callee must free the buffer!
		
	private:
		unsigned char *contents; // the entire contents of the file
		long contentsSize; // size of file read in & size of the contents buffer
};

#endif // __H_XML__
