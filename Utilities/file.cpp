/*
 * Filename      : file.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Monday, April 21, 2008
 * Purpose       : Abstract interface for transparent file usage
 * Notes         : See file.h for more notes
 */

#include "common.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

File::File() {
	contents = NULL;
}

File::File( string filename ) {
	contents = NULL;
	
	Open( filename );
}

bool File::Open( string filename ) {
	cout << "opening file " << filename << endl;
	
	if( DEBUG ) {
		// We will check to see if the file exists in the filesystem.
		// If it does not, we will check for it in the .tgz file
		FILE *fp = NULL;
		
		fp = fopen( filename.c_str(), "rb" );
		if( !fp ) {
			// File did not exist in filesystem
			cout << "file is not on filesystem" << endl;
		} else {
			// Read in the entire file into our private File::contents buffer
			long filesize = 0;
			
			cout << "error: " << strerror(errno) << endl;
			
			// Determine the size of the file
			fseek( fp, 0, SEEK_END );
			cout << "error: " << strerror(errno) << endl;
			filesize = ftell( fp );
			cout << "error: " << strerror(errno) << endl;
			fseek( fp, 0, SEEK_SET );
			cout << "error: " << strerror(errno) << endl;
			
			cout << "file is " << filesize << " bytes" << endl;
			
			// Allocate a buffer big enough to hold the file
			assert( contents == NULL );
			contents = (unsigned char *)malloc( sizeof(unsigned char) * filesize );
			if( !contents ) {
				// Failed to allocate
				Log::Error( "Failed to allocate buffer" );
				fclose( fp );
				return( false );
			}
						
			// Read the entire file into the buffer
			if( (signed)fread( contents, 1, filesize, fp ) != filesize ) {
				Log::Error( "Failed to read entire file in one call." );
			}
			
			contentsSize = filesize;
			
			fclose( fp );
			
			cout << "finished. file is " << filesize << " bytes" << endl;
			
			return( true );
		}
	}
	
	// Note the lack of an 'else' here. We do not want to look at the file system, else look in .tgz, we look in both.
	// If the override-tgz is enabled, we look in the filesystem, else we ignore it. If the filesystem failed to find
	// the file, we simply continue by looking into the .tgz. Only if both of these fail does this function fail
	assert( epiardata );
	
	long bufSize = 0;
	contents = epiardata->LoadBuffer( filename, &bufSize );
	if( !contents ) {
		Log::Error("Could not load file from .tgz file.");
		return( false );
	}
	contentsSize = bufSize;
	
	cout << "loaded file from .tgz, it is " << contentsSize << " bytes" << endl;
	
	return( true );
}

File::~File() {
	Close();
}

bool File::Close() {
	if( contents ) {
		free( contents );
		contentsSize = 0;
	}
	
	return( true );
}

// reads 'len' bytes into a buffer. Callee must free the buffer! if len = 0 or isn't passed, the entire file is returned
void *File::Read( long *bytesRead, int len = 0 ) {
	if( len == 0 ) {
		u_byte *buf = NULL;
		
		buf = (u_byte *)malloc( sizeof(u_byte) * contentsSize );
		memcpy( buf, contents, contentsSize );
		
		*bytesRead = contentsSize;
		
		return( buf );
	} else {
		Log::Warning("Feature not fully implemented. You may only set len=0, reading the entire file at once.");
	}
	
	return( NULL );
}
