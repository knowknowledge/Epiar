/**\file			options.cpp
 * \author			Matt Zweig
 * \date			Created:  Sunday, May 29, 2011
 * \date			Modified: Sunday, May 29, 2011
 * \brief			Global Options
 * \details
 */

#include "includes.h"
#include "Utilities/options.h"

bool Options::locked = true;
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
		optionsfile->New( path, "options" );
	}
	defaults = new XMLFile();
	defaults->New( path + ".bac", "options" );
}

void Options::Unlock()
{
	locked = false;
}

bool Options::IsLoaded()
{
	return (!locked) && (optionsfile != NULL);
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
		optionsfile->Set(path,value);
		assert( value == Get(path) );
	}
}

void Options::AddDefault( const string& path, const float value )
{
	assert( defaults );
	assert( optionsfile );
	defaults->Set( path, value );
	if( false == optionsfile->Has(path) )
	{
		optionsfile->Set(path,value);
		assert( value == convertTo<float>(Get(path)) );
	}
}

void Options::AddDefault( const string& path, const int value )
{
	assert( defaults );
	assert( optionsfile );
	defaults->Set( path, value );
	if( false == optionsfile->Has(path) )
	{
		optionsfile->Set(path,value);
		assert( value == convertTo<int>(Get(path)) );
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
