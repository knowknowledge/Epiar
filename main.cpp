/*
 * Filename      : main.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, June 4, 2006
 * Purpose       : Main entry point of Epiar codebase
 * Notes         :
 */

#include "common.h"
#include "Debug/graphics.h"
//#include "Debug/lua_test.h"
#include "Engine/simulation.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "includes.h"
#include "UI/ui.h"
#include "Utilities/archive.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

// parse command line switches
int parseArgs( int argc, char **argv );

// main data file, used throughout the tree (extern in common.h)
Archive *epiardata = NULL;
// main configuration file, used through the tree (extern in common.h)
XMLFile *optionsfile = NULL;
// main font used throughout the game
Font *Vera8 = NULL, *Vera10 = NULL, *Visitor10 = NULL, *VeraMono10 = NULL;

int main( int argc, char **argv ) {
	Log::Initalize();

	// load the main data files (used throughout the tree)
	epiardata = new Archive( "data.tgz" );

	// load the main configuration file (used throughout the tree)
	optionsfile = new XMLFile( "options.xml" );

	Log::Message( "Epiar %s starting up.", EPIAR_VERSION_FULL );

#ifdef COMP_MSVC
	Log::Message( "Compiled with MSVC vers: _MSC_VER" );
#endif // COMP_MSVC
#ifdef COMP_GCC
	Log::Message( "Compiled with GCC vers: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif // COMP_GCC

	Video::Initialize();
	Video::SetWindow( OPTION( int, "options/video/w" ), OPTION( int, "options/video/h"), OPTION( int, "options/video/bpp") );

	// load the main font used through the tree
	Vera8 = new Font( "Vera-8.af" );
	Vera10 = new Font( "Vera-10.af" );
	Visitor10 = new Font( "Visitor1-10.af" );
	VeraMono10 = new Font( "VeraMono-10.af" );

	if( parseArgs( argc, argv ) == 0 ) {
		Simulation debug( "sim-debug.xml" );
		debug.Run();
	}

	Video::Shutdown();
	
	Log::Message( "Epiar shutting down." );
	
	// free the main font files
	delete Vera8;
	delete Vera10;
	delete Visitor10;
	delete VeraMono10;
	// free the main data files
	delete epiardata;
	// free the configuration file data
	delete optionsfile;
	
	Log::Close();

	return( 0 );
}

// parse command line switches. returns -1 if a switch indicates the game should not be run, e.g. --help
// NOTE: cmd line args override settings in options.xml (found in data.tgz) for just this run of the program
int parseArgs( int argc, char **argv ) {
	for( int i = 1; i < argc; i++ ) {
		// remove any leading - or --
		if( (char)argv[i][0] == '-' ) argv[1] = &argv[1][1]; // handle a single '-', eg -help
		if( (char)argv[i][0] == '-' ) argv[1] = &argv[1][1]; // purposefully repeated to handle '--', eg --help
		
		// it'd be nice if we could overload the switch control structure to accept std::string, sigh
		string parm = argv[i];
		
		if( parm == "help" ) {
			// remember to keep this list updated when new parms are added
			printf("\n\t--help          - Displays this message");
			printf("\n\t--version       - Displays program version");
			printf("\n\t--ui-demo       - Runs a debug/display demo of the UI");
			printf("\n\t--graphics-demo - Runs a debug/display demo of various graphics functionality");
			printf("\n\t--lua-test      - Tests the Lua scripting functionality");
			printf("\n");
			return( -1 ); // indicates we should quit immediately and not run
		} else if( parm == "version" ) {
			printf("\nEpiar version %s", EPIAR_VERSION_FULL );
			printf("\n");
			return( -1 ); // indicates we should quit immediately and not run
		} else if( parm == "ui-demo" ) {
			ui_demo( true ); // temporary function

			return( -1 );
		} else if( parm == "graphics-demo" ) {
			graphics_demo(); // temporary function
			return( -1 );
		} else if( parm == "lua-test" ) {
			//lua_test(); // temporary function
			return( -1 );
		}
	}
	
	return( 0 );
}
