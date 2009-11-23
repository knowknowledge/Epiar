/**\file			filesystem.cpp
 * \author			Maoserr
 * \date			Created: Wednesday, November 18, 2009
 * \date			Modified: Wednesday, November 18, 2009
 * \brief			Provides an abstraction to the file system
 * \details
 */

#include "includes.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"


/**\class Filesystem
 * An interface to deal with file system operations.
 * see the File class for dealing with specific files.
 * \sa File */

/**Initialize the PhysFS system
 * \return Nonzero on success. */
int Filesystem::Init() {
	int retval;
	if ( (retval = PHYSFS_init(NULL)) == 0 )
		Log::Error("Error initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	Filesystem::AddPath("Resources.zip");
	// Always add executable base path.
	Filesystem::AddPath(PHYSFS_getBaseDir());
	return retval;
}

/**Adds an archive to be searched for files
 * \param archivename The path to the archive
 * \return Nonzero on success */
int Filesystem::AddPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_addToSearchPath(archivename.c_str(), 1)) == 0 )
		Log::Error("Error adding search path.\n%s",PHYSFS_getLastError());
	return retval;
}

/**Unloads the physfs library
  * \return Nonzero on success */
int Filesystem::DeInit() {
	int retval;
	if ( (retval = PHYSFS_deinit()) == 0 )
		Log::Error("Error de-initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	return retval;
}
