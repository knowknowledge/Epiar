/*
 * Filename      : file.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Monday, April 21, 2008
 * Purpose       : Low level interface for file access.
 * Notes         : Use filesystem for higher level access.
 */

#include "includes.h"
#include "Utilities/file.h"
#include "Utilities/log.h"

/** Constructor: does nothing
  */
File::File( void ){
}

/** Constructor: opens a file pointer to target
  * \param filename Name of the file to open
  */
File::File( const std::string& filename ) {
	Open( filename );
}

/** Opens a file pointer
  * \param filename Name of the file to open
  * \return true on success, false otherwise
  */
bool File::Open( const std::string& filename ) {
	PHYSFS_file *fp = PHYSFS_openRead( filename.c_str() );
	if( fp == NULL ){
		Log::Error("Could not open file: %s.\n%s", filename.c_str(),
			PHYSFS_getLastError());
		return( false );
	}
	contentSize = static_cast<long>( PHYSFS_fileLength( fp ) );
	return( true );
}

/** Reads a specified number of bytes from the file
  * This function requires you to provide your own buffer
  * as it will be more efficient.
  * \param numBytes Number of bytes to read
  * \param buffer Buffer to hold the data
  * \return true on success, false otherwise
  */
bool File::Read( long numBytes, unsigned char *buffer ){
	long bytesRead = static_cast<long>(
		PHYSFS_read( fp, buffer, contentSize, numBytes ));
	if( bytesRead == numBytes){
		return true;
	} else {
		Log::Error("Unable to read specified number of bytes. %s",
			PHYSFS_getLastError());
		return false;
	}
}

/** Returns the current position within the file
  * \return offset in bytes from start of file
  */
long File::Tell( void ){
	long offset;
	offset = static_cast<long>(
		PHYSFS_tell( fp ));
	if ( offset == -1 ){
		Log::Error("Error using file tell. %s",
			PHYSFS_getLastError());
	}
	return offset
}

/** Seek to a new position
  * \return true on success, false otherwise
  */
bool File::Seek( long pos ){
	int retval;
	retval = PHYSFS_seek( fp,
		static_cast<PHYSFS_uint64>( pos ));

/** Gets the length of the file in bytes
  * \return Length of the file in bytes
  */
long File::GetLength( void ){
	return contentSize;
}

/** Destructor: closes file and frees buffer
  */
File::~File() {
	Close();
}

/** Close function, closes file handle and frees buffer
  */
bool File::Close() {
	int retval = PHYSFS_close( fp );
	if ( retval == 0 ){
		Log::Error("Unable to close file handle.%s",
			PHYSFS_getLastError());
		return false;
	}
	contentsSize = 0;
	return true;
}

