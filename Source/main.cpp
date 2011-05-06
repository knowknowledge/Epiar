/**\file		main.cpp
 * \author		Chris Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created:	Sunday, June 4, 2006
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
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "menu.h"
#include "UI/ui.h"
#include "Utilities/argparser.h"
#include "Utilities/filesystem.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"
#include "Utilities/xml.h"
#include "Utilities/timer.h"

#ifdef EPIAR_COMPILE_TESTS
#include "Tests/tests.h"
#endif // EPIAR_COMPILE_TESTS

// main configuration file, used through the tree (extern in common.h)
XMLFile *optionsfile = NULL;
XMLFile *skinfile = NULL;
// main font used throughout the game
Font *SansSerif = NULL, *BitType = NULL, *Serif = NULL, *Mono = NULL;
ArgParser *argparser = NULL;

void Main_OS                ( int argc, char **argv ); ///< Run OS Specific setup code
void Main_Load_Settings     (); ///< Load the settings files
void Main_Init_Singletons   (); ///< Initialize global Singletons
void Main_Parse_Args        ( int argc, char **argv ); ///< Parse Command Line Arguments
void Main_Log_Environment   ( void ); ///< Record Environment variables
void Main_Close_Singletons  ( void ); ///< Close global Singletons

/**Main
 * \return 0 always
 * \details
 * This function does the following:
 *  - Load options
 *  - Load fonts
 *  - Runs the Simulation routine
 *  - Calls any cleanup code
 */
int main( int argc, char **argv ) {
	// Basic Setup
	Main_OS( argc, argv );
	Main_Load_Settings();

	// Respond to Command Line Arguments
	Main_Parse_Args( argc, argv );
	Main_Log_Environment();

	// THE GAME
	Main_Init_Singletons();
	Main_Menu();

	LogMsg(INFO, "Epiar shutting down." );

	// Close everything and Quit
	Main_Close_Singletons();

	return( 0 );
}

/** \details
 *  The OS Specific code here sets up OS specific environment variables and
 *  paths that are vital for normal operation.
 * 	
 *  Since nothing has is loaded or initialized before this code, do not use any
 *  code that is epiar specific (OPTIONS, Log, Lua, etc).
 *
 *  \param[in] argc standard c argc
 *  \param[in] argv standard c argv
 */
void Main_OS( int argc, char **argv ) {

#ifdef __APPLE__
	string path = argv[0];
	if( path.find("MacOS/Epiar") ){ // If this is being run from inside a Bundle
		// Chdir to the Bundle Contents
		string ContentsPath = path.substr(0, path.find("MacOS/Epiar") );
		chdir(ContentsPath.c_str());
	}
#endif

#ifdef _WIN32
	// This attaches a console to the parent process if it has a console
	if(AttachConsole(ATTACH_PARENT_PROCESS)){
		freopen("CONOUT$","wb",stdout);  // reopen stout handle as console window output
		freopen("CONOUT$","wb",stderr); // reopen stderr handle as console window output
	}
	#if defined(_MSC_VER) && defined(DEBUG)
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
	#endif//_MSC_VER
#endif //_WIN32

	Filesystem::Init( argv[0] );
}

/** \brief Load the options files
 *  \details This will load the options.xml and skin.xml files.
 *           The options.xml file defines miscellaneous flags and numerical settings.
 *           The skin.xml file defines the non-png aspects of the User Interface.
 *  \todo If these files do not exist, reasonable defaults should be loaded instead.
 */
