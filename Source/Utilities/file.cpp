/**\file			file.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Low level interface for file access.
 * \details
 * Use filesystem for higher level access.*/

#include "includes.h"
#include "Utilities/file.h"
#include "Utilities/log.h"

/** \class File
 * Low level file access abstraction through PhysicsFS. */

/**Creates empty file instance.*/
File::File( void ):
contentSize(0), bufSize(1024*5) {
}

/**Creates file instance linked to filename. \sa Open.*/
File::File( const string& filename):
contentSize(0), bufSize(1024*5) {
	Open( filename );
}

/**Returns a handle to the filename.
 * \param filename The filename path.
 * \return true if successful, false otherwise.*/
bool File::Open( const string& filename ) {
	fp = PHYSFS_openRead( filename.c_str() );
	if( fp == NULL ){
		Log::Error("Could not open file: %s.\n%s", filename.c_str(),
			PHYSFS_getLastError());
		return( false );
	}
	if ( PHYSFS_setBuffer( fp, bufSize ) == 0 ){
		Log::Error("Could not create internal buffer for file: %s.\n%s",
				filename.c_str(),PHYSFS_getLastError());
		Close();
		return( false );
	}
	contentSize = static_cast<long>( PHYSFS_fileLength( fp ) );
	return( true );
}

/**Reads a specified number of bytes.
 * \param numBytes Number of bytes to read.
 * \param buffer Buffer to read bytes into.
 * \return true if successful, false otherwise.*/
bool File::Read( long numBytes, char *buffer ){
	long bytesRead = static_cast<long>(
		PHYSFS_read( fp, buffer, contentSize, numBytes ));
	if ( bytesRead == numBytes ){
		return true;
	} else {
		Log::Error("Unable to read specified number of bytes. %s",
			PHYSFS_getLastError());
		return false;
	}
}

/**Reads the whole file into a buffer. Buffer will be automatically allocated
 * for you, but you must explicitly free it by using "delete [] buffer"
 * \return Pointer to buffer, NULL otherwise.*/
char *File::Read( void ){
	char *fBuffer = new char[static_cast<PHYSFS_uint32>(contentSize)];
	long bytesRead = static_cast<long>(
		PHYSFS_read( fp, fBuffer, 1, static_cast<PHYSFS_uint32>(contentSize) ));
	if( bytesRead == contentSize ){
		return fBuffer;
	} else {
		delete [] fBuffer;
		Log::Error("Unable to read file into memory. %s",
			PHYSFS_getLastError());
		return NULL;
	}
}

/**Gets the current offset from the beginning of the file.
 * \return Offset in bytes from the beginning of the file.*/
long File::Tell( void ){
	long offset;
	offset = static_cast<long>(
		PHYSFS_tell( fp ));
	if ( offset == -1 ){
		Log::Error("Error using file tell. %s",
			PHYSFS_getLastError());
	}
	return offset;
}

/**Seek to new position in the file.
 * \param pos Position in bytes form the beginning of the file.
 * \return true if successful, false otherwise.*/
bool File::Seek( long pos ){
	int retval;
	retval = PHYSFS_seek( fp,
		static_cast<PHYSFS_uint64>( pos ));
	if ( retval == 0 ){
		Log::Error("Error using file seek. %s",PHYSFS_getLastError());
		return false;
	}
	return true;
}

/**Gets the total length in bytes of the file.
 * \return Total length of the file in bytes.*/
long File::GetLength( void ){
	return contentSize;
}

/**Destroys file instance. \sa Close.*/
File::~File() {
	Close();
}

/**Closes the file handle and frees associated buffer.
 * \return true if successful, false otherwise*/
bool File::Close() {
	int retval = PHYSFS_close( fp );
	if ( retval == 0 ){
		Log::Error("Unable to close file handle.%s",
			PHYSFS_getLastError());
		return false;
	}
	contentSize = 0;
	return true;
}
