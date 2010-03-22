/**\file			log.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Main logging facilities for the codebase
 * \details
 */

#ifndef __H_LOG__
#define __H_LOG__

#include "includes.h"

class Log {
	public:
		#define _logMessage 0
		#define _logWarning 1
		#define _logError   2

		// __PRETTY_FUNCTION__ is a GNU extension
		// __FUNCTION__ is supported by many compilers, including microsoft
		// if we don't know the compiler, assume we don't have it and revert to __FILE__
#ifdef __GNUC__
		#define Message( ... ) realLog( _logMessage, __PRETTY_FUNCTION__, __VA_ARGS__ )
		#define Warning( ... ) realLog( _logWarning, __PRETTY_FUNCTION__, __VA_ARGS__ )
		#define Error( ... )   realLog( _logError, __PRETTY_FUNCTION__, __VA_ARGS__ )
#elif defined(_MSC_VER)
		#define Message( ... ) realLog( _logMessage, __FUNCTION__, __VA_ARGS__ )
		#define Warning( ... ) realLog( _logWarning, __FUNCTION__, __VA_ARGS__ )
		#define Error( ... )   realLog( _logError, __FUNCTION__, __VA_ARGS__ )
#else
		#define Message( ... ) realLog( _logMessage, __FILE__ __LINE__, __VA_ARGS__ )
		#define Warning( ... ) realLog( _logWarning, __FILE__ __LINE__, __VA_ARGS__ )
		#define Error( ... )   realLog( _logError, __FILE__ __LINE__, __VA_ARGS__ )
#endif 

		static void realLog( int type, const char *func, const char *message, ... );

		static void Start( void );
		static void Close( void );

	private:
		static void Open( void );

		static char *timestamp;
		static string logFilename;
		static FILE *fp; // pointer to the log
};

#endif // __H_LOG__