void Main_Load_Settings() {
	optionsfile = new XMLFile();
	if( !optionsfile->Open("Resources/Definitions/options.xml") )
	{
		// Create the default Options file
		optionsfile->New("Resources/Definitions/options.xml", "options");

		// Logging
		SETOPTION( "options/log/xml", 0 );
		SETOPTION( "options/log/out", 1 );
		SETOPTION( "options/log/alert", 0 );
		SETOPTION( "options/log/ui", 0 );
		SETOPTION( "options/log/sprites", 0 );

		// Video
		SETOPTION( "options/video/w", 1024 );
		SETOPTION( "options/video/h", 768 );
		SETOPTION( "options/video/bpp", 32 );
		SETOPTION( "options/video/fullscreen", 0 );
		SETOPTION( "options/video/fps", 60 );

		// Sound
		SETOPTION( "options/sound/musicvolume", 0.5f );
		SETOPTION( "options/sound/soundvolume", 0.5f );
		SETOPTION( "options/sound/background", 1 );
		SETOPTION( "options/sound/weapons", 1 );
		SETOPTION( "options/sound/engines", 1 );
		SETOPTION( "options/sound/explosions", 1 );
		SETOPTION( "options/sound/buttons", 1 );

		// Simultaion
		SETOPTION( "options/simulation/starfield-density", 750 );
		SETOPTION( "options/simulation/automatic-load", 0 );
		SETOPTION( "options/simulation/random-universe", 0 );
		SETOPTION( "options/simulation/random-seed", 0 );

		// Timing
		SETOPTION( "options/timing/screen-swap", 250 );
		SETOPTION( "options/timing/mouse-fade", 500 );
		SETOPTION( "options/timing/target-zoom", 500 );
		SETOPTION( "options/timing/alert-drop", 3500 );
		SETOPTION( "options/timing/alert-fade", 2500 );

		// Development
		SETOPTION( "options/development/ships-worldmap", 0 );
		SETOPTION( "options/development/debug-ai", 0 );
		SETOPTION( "options/development/debug-ui", 0 );

		optionsfile->Save();
	}

	skinfile = new XMLFile();
	if( !skinfile->Open("Resources/Skin/skin.xml") )
	{
		// Create the default Skin file
		skinfile->New("Resources/Skin/skin.xml", "Skin");

		// UI - Default
		skinfile->Set( "Skin/UI/Default/Font", "Resources/Fonts/FreeSans.ttf");
		skinfile->Set( "Skin/UI/Default/Color", "0xFFFFFF");
		skinfile->Set( "Skin/UI/Default/Size", 12);

		// UI - Textbox
		skinfile->Set( "Skin/UI/Textbox/Font", "Resources/Fonts/FreeMono.ttf");
		skinfile->Set( "Skin/UI/Textbox/Color/Foreground", "0xCCCCCC");
		skinfile->Set( "Skin/UI/Textbox/Color/Background", "0x666666");
		skinfile->Set( "Skin/UI/Textbox/Color/Edge", "0x262626");

		// UI - Tab
		skinfile->Set( "Skin/UI/Tab/Color/Active", "0x393939");
		skinfile->Set( "Skin/UI/Tab/Color/Inactive", "0x262626");

		// HUD - Alert
		skinfile->Set( "Skin/HUD/Alert/Font", "Resources/Fonts/FreeSans.ttf");
		skinfile->Set( "Skin/HUD/Alert/Color", "0xFFFFFF");
		skinfile->Set( "Skin/HUD/Alert/Size", 12);

		skinfile->Save();
	}
}

/** \details
 *  This will initialize the singletons for this Epiar instance:
 *   - OPTIONS (optionsfile)
 *   - SKIN (skinfile)
 *   - Audio
 *   - Fonts
 *   - Timer
 *   - Video
 *   - ArgParser
 *
 *  Singletons should be kept to a minimum whenever possible.
 *
 *  \param[in] argc standard c argc
 *  \param[in] argv standard c argv
 *
 *  \TODO Remove Fonts with a style.
 *  \TODO Add Logger
 *
 *  \warn This may exit early on Errors
 */
void Main_Init_Singletons() {
	Audio::Instance().Initialize();
	Audio::Instance().SetMusicVol ( OPTION(float,"options/sound/musicvolume") );
	Audio::Instance().SetSoundVol ( OPTION(float,"options/sound/soundvolume") );

	SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
	BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
	Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
	Mono            = new Font( "Resources/Fonts/FreeMono.ttf" );

	Timer::Initialize();
	Video::Initialize();
	UI::Initialize("Main Screen");

	srand ( time(NULL) );
}

/** \details
 *  This cleanup is done for completeness, but the normal runtime should do all
 *  of this automatically.
 *  \warn Do not run any non-trivial code after calling this.
 */
void Main_Close_Singletons( void ) {
	Video::Shutdown();
	Audio::Instance().Shutdown();

	// free the main font files
	delete SansSerif;
	delete BitType;
	delete Serif;
	delete Mono;

	// free the configuration file data
	delete optionsfile;
	delete skinfile;

	Log::Instance().Close();
}

/** \details
 *  This processes all of the command line arguments using the ArgParser. As a
 *  general rule there are two kinds of Arguments:
 *   - Help Arguments that print the version, usage, etc.
 *   - OPTION Arguments that override a normal OPTION value.
 *   - Test Arguments that run Epiar Unit tests and then exit.
 *  
 *  \warn This may exit early.
 */
