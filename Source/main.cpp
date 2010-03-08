/**\file		main.cpp
 * \author		Chris Thielen (chris@luethy.net)
 * \author		and others.
 * \date		Created:	Sunday, June 4, 2006
 * \date		Modified:	Thursday, November 19, 2009
 * \brief		Main entry point of Epiar codebase
 * \details
 *	This file performs two functions:
 *		- Runs the Epiar simulation.
 *		- Parse command line arguments.
 */

#include "includes.h"
#include "common.h"
#include "Audio/audio.h"
#include "Tests/graphics.h"
#include "Engine/simulation.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

// parse command line switches
int parseArgs( int argc, char **argv );

// main configuration file, used through the tree (extern in common.h)
XMLFile *optionsfile = NULL;
// main font used throughout the game
Font *SansSerif = NULL, *BitType = NULL, *Serif = NULL, *Mono = NULL;

/**Main runtime.
 * \return 0 always
 * \details
 * This function does the following:
 *  - Load options
 *  - Load fonts
 *  - Calls parseArgs to parse command line
 *  - Runs the Simulation routine
 *  - Calls any cleanup code
 */
int main( int argc, char **argv ) {
	// Use ".dat" extension for data files
#ifdef USE_PHYSICSFS
	Filesystem::Init( argv[0], "dat" );
#endif

#ifdef __APPLE__
	string path = argv[0];
	if( path.find("MacOS/Epiar") ){ // If this is being run from inside a Bundle
		// Chdir to the Bundle Contents
		string ContentsPath = path.substr(0, path.find("MacOS/Epiar") );
		chdir(ContentsPath.c_str());
	}
#endif

	// load the main configuration file (used throughout the tree)
	optionsfile = new XMLFile( "Resources/Definitions/options.xml" );

	Log::Start();
	Log::Message( "Epiar %s starting up.", EPIAR_VERSION_FULL );

#ifdef COMP_MSVC
	Log::Message( "Compiled with MSVC vers: _MSC_VER" );
#endif // COMP_MSVC
#ifdef COMP_GCC
	Log::Message( "Compiled with GCC vers: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif // COMP_GCC

	Log::Message("Executable Path: %s", argv[0]);

	Video::Initialize();
	Video::SetWindow( OPTION( int, "options/video/w" ), OPTION( int, "options/video/h"), OPTION( int, "options/video/bpp") );
	Audio::Instance().Initialize();
	Audio::Instance().SetMusicVol ( OPTION(float,"options/sound/musicvolume") );
	Audio::Instance().SetSoundVol ( OPTION(float,"options/sound/soundvolume") );

	Log::Message("Using Font Engine: FreeType");
	//******** FreeType Rendering ********
	SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
	BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
	Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
	Mono            = new Font( "Resources/Fonts/FreeMono.ttf" );

	if( parseArgs( argc, argv ) == 0 ) {
		Simulation debug( "Resources/Definitions/sim-debug.xml" );
		debug.Run();
	}

	Video::Shutdown();
	Audio::Instance().Shutdown();

	Log::Message( "Epiar shutting down." );
	
	// free the main font files
	delete SansSerif;
	delete BitType;
	delete Serif;
	delete Mono;
	// free the configuration file data
	delete optionsfile;
	
#ifdef USE_PHYSICSFS
	Filesystem::DeInit();
#endif
	Log::Close();

	return( 0 );
}

/**Parse command line switches.
 * \return -1 if a switch indicates the game should not be run, e.g. --help
 * \details
 * Cmd line args override settings in options.xml (found in data.tgz) 
 * for just this run of the program.
 */
int parseArgs( int argc, char **argv ) {
	for( int i = 1; i < argc; i++ ) {
		// remove any leading - or --
		if( (char)argv[i][0] == '-' ) argv[1] = &argv[1][1]; // handle a single '-', eg -help
		if( (char)argv[i][0] == '-' ) argv[1] = &argv[1][1]; // purposefully repeated to handle '--', eg --help
		
		// it'd be nice if we could overload the switch control structure to accept std::string, sigh
		string parm = argv[i];
		Log::Message("Argument[%d]: %s", i,argv[i]);
		
		if( parm == "help" ) {
			// remember to keep this list updated when new parms are added
			printf("\n\t--help           - Displays this message");
			printf("\n\t--version        - Displays program version");
			printf("\n\t--editor-mode    - Editor and edit Game Components");
			printf("\n\t--ui-demo        - Runs a debug/display demo of the UI");
			printf("\n\t--no-audio       - Turns off all sounds.");
			printf("\n\t--[no]log-xml    - Turn on logggin to an XML file");
			printf("\n\t--[no]log-stdout - Turn on logging to standard out");
			//printf("\n\t--graphics-demo - Runs a debug/display demo of various graphics functionality");
			printf("\n\t--lua-test       - Tests the Lua scripting functionality");
			printf("\n");
			return( -1 ); // indicates we should quit immediately and not run
		} else if( parm == "version" ) {
			printf("\nEpiar version %s", EPIAR_VERSION_FULL );
			printf("\n");
			return( -1 ); // indicates we should quit immediately and not run
		} else if( parm == "editor-mode" ) {
			SETOPTION("options/development/editor-mode",1);
		} else if( parm == "ui-demo" ) {
			ui_demo( true ); // temporary function
			return( -1 );
		} else if( parm == "no-audio" ) {
			cout<<"turning off sound"<<endl;
			SETOPTION("options/sound/background",0);
			SETOPTION("options/sound/weapons",0);
			SETOPTION("options/sound/engines",0);
			SETOPTION("options/sound/explosions",0);
			SETOPTION("options/sound/buttons",0);
		} else if( parm == "graphics-demo" ) {
			//graphics_demo(); // temporary function
			return( -1 );
		} else if( parm == "lua-test" ) {
			//lua_test(); // temporary function
			return( -1 );
		} else if( parm == "log-xml" ) { SETOPTION("options/log/xml", 1);
		} else if( parm == "log-out" ) { SETOPTION("options/log/out", 1);
		} else if( parm == "nolog-xml" ) { SETOPTION("options/log/xml", 0);
		} else if( parm == "nolog-out" ) { SETOPTION("options/log/out", 0);
		}
	}
	
	return( 0 );
}
