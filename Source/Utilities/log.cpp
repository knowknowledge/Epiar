/**\file			log.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Main logging facilities for the codebase
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Utilities/log.h"

/**\class Log
 * \brief Main logging facilities for the code base. */

FILE *Log::fp = NULL;
char *Log::timestamp = NULL;
string Log::logFilename = "";

void Log::Start(){
	time_t rawtime;
	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Need to replace ":" with "_" because windows can't handle it
	timestamp[ 13 ] ='_';
	timestamp[ 16 ] ='_';

	// generate the log's filename based on the time
	logFilename = string("Epiar-Log-") + string(timestamp) + string(".xml");

	fp = NULL;
}

void Log::Open() {
	// open the debug file
	fp = fopen( logFilename.c_str(), "wb" );
	if( !fp ) {
		fprintf( stderr, "Could not open \"%s\" for debugging!", logFilename.c_str() );
	} else {
		// Write the xml header
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"	);
		fprintf(fp, "<debugSession time=\"%s\" />\n", timestamp );
	}	
}

void Log::Close( void ) {
	if( fp ) {
		fprintf(fp, "</debugSession>\n");
		fclose( fp );
	}
}

void Log::realLog( int type, const char *func, const char *message, ... ) {
	va_list args;
	time_t rawtime;
	char *timestamp = NULL;
	static char logBuffer[ 4096 ] = {0};
	char logType[8] = {0};


	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Set type for logging
	switch( type ) {
		case _logMessage:
			snprintf( logType, sizeof(logType), "Message" );
			break;
		case _logWarning:
			snprintf( logType, sizeof(logType), "Warning" );
			break;
		case _logError:
			snprintf( logType, sizeof(logType), "Error" );
			break;
	}


	va_start( args, message );

	vsnprintf( logBuffer, sizeof(logBuffer), message, args );

	va_end( args );

	if( logBuffer[ strlen(logBuffer) - 1 ] == '\n' ) logBuffer[ strlen(logBuffer) - 1 ] = 0;

	// Print the message:
	if( OPTION(int, "options/log/out") == 1 ) {
		printf( "%s (%s) - ", func, logType );
		printf( "%s\n", logBuffer );
	}
	
	// Save the message to a file
	if( OPTION(int, "options/log/xml") == 1 ) {

		if( fp==NULL ){
			Log::Open();
		}

		fprintf(fp, "<log>\n");
		fprintf(fp, "\t<function>%s</function>\n\t<type>%s</type>\n\t<time>%s</time>\n\t<message>", func, logType, timestamp );
		fprintf(fp, "%s", logBuffer );
		fprintf(fp, "</message>\n</log>\n" );
	}

	return;
}
