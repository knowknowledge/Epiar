/**\file			log.cpp
 * \author			Chris Thielen (chris@epiar.net)
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

/**\brief Empty destructor.*/
Log::~Log(){
}

/**\brief Retrieves the current instance of the log class.*/
Log& Log::Instance( void ){
	static Log _instance;
	return _instance;
}

/**\brief Allows changing of the log level dynamically (string version).*/
bool Log::SetLevel( const string& _loglvl ){
	// Check logging level
	this->loglvl=this->ReverseLookUp( _loglvl );
	if( this->loglvl == INVALID ){
		LogMsg(DEBUG1,"Invalid logging level specified, reverting to default log level.");
		this->loglvl=this->loglvldefault;
		return false;
	}
	return true;
}

/**\brief Allows changing of the log level dynamically ( enum version ).*/
bool Log::SetLevel( Level _loglvl ){
	LogMsg(DEBUG1,"Changing Log Level from '%s' to '%s'.", lvlStrings[loglvl].c_str(), lvlStrings[_loglvl].c_str());
	this->loglvl=_loglvl;
	return true;
}

/**\brief Changes the function filter.*/
void Log::SetFunFilter( const string& _funfilter ){
	this->funfilter.clear();
	// Check function filter
	if( !_funfilter.empty() ){
		LogMsg(DEBUG1,"Filtering log by function named: %s.", _funfilter.c_str());
		this->funfilter = _funfilter;
	}
}

/**\brief Changes the message filter.*/
void Log::SetMsgFilter( const string& msgfilter ){
	this->filter.clear();
	// Check message filter
	if( !msgfilter.empty() ){
		LogMsg(DEBUG1,"Filter log by message text: %s", msgfilter.c_str());
		this->filter=msgfilter;
	}
}


/**\brief Opens the log file.*/
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

/**\brief Frees the handle to the log file.*/
void Log::Close( void ) {
	if( fp ) {
		fprintf(fp, "</debugSession>\n");
		fclose( fp );
	}
}

/**\brief The real log function.*/
void Log::realLog( Level lvl, const string& func, const char *message, ... ) {
	// Check log level
	if( lvl < this->loglvl )
		return;

	// Check function filter
	if( !this->funfilter.empty() ){
		if( !func.find(this->funfilter) )
			return;
	}

	// Check message filter
	if( !this->filter.empty() ){
		//if( !message.find( this->filter  ) )
		//	return;
	}

	va_list args;
	time_t rawtime;
	static char logBuffer[4096];

	time( &rawtime );

	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	va_start( args, message );
	vsnprintf( logBuffer, sizeof(logBuffer), message, args );
	va_end( args );

	if( logBuffer[ strlen(logBuffer) - 1 ] == '\n' ) logBuffer[ strlen(logBuffer) - 1 ] = 0;

	// Print the message:
	if( OPTION(int, "options/log/out") == 1 )
		cout<<func<<" ("<<lvlStrings[lvl]<<") - "<< logBuffer <<endl;;
	
	// Save the message to a file
	if( OPTION(int, "options/log/xml") == 1 ) {

		if( fp==NULL ){
			Log::Open();
		}

		fprintf(fp, "<log>\n");
		fprintf(fp, "\t<function>%s</function>\n\t<type>%s</type>\n\t<time>%s</time>\n\t<message>", func.c_str(), lvlStrings[lvl].c_str(), timestamp );
		fprintf(fp, "%s", logBuffer );
		fprintf(fp, "</message>\n</log>\n" );
	}
}

/**\brief Constructor, used to initialize variables.*/
Log::Log()
	:loglvldefault(ALL)
{
	time_t rawtime;

	lvlStrings[NONE]="None";
	lvlStrings[FATAL]="Fatal";
	lvlStrings[CRITICAL]="Critical";
	lvlStrings[ERR]="Error";
	lvlStrings[WARN]="Warn";
	lvlStrings[ALERT]="Alert";
	lvlStrings[NOTICE]="Notice";
	lvlStrings[INFO]="Info";
	lvlStrings[VERBOSE1]="Verbose1";
	lvlStrings[VERBOSE2]="Verbose2";
	lvlStrings[VERBOSE3]="Verbose3";
	lvlStrings[DEBUG1]="Debug1";
	lvlStrings[DEBUG2]="Debug2";
	lvlStrings[DEBUG3]="Debug3";
	lvlStrings[DEBUG4]="Debug4";

	time( &rawtime );
	timestamp = ctime( &rawtime );
	timestamp[ strlen(timestamp) - 1 ] = 0;

	// Need to replace ":" with "_" because windows can't handle it
	timestamp[ 13 ] ='_';
	timestamp[ 16 ] ='_';

	// generate the log's filename based on the time
	logFilename = string("Epiar-Log-") + string(timestamp) + string(".xml");
	printf("Logging to: '%s'\n",logFilename.c_str());

	fp = NULL;
}

/**\brief Does a reverse lookup of the log level based on a string.*/
Log::Level Log::ReverseLookUp( const string& _lvl ){
	// Figure out which log level we're doing.
	map<Level,string>::iterator it;
	for ( it=lvlStrings.begin() ; it != lvlStrings.end(); it++ ){
		if( (*it).second == _lvl )
			return (*it).first;
	}

	return INVALID;
}

