/**\file			file.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Low level interface for file access.
 * \details
 * Use filesystem for higher level access.*/

#include "includes.h"
#include "Utilities/file.h"
#include "Utilities/log.h"

#ifndef USE_PHYSICSFS
#define PHYSFS_getLastError() "FAILED!"
#endif


/** \class File
 * Low level file access abstraction through PhysicsFS. */

/**Creates empty file instance.*/
File::File( void ):
fp(NULL), contentSize(0),validName("") {
}

/**Creates file instance linked to filename. \sa Open.*/
File::File( const string& filename, bool writable ):
fp(NULL), contentSize(0), validName("") {
	if( writable )
		OpenWrite( filename );
	else
		OpenRead( filename );
}

/**Opens a file for reading
 * \param filename The filename path.
 * \return true if successful, false otherwise.*/
bool File::OpenRead( const string& filename ) {
	if ( fp != NULL )
		this->Close();

	const char *cName;

	cName = filename.c_str();

	// Check for file existence
	if( !File::Exists(filename) ){
		LogMsg(WARN,"Could not open file for reading. File does not exist.\n");
		return false;
	}

#ifdef USE_PHYSICSFS
	fp = PHYSFS_openRead( cName );
#else
	fp = fopen(cName,"rb");
#endif
	if( fp == NULL ){
		LogMsg(ERR,"Could not open file: %s.\n%s", cName,
			PHYSFS_getLastError());
		return false ;
	}
	
#ifdef USE_PHYSICSFS
	contentSize = static_cast<long>( PHYSFS_fileLength( fp ) );
#else
	struct stat fileStatus;
	int stat_ret = stat(cName, &fileStatus );
	if (stat_ret)
		return false;
	contentSize = fileStatus.st_size;
#endif
	validName.assign( filename );

	return true ;
}

/**Opens a file for writing
 * \param filename The filename path
 * \return true if successful, false otherwise.*/
bool File::OpenWrite( const string& filename ) {
	if ( fp != NULL )
		this->Close();

	const char *cName;
	cName = filename.c_str();
#ifdef USE_PHYSICSFS
	this->fp = PHYSFS_openWrite( cName );
#else
	this->fp = fopen( cName, "wb");
#endif
	if( fp == NULL ){
		LogMsg(ERR,"Could not open file (%s) for writing: %s\n",cName,
				PHYSFS_getLastError());
		return false;
	}
	validName.assign( filename );

	return true;
}

/**Returns the full path to the file
 * \return path successful, NULL otherwise.*/
string File::GetAbsolutePath(){
	string abs = PHYSFS_getRealDir( validName.c_str() );

	abs += PHYSFS_getDirSeparator() + validName;

	return abs;
}

/**Reads a specified number of bytes.
 * \param numBytes Number of bytes to read.
 * \param buffer Buffer to read bytes into.
 * \return true if successful, false otherwise.*/
bool File::Read( long numBytes, char *buffer ){
	if ( fp == NULL )
		return false;

	long bytesRead = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_read( fp, buffer, 1, numBytes )
#else
		fread(buffer, 1, numBytes, fp )
#endif
		);
	if ( bytesRead == numBytes ){
		return true;
	} else {
		LogMsg(ERR,"%s: Unable to read specified number of bytes. %s",
			validName.c_str(), PHYSFS_getLastError());
		return false;
	}
}

/**Reads the whole file into a buffer. Buffer will be automatically allocated
 * for you, but you must explicitly free it by using "delete [] buffer"
 * \return Pointer to buffer, NULL otherwise.*/
char *File::Read( void ){
	if ( fp == NULL )
		return NULL;

	// Seek to beginning
	Seek( 0 );
	char *fBuffer = new char[static_cast<Uint32>(contentSize)];
	long bytesRead = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_read( this->fp, fBuffer, 1, contentSize )
#else
		fread(fBuffer,1,contentSize,fp)
#endif
		);
	if( bytesRead == contentSize){
		return fBuffer;
	} else {
		delete [] fBuffer;
		LogMsg(ERR,"%s: Unable to read file into memory. %s",
			validName.c_str(), PHYSFS_getLastError());
		return NULL;
	}
}

/**Writes buffer to file.
 * \param buffer The buffer to write
 * \param bufsize The size of the buffer in bytes
 * \return true if successful, false otherwise. */
