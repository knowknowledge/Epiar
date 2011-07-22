/**\file			options.h
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Sunday, May 29, 2011
 * \brief			Global Options
 * \details
 */

#ifndef __H_OPTIONS
#define __H_OPTIONS

#include "Utilities/xml.h"
#include "Utilities/string_convert.h"

// To simply access options
#define OPTION(T, path) (convertTo<T>( Options::Get(path) ))
#define SETOPTION(path, value) (Options::Set((path),(value)) )

class Options
{
	public:
		static void Initialize( const string& path );

		static bool IsLoaded();

		static void Unlock();

		static bool Save( const string& path = "" );

		static void AddDefault( const string& path, const string& value );
		static void AddDefault( const string& path, const float value );
		static void AddDefault( const string& path, const int value );

		static void RestoreDefaults();

		static string Get( const string& path );

		static void Set( const string& path, const string& value );
		static void Set( const string& path, const float value );
		static void Set( const string& path, const int value );


	private:
		static bool locked;
		static XMLFile *optionsfile;
		static XMLFile *defaults;
};

#endif // __H_OPTIONS
