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
#include "ArgParser.h"
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

	int argpresult = parseArgs( argc, argv );
	if( argpresult == 0 ) {
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

/**\brief Parse command line switches.
 * \return -1 if a switch indicates the game should not be run, e.g. --help
 * \details
 * Cmd line args override settings in options.xml (found in data.tgz) 
 * for just this run of the program.
 */
int parseArgs( int argc, char **argv ) {
	ArgParser argparser(argc,argv);
	argparser.SetOpt("h",				"Display help screen");
	argparser.SetOpt("help",			"Display help screen");
	argparser.SetOpt("version",			"Display program version");
	argparser.SetOpt("ui-demo",			"Show a UI demo");
	argparser.SetOpt("graphics-demo",	"Demo the graphics");
	argparser.SetOpt("lua-test",		"Test Lua functionality");
	argparser.SetOpt("editor-mode",		"Puts you in edit mode");
	argparser.SetOpt("no-audio",		"Disables audio");
	argparser.SetOpt("nolog-xml",		"(Default) Disable logging messages to xml files.");
	argparser.SetOpt("log-xml",			"Log messages to xml files.");
	argparser.SetOpt("log-out",			"(Default) Log messages to console.");
	argparser.SetOpt("nolog-out",		"Disable logging messages to console.");

	// These are immediate options (I.E. they stop the argument processing immediately)
	if ( argparser.HaveOpt("h") || argparser.HaveOpt("help") ){
		argparser.PrintUsage();
		return -1;
	}
	if ( argparser.HaveOpt("version") ){
		printf("\nEpiar version %s", EPIAR_VERSION_FULL );
		printf("\n");
		return( -1 ); // indicates we should quit immediately
	}
	if ( argparser.HaveOpt("ui-demo") ) {
			ui_demo( true ); // temporary function
			return( 0 );
	}
	if ( argparser.HaveOpt("graphics-demo") ) {
			//graphics_demo(); // temporary function
			return( -1 );
	}
	if ( argparser.HaveOpt("lua-test") ) {
			//lua_test(); // temporary function
			return( -1 );
	}
	// Following are cumulative options (I.E. you can have multiple of them)
	if ( argparser.HaveOpt("editor-mode") ){
			SETOPTION("options/development/editor-mode",1);
	}
	if ( argparser.HaveOpt("no-audio") ) {
			cout<<"turning off sound"<<endl;
			SETOPTION("options/sound/background",0);
			SETOPTION("options/sound/weapons",0);
			SETOPTION("options/sound/engines",0);
			SETOPTION("options/sound/explosions",0);
			SETOPTION("options/sound/buttons",0);
	}
	if ( argparser.HaveOpt("log-xml") ) 	{ SETOPTION("options/log/xml", 1);}
	else if ( argparser.HaveOpt("nolog-xml") ) 	{ SETOPTION("options/log/xml", 0);}
	if ( argparser.HaveOpt("log-out") ) 	{ SETOPTION("options/log/out", 1);}
	else if ( argparser.HaveOpt("nolog-out") ) 	{ SETOPTION("options/log/out", 0);}

	// Print unused options.
	list<string> unused = argparser.GetUnused();
	list<string>::iterator it;
	for ( it=unused.begin() ; it != unused.end(); it++ )
		cout << "\tUnknown options:\t" << (*it)<<endl;

	return( 0 );
}
