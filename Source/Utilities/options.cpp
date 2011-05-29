/**\file			options.cpp
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Sunday, May 29, 2011
 * \brief			Global Options
 * \details
 */

#include "Utilities/options.h"

XMLFile *Options::optionsfile = NULL; ///< Static instance of the optionsfile.

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
	}
}

bool Options::IsLoaded()
{
	return (optionsfile != NULL);
}

bool Options::Save( const string& path )
{
	assert( optionsfile );
	if( path != "" )
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
	assert( optionsfile );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
}

void Options::AddDefault( const string& path, const float value )
{
	assert( optionsfile );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
}

void Options::AddDefault( const string& path, const int value )
{
	assert( optionsfile );
	if( false == optionsfile->Has(path) )
	{
		Set(path,value);
	}
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
