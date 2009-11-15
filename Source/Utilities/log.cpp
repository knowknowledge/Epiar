/*
 * Filename      : log.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, June 4, 2006
 * Last Modified : Saturdayy, January 5, 2008
 * Purpose       : Main logging facilities for the codebase
 * Notes         :
 */

#include "common.h"
#include "includes.h"
#include "Utilities/log.h"

FILE *Log::fp = NULL;

void Log::Initalize( void ) {
#ifdef DEBUG
	time_t rawtime;
	char logFilename[128] = {0};
	char *timestamp = NULL;

	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Need to replace ":" with "_" because windows can't handle it
	timestamp[ 13 ] ='_';
	timestamp[ 16 ] ='_';

	// generate the log's filename based on the time
	sprintf( logFilename, "Epiar-Log-%s.xml", timestamp );

	// open the debug file
	fp = fopen( logFilename, "wb" );
	if( !fp ) {
		fprintf( stderr, "Could not open \"%s\" for debugging!", logFilename );
	} else {
		// Write the xml header
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"	);
		fprintf(fp, "<debugSession time=\"%s\" />\n", timestamp );
	}
#endif /* DEBUG */
}

void Log::Close( void ) {
#ifdef DEBUG
	if( fp ) {
		fprintf(fp, "</debugSession>\n");
		fclose( fp );
	}
#endif /* DEBUG */
}

void Log::realLog( int type, const char *func, const char *message, ... ) {
	va_list args;
	time_t rawtime;
	char *timestamp;
	static char logBuffer[ 4096 ] = {0};
	char logType[8] = {0};

	if( !fp ) return;

	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Set type for logging
	switch( type ) {
		case _logMessage:
			sprintf( logType, "Message" );
			break;
		case _logWarning:
			sprintf( logType, "Warning" );
			break;
		case _logError:
			sprintf( logType, "Error" );
			break;
	}

	fprintf(fp, "<log>\n");
	fprintf(fp, "\t<function>%s</function>\n\t<type>%s</type>\n\t<time>%s</time>\n\t<message>", func, logType, timestamp );

#ifdef DEBUG
	printf( "%s (%s) - ", func, logType );
#endif

	va_start( args, message );

	vsnprintf( logBuffer, 4095, message, args );

	va_end( args );

	if( logBuffer[ strlen(logBuffer) - 1 ] == '\n' ) logBuffer[ strlen(logBuffer) - 1 ] = 0;

	fprintf( fp, "%s\n", logBuffer );
#ifdef DEBUG
	printf( "%s\n", logBuffer );
#endif

	fprintf( fp, "</message>\n</log>\n" );

	return;
}
