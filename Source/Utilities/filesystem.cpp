/*
 * Filename      : filesystem.cpp
 * Author(s)     : Maoserr
 * Date Created  : Wednesday, November 18, 2009
 * Last Modified : Wednesday, November 18, 2009
 * Purpose       : Provides an abstraction to the file system
 * Notes         :
 */

#include "includes.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"


/**
 * Initialize the PhysFS system
 * \return Nonzero on success
 */

int Filesystem::Init() {
	int retval;
	if ( (retval = PHYSFS_init(NULL)) == 0 )
		Log::Error("Error initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	return retval;
}

/**
  * Adds an archive to be searched for files
  * \param archivename The path to the archive
  * \return Nonzero on success
  */
int Filesystem::AddArchive( const std::string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_addToSearchPath(archivename.c_str(), 1)) == 0 )
		Log::Error("Error adding search path.\n%s",PHYSFS_getLastError());
	return retval;
}

unsigned char *Filesystem::CopyFileToMem( const std::string &filename,
		int *bytesRead ){
	const char *cName;

	cName = filename.c_str();
	// Check for file existence
	if ( !PHYSFS_exists( cName ) ){
		Log::Error("File does not exist: %s.", cName);
		return NULL;
	}

	PHYSFS_file *hImg;
	// Try to open it
	if ( (hImg = PHYSFS_openRead( cName )) == NULL ){
		Log::Error("Could not open image file: %s.\n%s", cName,
			PHYSFS_getLastError());
		return NULL;
	}

	PHYSFS_sint64 fileSize = PHYSFS_fileLength( hImg );
	unsigned char* buffer = new unsigned char[static_cast<int> (fileSize)];
	// Allocate buffer for it
	if ( buffer == NULL ){
		Log::Error("Could not allocate memory for: %s.",cName);
		PHYSFS_close( hImg );
	}

	// Read the file
	*bytesRead = static_cast<int>(PHYSFS_read( hImg, buffer, 1,
				static_cast<PHYSFS_uint32>(fileSize)));
	if ( *bytesRead != fileSize ){
		Log::Error("Could not read image file: %s.",cName);
		delete [] buffer;
		buffer = NULL;
		PHYSFS_close( hImg );
		return NULL;
	}
	return buffer;
}

/**
  * Unloads the physfs library
  * \return Nonzero on success
  */
int Filesystem::DeInit() {
	int retval;
	if ( (retval = PHYSFS_deinit()) == 0 )
		Log::Error("Error de-initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	return retval;
}
