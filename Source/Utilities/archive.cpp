/*
 * Filename      : archive.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : To work with .tar.gz files
 * Notes         :
 */

#include "Utilities/archive.h"
#include "Utilities/log.h"
#include <zlib.h>

Archive::Archive() {
	// hella cool constructor (hcc)
}

Archive::Archive( string filename ) {
	Open( filename );
}

bool Archive::Open( string filename ) {
	fp = gzopen( filename.c_str(), "rb" );
	
	if( fp == NULL ) {
		Log::Error( "Could not open archive file '%s'\n", filename.c_str() );
		return( false );
	}
	
	this->filename = filename;
	
	return( true );
}

Archive::~Archive() {
	if( filename.length() > 0 ) {
		// an archive is open
		gzclose( fp );
		fp = NULL;
	}
}

Image Archive::LoadPNG( string filename ) {
	Image img;
	unsigned char *buf = NULL;
	long bufSize = 0;
	
	buf = LoadBuffer( filename, &bufSize );
	if( buf == NULL ) {
		Log::Error( "Could not open PNG from archive. Buffer failed to allocate." );
		return( img );
	}
	
	img.Load( buf, bufSize );
	free( buf );

	return( img );
}

u_byte *Archive::LoadBuffer( string filename, long *bufSize ) {
	long filesize = 0;
	u_byte *buf = NULL;
	
	// find the file in the archive
	filesize = Seek( filename );
	
	if( filesize == -1 ) {
		Log::Error( "Could not find '%s' in the archive to load into buffer.", filename.c_str() );
		return( NULL );
	}
	
	// allocate the buffer
	buf = (u_byte *)malloc( filesize );
	memset( buf, 0, filesize );
	
	if( gzread( fp, buf, filesize ) != filesize ) {
		Log::Error( "gzread() failed to read entire file." );
		free( buf );
		return( NULL );
	}

	*bufSize = filesize;
	
	return( buf );
}

xmlDocPtr Archive::LoadXML( string filename ) {
	unsigned char *buf = NULL;
	long bufSize = 0;
	xmlDocPtr doc = NULL;
	
	buf = LoadBuffer( filename, &bufSize );
	if( buf == NULL ) {
		Log::Error( "Could not load XML from archive. Buffer failed to allocate." );
		return( NULL );
	}

	doc = xmlParseFile( (const char *)filename.c_str() );
	free( buf );

	return( doc );
}

// Moves fp to the start of the file contents (past the header) and returns the filesize. -1 on error
long Archive::Seek( string filename ) {
	ustar_header_t buf;
	int endOfArchive = 0;

	// start at the beginning of the archive
	gzseek( fp, 0, SEEK_SET );
	
	while( gzread( fp, &buf, sizeof( buf ) ) > 0 ) {
		long fileSize = 0;
		
		// check for end of archive
		int i;
		for( i = 0; i < 512; i++ ) {
			u_byte *p = (u_byte *)&buf;
			if( *p != 0 ) break;
		}
		if( i == 512 ) {
			// header block is blank ...
			endOfArchive++;
			
			if( endOfArchive == 2 ) break;
		} else {
			//printf("filename: %s\n", buf.fileName );
			
			// fileSize is a string representation of an octal number. convert to decimal.
			//printf("filesize (str): %s\n", buf.fileSize );
			int pow = 0;
			for( i = 10; i >= 0; i-- ) {
				char l = buf.fileSize[i];
				int c = atoi( &l );
				printf("i = %d, c = %d\n", i, c );
				if( pow > 0 )
					fileSize += c * ( 8 * pow );
				else
					fileSize += c;
				pow++;
			}
			
			// is this the right file? if so, return it's size. fp is already at the start of the file contents (just past the header)
			if( !strncmp( filename.c_str(), (const char *)buf.fileName, strlen( filename.c_str() ) ) ) return( fileSize );
			
			//printf("filesize: %d\n", (int)fileSize );
			//printf("amt of paddig: %d\n", 512 - (int)(fileSize % 512));
			
			// these aren't the droids we're looking for ...
			// seek past this file
			gzseek( fp, fileSize, SEEK_CUR );
			// all tar files are 512-byte padded
			gzseek( fp, 512 - (int)(fileSize % 512), SEEK_CUR );
		}
	}
	
	return( -1 );
}

void Archive::_DebugList( void ) {
	gzFile fp;
	ustar_header_t buf;
	int endOfArchive = 0;
	
	fp = gzopen( filename.c_str(), "rb" );
	assert( fp != NULL );

	while( gzread( fp, &buf, sizeof( buf ) ) > 0 ) {
		long fileSize = 0;
		
		// check for end of archive
		int i;
		for( i = 0; i < 512; i++ ) {
			u_byte *p = (u_byte *)&buf;
			if( *p != 0 ) break;
		}
		if( i == 512 ) {
			// header block is blank ...
			endOfArchive++;
			
			if( endOfArchive == 2 ) break;
		} else {
			printf("filename: %s\n", buf.fileName );
			
			// fileSize is a string representation of an octal number. convert to decimal.
			printf("filesize (str): %s\n", buf.fileSize );
			int pow = 0;
			for( i = 10; i >= 0; i-- ) {
				char l = buf.fileSize[i];
				int c = atoi( &l );
				printf("i = %d, c = %d\n", i, c );
				if( pow > 0 )
					fileSize += c * ( 8 * pow );
				else
					fileSize += c;
				pow++;
			}
			
			printf("filesize: %d\n", (int)fileSize );
			printf("amt of paddig: %d\n", 512 - (int)(fileSize % 512));
			
			// seek past this file
			gzseek( fp, fileSize, SEEK_CUR );
			// all tar files are 512-byte padded
			gzseek( fp, 512 - (int)(fileSize % 512), SEEK_CUR );
		}
	}
	
	gzclose( fp );
}