void Main_Parse_Args( int argc, char **argv ) {
	// Parse command line options first.
	argparser = new ArgParser(argc, argv);

	argparser->SetOpt(SHORTOPT, "h",             "Display help screen");
	argparser->SetOpt(LONGOPT, "help",           "Display help screen");
	argparser->SetOpt(SHORTOPT, "v",             "Display program version");
	argparser->SetOpt(LONGOPT, "version",        "Display program version");
	argparser->SetOpt(LONGOPT, "no-audio",       "Disables audio");
	argparser->SetOpt(LONGOPT, "nolog-xml",      "(Default) Disable logging messages to xml files.");
	argparser->SetOpt(LONGOPT, "log-xml",        "Log messages to xml files.");
	argparser->SetOpt(LONGOPT, "log-out",        "(Default) Log messages to console.");
	argparser->SetOpt(LONGOPT, "nolog-out",      "Disable logging messages to console.");
	argparser->SetOpt(LONGOPT, "ships-worldmap", "Displays ships on the world map.");
	argparser->SetOpt(VALUEOPT, "log-lvl",       "Logging level.(None,Fatal,Critical,Error,"
	                                             "\n\t\t\t\tWarn,Alert,Notice,Info,Verbose[1-3],Debug[1-4])");
	argparser->SetOpt(VALUEOPT, "log-fun",       "Filter log messages by function name.");
	argparser->SetOpt(VALUEOPT, "log-msg",       "Filter log messages by string content.");

#ifdef EPIAR_COMPILE_TESTS
	argparser->SetOpt(VALUEOPT, "run-test",      "Run specified test");
#endif // EPIAR_COMPILE_TESTS

	// These are immediate options (I.E. they stop the argument processing immediately)
	if ( argparser->HaveShort("h") || argparser->HaveLong("help") ){
		argparser->PrintUsage();
		exit( 0 );
	}

	if ( argparser->HaveShort("v") || argparser->HaveLong("version") ){
		printf("\nEpiar version %s\n", EPIAR_VERSION_FULL );
		exit( 0 );
	}

#ifdef EPIAR_COMPILE_TESTS
	string testname = argparser->HaveValue("run-test");
	if ( !(testname.empty()) ) {
		Test testInst(testname);
		exit( testInst.RunTest( argc, argv ) );
	}
#endif // EPIAR_COMPILE_TESTS

	// Override OPTION values.

	// Following are cumulative options (I.E. you can have multiple of them)
	if ( argparser->HaveOpt("no-audio") ) {
			SETOPTION("options/sound/background",0);
			SETOPTION("options/sound/weapons",0);
			SETOPTION("options/sound/engines",0);
			SETOPTION("options/sound/explosions",0);
			SETOPTION("options/sound/buttons",0);
	}
	if(argparser->HaveOpt("ships-worldmap"))
	   SETOPTION("options/development/ships-worldmap",1);
	if      ( argparser->HaveOpt("log-xml") ) 	{ SETOPTION("options/log/xml", 1);}
	else if ( argparser->HaveOpt("nolog-xml") ) 	{ SETOPTION("options/log/xml", 0);}
	if      ( argparser->HaveOpt("log-out") ) 	{ SETOPTION("options/log/out", 1);}
	else if ( argparser->HaveOpt("nolog-out") ) 	{ SETOPTION("options/log/out", 0);}

	string funfilt = argparser->HaveValue("log-fun");
	string msgfilt = argparser->HaveValue("log-msg");
	string loglvl = argparser->HaveValue("log-lvl");

	if("" != funfilt) Log::Instance().SetFunFilter(funfilt);
	if("" != msgfilt) Log::Instance().SetMsgFilter(msgfilt);
	if("" != loglvl)  Log::Instance().SetLevel( loglvl );

	// Print unused options.
	list<string> unused = argparser->GetUnused();
	list<string>::iterator it;
	for ( it = unused.begin() ; it != unused.end(); it++ )
		cout << "\tUnknown options:\t" << (*it) << endl;
	if ( !unused.empty() ) {
		argparser->PrintUsage();

		// free the configuration file data
		delete optionsfile;

		exit( 1 );
	}
}

/** \details
 *  This records basic Epiar information about the current Environment.
 */
void Main_Log_Environment( void ) {
	LogMsg(INFO, "Epiar Version %s", EPIAR_VERSION_FULL );

#ifdef COMP_MSVC
	LogMsg(INFO, "Compiled with MSVC vers: _MSC_VER" );
#endif // COMP_MSVC

#ifdef COMP_GCC
	LogMsg(INFO, "Compiled with GCC vers: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif // COMP_GCC

	LogMsg(INFO,"Executable Path: %s", argparser->GetPath().c_str() );
}

