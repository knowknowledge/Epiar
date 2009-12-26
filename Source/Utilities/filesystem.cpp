/**\file			filesystem.cpp
 * \author			Maoserr
 * \date			Created: Wednesday, November 18, 2009
 * \date			Modified: Wednesday, November 18, 2009
 * \brief			Provides an abstraction to the file system
 * \details
 */

#ifdef USE_PHYSICSFS

#include "includes.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"

/**\class Filesystem
 * An interface to deal with file system operations.
 * see the File class for dealing with specific files.
 * \sa File */

/**Initialize the PhysFS system
 * \return Nonzero on success. */
int Filesystem::Init( const char* argv0 ) {
	int retval;
	if ( (retval = PHYSFS_init(argv0)) == 0 )
		Log::Error("Error initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	return retval;
}

/**Initialize the PhysFS system with some default configuration.
 * The write directory is set to $HOME/.Games/Epiar
 * The search path is set to
 *  - The write directory
 *  - The directory of the current executeable
 *  - Any archives found in the path (must pass in the extension)
 * \return Nonzero on success. */
int Filesystem::Init( const char* argv0, const string &extension ) {
	int retval;
	if ( (retval = PHYSFS_init(argv0)) == 0 )
		Log::Error("Error initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	if ( (retval = PHYSFS_setSaneConfig("Games","Epiar",
				extension.c_str(),0,1) == 0) )
		Log::Error("Error initializing PhysicsFS configuration.\n%s",PHYSFS_getLastError());
	return retval;
}

/**Appends an archive to be searched for files
 * \param archivename The path to the archive
 * \return Nonzero on success */
int Filesystem::AppendPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_addToSearchPath(archivename.c_str(), 1)) == 0 )
		Log::Error("Error on appends to search path %s.\n%s",archivename.c_str(),
				PHYSFS_getLastError());
	return retval;
}

/**Prepends an archive to be searched for files
 * \param ardhivename The path to the archive (can also be a folder)
 * \return Nonzero on success */
int Filesystem::PrependPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_addToSearchPath(archivename.c_str(), 0)) == 0 )
		Log::Error("Error on prepends to search path %s.\n%s",archivename.c_str(),
				PHYSFS_getLastError());
	return retval;
}

/**Enumerates all files available in a path.
 * \param path String pointing to the path
 * \return Nonzero on success */
int Filesystem::Enumerate( const string& path )
{
	char **rc;

	rc = PHYSFS_enumerateFiles(path.c_str());
	if (rc == NULL){
		Log::Error("Failure to enumerate %s. reason: %s.\n",
				path.c_str(),PHYSFS_getLastError());
		return 0;
	}
	else
	{
		int file_count;
		char **i;
		for (i = rc, file_count = 0; *i != NULL; i++, file_count++)
			Log::Message("%s\n", *i);

		Log::Message("\n total (%d) files.\n", file_count);
		PHYSFS_freeList(rc);
		return 1;
	}
}

#if defined(_DEBUG) || defined(DEBUG)
/**Prints the current version of PhysFS.*/
void Filesystem::Version( void ){
	PHYSFS_Version compiled, linked;
	PHYSFS_VERSION(&compiled);
	PHYSFS_getLinkedVersion(&linked);
	Log::Message(" Compiled against PhysicsFS version %d.%d.%d,\n"
			" and linked against %d.%d.%d.\n\n",
			(int) compiled.major, (int) compiled.minor, (int) compiled.patch,
			(int) linked.major, (int) linked.minor, (int) linked.patch);
}

/**Prints the current archives supported.*/
void Filesystem::OutputArchivers( void ){
	const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
	const PHYSFS_ArchiveInfo **i;

	Log::Message("Supported archive types:\n");
	if (*rc == NULL)
		Log::Message(" * Apparently, NONE!\n");
	else{
		for (i = rc; *i != NULL; i++){
			Log::Message(" * %s: %s\n	Written by %s.\n	%s\n",
					(*i)->extension, (*i)->description,
					(*i)->author, (*i)->url);
		}
	}
}
#endif // defined _DEBUG or DEBUG

/**Unloads the physfs library
  * \return Nonzero on success */
int Filesystem::DeInit() {
	int retval;
	if ( (retval = PHYSFS_deinit()) == 0 )
		Log::Error("Error de-initializing PhysicsFS.\n%s",PHYSFS_getLastError());
	return retval;
}

#endif
