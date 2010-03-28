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
#include "Utilities/argparser.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

#ifdef EPIAR_COMPILE_TESTS
#include "Tests/tests.h"
#endif // EPIAR_COMPILE_TESTS


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
#ifdef _WIN32
	// This attaches a console to the parent process if it has a console
	if(AttachConsole(ATTACH_PARENT_PROCESS)){
		freopen("CONOUT$","wb",stdout);  // reopen stout handle as console window output
		freopen("CONOUT$","wb",stderr); // reopen stderr handle as console window output
	}
#endif //_WIN32

	// Parse command line options first.
	ArgParser argparser(argc,argv);
	argparser.SetOpt(SHORTOPT,"h",				"Display help screen");
	argparser.SetOpt(LONGOPT,"help",			"Display help screen");
	argparser.SetOpt(SHORTOPT,"v",				"Display program version");
	argparser.SetOpt(LONGOPT,"version",			"Display program version");
	argparser.SetOpt(LONGOPT,"editor-mode",		"Puts you in edit mode");
	argparser.SetOpt(LONGOPT,"no-audio",		"Disables audio");
	argparser.SetOpt(LONGOPT,"nolog-xml",		"(Default) Disable logging messages to xml files.");
	argparser.SetOpt(LONGOPT,"log-xml",			"Log messages to xml files.");
	argparser.SetOpt(LONGOPT,"log-out",			"(Default) Log messages to console.");
	argparser.SetOpt(LONGOPT,"nolog-out",		"Disable logging messages to console.");
#ifdef EPIAR_COMPILE_TESTS
	argparser.SetOpt(VALUEOPT,"run-test",		"Run specified test");
#endif // EPIAR_COMPILE_TESTS

	// These are immediate options (I.E. they stop the argument processing immediately)
	if ( argparser.HaveShort("h") || argparser.HaveLong("help") ){
		argparser.PrintUsage();
		return 0;
	}
	if ( argparser.HaveShort("v") || argparser.HaveLong("version") ){
		printf("\nEpiar version %s", EPIAR_VERSION_FULL );
		printf("\n");
		return 0;
	}

#ifdef EPIAR_COMPILE_TESTS
	string testname = argparser.HaveValue("run-test");
	if ( !(testname.empty()) ) {
		Test testInst(testname);
		return testInst.RunTest( argc, argv );
	}
#endif // EPIAR_COMPILE_TESTS

#ifdef USE_PHYSICSFS
	// Use ".dat" extension for data files
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

	// Unfortunately, we need to load the main configuration file (used throughout the tree)
	// before parsing more options
	optionsfile = new XMLFile( "Resources/Definitions/options.xml" );

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
	if ( !unused.empty() ){
		// free the configuration file data
		delete optionsfile;
		return -1;
	}

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

	Simulation debug( "Resources/Definitions/sim-debug.xml" );
	debug.Run();

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

