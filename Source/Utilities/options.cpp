/**\file			options.cpp
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Sunday, May 29, 2011
 * \brief			Global Options
 * \details
 */

#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/options.h"

XMLFile *Options::optionsfile = NULL; ///< Static instance of the optionsfile.
XMLFile *Options::defaults = NULL; ///< Static instance of the default option values.

/**\class Options
 * \brief Container and accessor of Game options
 *
 */

void Options::Initialize( const string& path )
{
	optionsfile = new XMLFile();
	if( !optionsfile->Open( path ) )
	{
		// Create the default Options file
		optionsfile->New( path, "options");
		LogMsg(INFO, "Options file could not be found. Creating a new one." );
	}
	defaults = new XMLFile();
	defaults->New( path + ".bac", "options");
}

bool Options::IsLoaded()
{
	return (optionsfile != NULL);
}

bool Options::Save( const string& path )
{
	assert( optionsfile );
	if( path == "" )
	{
		return optionsfile->Save();
	}
	else
	{
		return optionsfile->Save( path );
	}
}

void Options::AddDefault( const string& path, const string& value )
{
	assert( defaults );
	assert( optionsfile );
	defaults->Set( path, value );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
}

void Options::AddDefault( const string& path, const float value )
{
	assert( defaults );
	assert( optionsfile );
	defaults->Set( path, value );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
}

void Options::AddDefault( const string& path, const int value )
{
	assert( defaults );
	assert( optionsfile );
	defaults->Set( path, value );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
}

void Options::RestoreDefaults()
{
	optionsfile->Copy( defaults );
}

string Options::Get( const string& path )
{
	assert( optionsfile );
	return optionsfile->Get( path );
}

void Options::Set( const string& path, const string& value )
{
	assert( optionsfile );
	optionsfile->Set( path, value );
}

void Options::Set( const string& path, const float value )
{
	assert( optionsfile );
	optionsfile->Set( path, value );
}

void Options::Set( const string& path, const int value )
{
	assert( optionsfile );
	optionsfile->Set( path, value );
}