bool File::Write( char *buffer, const long bufsize ){
	if ( fp == NULL )
		return false;
#ifdef USE_PHYSICSFS
	PHYSFS_sint64 bytesWritten = PHYSFS_write(this->fp, buffer, sizeof(char), bufsize);
#else
	long bytesWritten = fwrite(buffer,1,bufsize,fp);
#endif
	if ( bytesWritten != bufsize){
		LogMsg(ERR,"%s: Unable to write to file. %s",this->validName.c_str(),
			PHYSFS_getLastError());
		return false;
	}
	return true;
}

/**Gets the current offset from the beginning of the file.
 * \return Offset in bytes from the beginning of the file.*/
long File::Tell( void ){
	long offset;
	offset = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_tell( fp )
#else
		ftell(fp)
#endif
		);
	if ( offset == -1 ){
		LogMsg(ERR,"%s: Error using file tell. %s",
			validName.c_str(), PHYSFS_getLastError());
	}
	return offset;
}

/**Seek to new position in the file.
 * \param pos Position in bytes form the beginning of the file.
 * \return true if successful, false otherwise.*/
bool File::Seek( long pos ){
	if ( fp == NULL )
		return false;
#ifdef USE_PHYSICSFS
	int retval;
	retval = PHYSFS_seek( fp,
		static_cast<PHYSFS_uint64>( pos ));
	if ( retval == 0 ){
		LogMsg(ERR,"%s: Error using file seek [%ld]. %s",
		                validName.c_str(), pos, PHYSFS_getLastError());
		return false;
	}
#else
	const char *cName;
	cName = validName.c_str();
	fseek(fp, pos, SEEK_SET);
#endif
	return true;
}

/**Gets the total length in bytes of the file.
 * \return Total length of the file in bytes.*/
long File::GetLength( void ){
	return contentSize;
}

/**Sets the internal buffer for read/write operations.
 * \return Nonzero on success */
int File::SetBuffer( int bufSize ){
#ifdef USE_PHYSICSFS
	if ( PHYSFS_setBuffer( fp, bufSize ) == 0 ){
		LogMsg(ERR,"Could not create internal buffer for file: %s.\n%s",
				validName.c_str(),PHYSFS_getLastError());
		PHYSFS_close( fp );
		return 0;
	}
	return 1;
#else
	return 0; // No idea how this works??? ~ Matt Zweig
#endif
}

/**Destroys file instance. \sa Close.*/
File::~File() {
	Close();
}

/**Closes the file handle and frees associated buffer.
 * \return true if successful, false otherwise*/
bool File::Close() {
	if ( validName.compare( "" ) == 0 )
		return NULL;

	if ( fp == NULL )
		return false;

#ifdef USE_PHYSICSFS
	int retval = PHYSFS_close( fp );
	if ( retval == 0 )
#else
	int retval = fclose( fp );
	if ( retval != 0 )
#endif
	{
		LogMsg(ERR,"%s: Unable to close file handle.%s",
			validName.c_str(), PHYSFS_getLastError());
		return false;
	}
	contentSize = 0;
	return true;
}

bool File::Exists( const string& filename ) {
	const char *cName;
	cName = filename.c_str();
#ifdef USE_PHYSICSFS
	if ( !PHYSFS_exists( cName ) ){
		LogMsg(ERR,"File does not exist: %s.", cName);
		return false;
	}
#else
	struct stat fileStatus;
	int stat_ret = stat(cName, &fileStatus );
	if ( stat_ret != 0 ) {
		//LogMsg(INFO, "Stat for %s: [%d]\n",cName,stat_ret);
		switch( stat_ret ) {
			case EACCES:        LogMsg(ERR,"Epiar cannot access:%s.", cName); break;
			case EFAULT:        LogMsg(ERR,"Invalid address: %s.", cName); break;
			case EIO:           LogMsg(ERR,"An I/O Error Occured: %s.", cName); break;
			default:			LogMsg(ERR,"Unknown error occurred: %s.", cName);
		}
		return false;
	}
#endif
	return true;
}

bool File::IsDir( const string& filename ) {
	// TODO: determine if the filename is a directory
	// This can be used for walking a directory tree
	// This can be used for skipping directories
	return false;
}

bool IsBigEndian() {
	int test_var = 1;
	unsigned char *test_array = (unsigned char*)&test_var;
	return (test_array[0] == '\0');
}

