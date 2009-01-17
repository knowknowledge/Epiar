/*
 * Filename      : log.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, June 4, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Header for logging facilities
 * Notes         :
 */

#ifndef __H_LOG__
#define __H_LOG__

#include "includes.h"

class Log {
	public:
		#define _logMessage 0
		#define _logWarning 1
		#define _logError   2

		#define Message( ... ) realLog( _logMessage, __PRETTY_FUNCTION__, __VA_ARGS__ );
		#define Warning( ... ) realLog( _logWarning, __PRETTY_FUNCTION__, __VA_ARGS__ );
		#define Error( ... )   realLog( _logError, __PRETTY_FUNCTION__, __VA_ARGS__ );

		static void Initalize( void );

		static void realLog( int type, const char *func, char *message, ... );

		static void Close( void );

	protected:
		Log();
		Log( const Log & );
		Log& operator= (const Log&);

	private:
		static FILE *fp; // pointer to the log
};

#endif // __H_LOG__
