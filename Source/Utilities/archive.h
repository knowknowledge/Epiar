/*
 * Filename      : archive.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, January 5, 2008
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : To work with .tar.gz files
 * Notes         :
 */

#ifndef __H_ARCHIVE__
#define __H_ARCHIVE__

#include "Graphics/image.h"
#include "includes.h"
#include <zlib.h>

class Archive {
	public:
		Archive();
		Archive( string filename );
		~Archive();
		bool Open( string filename );
		Image LoadPNG( string filename );
		u_byte *LoadBuffer( string filename, long *bufSize );
		xmlDocPtr LoadXML( string filename );
		void _DebugList( void );
		
	private:
		// Moves fp to the start of the file contents (past the header) and returns the filesize. -1 on error
		long Seek( string filename );
		
		typedef struct _ustar_header {
			u_byte fileName[100];
			u_byte fileMode[8];
			u_byte owner_id[8];
			u_byte group_id[8];
			u_byte fileSize[12]; // in bytes, encoded in octal with leading zeroes
			u_byte lastModTime[12];
			u_byte checkSum[8];
			u_byte typeFlag;
			u_byte nameLinked[100];
			u_byte ustarIndicator[6]; // "ustar"
			u_byte ustarVersion[2]; // 00
			u_byte ownerUser[32];
			u_byte ownerGroup[32];
			u_byte devMaj[8];
			u_byte devMin[8];
			u_byte filenamePrefix[155];
			u_byte zeroes[12];
		} ustar_header_t;
		
		typedef struct _contents_node {
			string filename;
			unsigned long filesize;
			unsigned long offset; // location of file's header start from SEEK_SET
			ustar_header_t header;
		} contents_node_t;

		list<contents_node_t> contents; // table of contents
		string filename; // name of archive on system
		gzFile fp;
};

#endif // __H_ARCHIVE__
