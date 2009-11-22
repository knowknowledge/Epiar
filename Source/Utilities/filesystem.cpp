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
	Filesystem::AddPath("Resources.7z");
	// Always add executable base path.
	Filesystem::AddPath(PHYSFS_getBaseDir());
	return retval;
}

/**
  * Adds an archive to be searched for files
  * \param archivename The path to the archive
  * \return Nonzero on success
  */
int Filesystem::AddPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_addToSearchPath(archivename.c_str(), 1)) == 0 )
		Log::Error("Error adding search path.\n%s",PHYSFS_getLastError());
	return retval;
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
