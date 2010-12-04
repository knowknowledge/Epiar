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
#include "Engine/simulation.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
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
// main font used throughout the game
Font *SansSerif = NULL, *BitType = NULL, *Serif = NULL, *Mono = NULL;

Image* newSplashScreen(){
	string splashScreens[] = {
		"Resources/Art/menu1.png",
		"Resources/Art/menu2.png",
		"Resources/Art/menu3.png",
		"Resources/Art/menu4.png",
		"Resources/Art/menu5.png",
	};
	srand ( time(NULL) );
	Video::Erase();
	return Image::Get(splashScreens[rand()% (sizeof(splashScreens)/sizeof(splashScreens[0])) ]);
}

typedef enum {
	DoNothing,
	Play,
	Options,
	Editor,
	Quit,
} menuOption;

menuOption clicked = DoNothing;

// Currently Static functions are the only way I could think of to have C only 
void clickPlay() { clicked = Play; }
void clickOptions() { clicked = Options; }
void clickEditor() { clicked = Editor; }
void clickQuit() { clicked = Quit; }

void createMenu() {
	int x = OPTION( int, "options/video/w" ) - 200;
	// Create UI
	UI::Add( new Button(x, 200, 100, 30, "Play",    clickPlay    ) );
	UI::Add( new Button(x, 300, 100, 30, "Options", clickOptions ) );
	UI::Add( new Button(x, 400, 100, 30, "Editor",  clickEditor  ) );
	UI::Add( new Button(x, 500, 100, 30, "Quit",    clickQuit    ) );
}

void mainmenu() {
	bool quitSignal = false;
	bool screenNeedsReset = false;
	Input inputs;
	list<InputEvent> events;

	Image* splash = newSplashScreen();
	createMenu();

	string simName = "Resources/Simulation/default";
	Simulation debug;

	// Input Loop
	do {
		if (screenNeedsReset) {
			UI::Close();
			createMenu();
			splash = newSplashScreen();
			screenNeedsReset = false;
		}

		// Forget about the last click
		clicked = DoNothing;

		// Collect user input events
		events = inputs.Update( quitSignal );
		UI::HandleInput( &events );

		// Draw Things
		Video::Erase();
		splash->DrawStretch(0,0,OPTION( int, "options/video/w" ),OPTION( int, "options/video/h"));
		// Draw the "logo"
		Image::Get("Resources/Art/logo.png")->Draw(Video::GetWidth() - 240, Video::GetHeight() - 120 );
		UI::Draw();
		Video::Update();

		switch(clicked){
			case Play:
				UI::Close();
				screenNeedsReset = true;

				Video::Erase();
				splash = newSplashScreen();
				splash->DrawStretch(0,0,OPTION( int, "options/video/w" ),OPTION( int, "options/video/h"));
				Image::Get("Resources/Art/logo.png")->Draw(Video::GetWidth() - 240, Video::GetHeight() - 120 );
				Video::Update();

				if( false == debug.isLoaded() )
				{
					if(	!debug.Load( simName ) )
					{
						LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
						break;
					}
					debug.SetupToRun();
				}

				// Only attempt to Run if the Simulation has loaded
				assert( debug.isLoaded() );

				debug.Run();

				break;

			case Options:
				UI::Close();
				Lua::Call("options");
				break;

			case Editor:
				UI::Close();
				screenNeedsReset = true;

				if( false == debug.isLoaded() )
				{
					if(	!debug.Load( simName ) )
					{
						LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
						break;
					}
				}

				// Only attempt to Edit if the Simulation has loaded
				assert( debug.isLoaded() );

				debug.Edit();

				break;

			case Quit:
				quitSignal = true;
				break;

			default:
				break;
		}
		

		// Wait until the next click
		Timer::Delay(50);
	}while(!quitSignal);

}

/**Main runtime.
 * \return 0 always
 * \details
 * This function does the following:
 *  - Load options
 *  - Load fonts
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
	#if defined(_MSC_VER) && defined(DEBUG)
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
	#endif//_MSC_VER
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
	argparser.SetOpt(LONGOPT,"ships-worldmap",	"Displays ships on the world map.");
	argparser.SetOpt(VALUEOPT,"log-lvl",		"Logging level.(None,Fatal,Critical,Error,"
			"\n\t\t\t\tWarn,Alert,Notice,Info,Verbose[1-3],Debug[1-4])");
	argparser.SetOpt(VALUEOPT,"log-fun",		"Filter log messages by function name.");
	argparser.SetOpt(VALUEOPT,"log-msg",		"Filter log messages by string content.");

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
	optionsfile = new XMLFile();
	if( !optionsfile->Open("Resources/Definitions/options.xml") )
	{
		printf("Failed to find Options file at 'Resources/Definitions/options.xml'. Aborting Epiar.");
		return -1;
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
	if(argparser.HaveOpt("ships-worldmap"))
	   SETOPTION("options/development/ships-worldmap",1);
	if ( argparser.HaveOpt("log-xml") ) 	{ SETOPTION("options/log/xml", 1);}
	else if ( argparser.HaveOpt("nolog-xml") ) 	{ SETOPTION("options/log/xml", 0);}
	if ( argparser.HaveOpt("log-out") ) 	{ SETOPTION("options/log/out", 1);}
	else if ( argparser.HaveOpt("nolog-out") ) 	{ SETOPTION("options/log/out", 0);}

	string funfilt = argparser.HaveValue("log-fun");
	string msgfilt = argparser.HaveValue("log-msg");
	string loglvl = argparser.HaveValue("log-lvl");

	if("" != funfilt) Log::Instance().SetFunFilter(funfilt);
	if("" != msgfilt) Log::Instance().SetMsgFilter(msgfilt);
	if("" != loglvl)  Log::Instance().SetLevel( loglvl );

	// Print unused options.
	list<string> unused = argparser.GetUnused();
	list<string>::iterator it;
	for ( it = unused.begin() ; it != unused.end(); it++ )
		cout << "\tUnknown options:\t" << (*it) << endl;
	if ( !unused.empty() ) {
		argparser.PrintUsage();

		// free the configuration file data
		delete optionsfile;

		return -1;
	}

	LogMsg(INFO, "Epiar %s starting up.", EPIAR_VERSION_FULL );

#ifdef COMP_MSVC
	LogMsg(INFO, "Compiled with MSVC vers: _MSC_VER" );
#endif // COMP_MSVC
#ifdef COMP_GCC
	LogMsg(INFO, "Compiled with GCC vers: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif // COMP_GCC

	LogMsg(INFO,"Executable Path: %s", argv[0]);

	Audio::Instance().Initialize();
	Audio::Instance().SetMusicVol ( OPTION(float,"options/sound/musicvolume") );
	Audio::Instance().SetSoundVol ( OPTION(float,"options/sound/soundvolume") );

	LogMsg(INFO,"Using Font Engine: FreeType");

	SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
	BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
	Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
	Mono            = new Font( "Resources/Fonts/FreeMono.ttf" );

	Timer::Initialize();
	Video::Initialize();

	mainmenu();

	Video::Shutdown();
	Audio::Instance().Shutdown();

	LogMsg(INFO, "Epiar shutting down." );

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
	Log::Instance().Close();

	return( 0 );
}

