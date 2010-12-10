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
XMLFile *skinfile = NULL;
// main font used throughout the game
Font *SansSerif = NULL, *BitType = NULL, *Serif = NULL, *Mono = NULL;
ArgParser *argparser;

void Main_OS                ( int argc, char **argv ); ///< Run OS Specific setup code
void Main_Init_Singletons   ( int argc, char **argv ); ///< Initialize global Singletons
void Main_Parse_Args        ( ); ///< Parse Command Line Arguments
void Main_Log_Environment   ( void ); ///< Record Environment variables
void Main_Menu              ( void ); ///< Run the Main Menu
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
	Main_Init_Singletons( argc, argv );

	// Respond to Command Line Arguments
	Main_Parse_Args();
	Main_Log_Environment();

	// THE GAME
	Main_Menu();

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

}

/** \details
 *  This will initialize the singletons for this Epiar instance:
 *   - OPTIONS
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
void Main_Init_Singletons( int argc, char **argv ) {
	optionsfile = new XMLFile();
	if( !optionsfile->Open("Resources/Definitions/options.xml") )
	{
		fprintf(stderr, "Failed to find Options file at 'Resources/Definitions/options.xml'. Aborting Epiar.");
		exit(-1);
	}

	skinfile = new XMLFile();
	if( !skinfile->Open("Resources/Definitions/skin.xml") )
	{
		fprintf(stderr, "Failed to find Skin file at 'Resources/Definitions/skin.xml'. Aborting Epiar.");
		exit(-1);
	}

	Audio::Instance().Initialize();
	Audio::Instance().SetMusicVol ( OPTION(float,"options/sound/musicvolume") );
	Audio::Instance().SetSoundVol ( OPTION(float,"options/sound/soundvolume") );

	SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
	BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
	Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
	Mono            = new Font( "Resources/Fonts/FreeMono.ttf" );

	Timer::Initialize();
	Video::Initialize();

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
	argparser->SetOpt(LONGOPT,  "ui-demo",       "Runs the UI demo.");

#ifdef EPIAR_COMPILE_TESTS
	argparser->SetOpt(VALUEOPT, "run-test",      "Run specified test");
#endif // EPIAR_COMPILE_TESTS

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
void Main_Parse_Args( ) {

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
			cout<<"turning off sound"<<endl;
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

	argparser->HaveLong("ui-demo");

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

typedef enum {
	Menu_DoNothing  = 0x0,
	Menu_Play       = 0x1,
	Menu_Options    = 0x2,
	Menu_NewSim     = 0x4,
	Menu_Editor     = 0x8,
	Menu_Quit       = 0x10,
	Menu_ALL        = 0xFF,
} menuOption;

menuOption clicked = Menu_DoNothing;

// Currently Static functions are the only way I could think of to have C only 
void clickPlay() { clicked = Menu_Play; } ///< Signal the Main Menu to Play Simulation.
void clickOptions() { clicked = Menu_Options; } ///< Signal the Main Menu to Open the Options.
void clickNewSim() { clicked = Menu_NewSim; } ///< Signal the Main Menu to Run the Editor.
void clickEditor() { clicked = Menu_Editor; } ///< Signal the Main Menu to Run the Editor.
void clickQuit() { clicked = Menu_Quit; } ///< Signal the Main Menu to Quit.

/** \brief Create a simple UI test.
 *  \todo Move this to the Unit Tests
 */
void ui_test() {

	// Example of Nestable UI Creation
	UI::Add(
		(new Tabs( 50,50,500,500, "TEST TABS"))
		->AddChild( (new Tab( "Nested Frames" ))
			->AddChild( (new Button(10, 10, 100, 30, "Quit 1",    clickQuit    )) )
			->AddChild( (new Frame( 50,50,400,400 ))
				->AddChild( (new Button(10, 10, 100, 30, "Quit 2",    clickQuit    )) )
				->AddChild( (new Frame( 50,50,300,300 ))
					->AddChild( (new Button(10, 10, 100, 30, "Quit 3",    clickQuit    )) )
					->AddChild( (new Frame( 50,50,200,200 ))
						->AddChild( (new Button(10, 10, 100, 30, "Quit 4",    clickQuit    )) )
					)
				)
			)
		)
		->AddChild( (new Tab( "Scoll to Buttons" ))
			->AddChild( (new Button(10,   0, 100, 30, "Quit 1",    clickQuit    )) )
			->AddChild( (new Button(10, 100, 100, 30, "Quit 1",    clickQuit    )) )
			->AddChild( (new Button(10, 200, 100, 30, "Quit 2",    clickQuit    )) )
			->AddChild( (new Button(10, 300, 100, 30, "Quit 3",    clickQuit    )) )
			->AddChild( (new Button(10, 400, 100, 30, "Quit 4",    clickQuit    )) )
			->AddChild( (new Button(10, 500, 100, 30, "Quit 5",    clickQuit    )) )
			->AddChild( (new Button(10, 600, 100, 30, "Quit 6",    clickQuit    )) )
			->AddChild( (new Frame( 250,50,300,300 ))
				->AddChild( (new Button(10,   0, 100, 30, "Quit 1",    clickQuit    )) )
				->AddChild( (new Button(10, 100, 100, 30, "Quit 1",    clickQuit    )) )
				->AddChild( (new Button(10, 200, 100, 30, "Quit 2",    clickQuit    )) )
				->AddChild( (new Button(10, 300, 100, 30, "Quit 3",    clickQuit    )) )
				->AddChild( (new Button(10, 400, 100, 30, "Quit 4",    clickQuit    )) )
				->AddChild( (new Button(10, 500, 100, 30, "Quit 5",    clickQuit    )) )
				->AddChild( (new Button(10, 600, 100, 30, "Quit 6",    clickQuit    )) )
			)
		)
	);

	assert( NULL != UI::Search("/[0]/") );
	assert( NULL != UI::Search("/Tabs/") );
	assert( NULL != UI::Search("/(100,100)/") );
	assert( NULL != UI::Search("/'TEST TABS'/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'TEST TABS'/[0]/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab[1]/") );
	assert( NULL != UI::Search("/'TEST TABS'/[0]/Frame/") );
	assert( NULL != UI::Search("/'TEST TABS'/[0]/(60,60)/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab/Frame/Button/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab/Frame/Frame/Button/") );
	assert( NULL != UI::Search("/'TEST TABS'/Tab/Frame/Frame/Frame/Button/") );
}

/** Epiar's Main Menu
 *
 *  This runs a while(1) loop collecting user input and drawing the screen.
 *  While similar to the Run Loop in the Simulation, this should be simpler
 *  since there is no HUD, Consol or Sprites.
 *
 */
void Main_Menu( void ) {
	bool quitSignal = false;
	bool screenNeedsReset = true;
	Input inputs;
	list<InputEvent> events;
	menuOption availableMenus = (menuOption)(Menu_Play | Menu_NewSim | Menu_Editor | Menu_Quit);
	int screenNum;
	int button_x = OPTION( int, "options/video/w" ) - 200;

	string splashScreens[] = {
		"Resources/Art/menu1.png",
		"Resources/Art/menu2.png",
		"Resources/Art/menu3.png",
		"Resources/Art/menu4.png",
		"Resources/Art/menu5.png",
	};

	screenNum = rand() % (sizeof(splashScreens) / sizeof(splashScreens[0]));
	Image* splash = Image::Get( splashScreens[screenNum] );

	string simName = "Resources/Simulation/default";
	Simulation debug;

	// Input Loop
	do {
		if (screenNeedsReset) {
			UI::Close();

			// Create UI
			if( availableMenus & Menu_Play )
				UI::Add( new Button(button_x, 200, 100, 30, "Play",    clickPlay    ) );
			if( availableMenus & Menu_Editor ) {
				UI::Add( new Button(button_x, 300, 100, 30, "Editor",  clickEditor  ) );
				if( false == debug.isLoaded() )
				{
					UI::Add( new Textbox(button_x, 330, 100, 1, "default", "Old Name") );
				}
			}
			if( availableMenus & Menu_NewSim ) {
				UI::Add( new Button(button_x, 380, 100, 30, "New Simulation", clickNewSim ) );
				UI::Add( new Textbox(button_x, 410, 100, 1, "", "New Name") );
			}
			if( availableMenus & Menu_Options )
				UI::Add( new Button(button_x, 460, 100, 30, "Options", clickOptions ) );
			if( availableMenus & Menu_Quit )
				UI::Add( new Button(button_x, 500, 100, 30, "Quit",    clickQuit    ) );

			if( argparser->HaveLong("ui-demo") ) {
				ui_test();
			}

			screenNeedsReset = false;
		}

		// Forget about the last click
		clicked = Menu_DoNothing;

		// Collect user input events
		events = inputs.Update();
		UI::HandleInput( events );

		// Draw Things
		Video::Erase();
		splash->DrawStretch(0,0,OPTION( int, "options/video/w" ),OPTION( int, "options/video/h"));
		// Draw the "logo"
		Image::Get("Resources/Art/logo.png")->Draw(Video::GetWidth() - 240, Video::GetHeight() - 120 );
		UI::Draw();
		Video::Update();


		switch(clicked){
			case Menu_Play:
				UI::Close();
				screenNeedsReset = true;
				availableMenus = (menuOption)(availableMenus & ~Menu_Editor);
				availableMenus = (menuOption)(availableMenus | Menu_Options);

				Video::Erase();
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

			case Menu_Options:
				assert( Lua::CurrentState() != NULL );
				Lua::Call("options");
				break;

			case Menu_NewSim:
			case Menu_Editor:
				screenNeedsReset = true;
				availableMenus = (menuOption)(availableMenus & ~Menu_Play);
				availableMenus = (menuOption)(availableMenus & ~Menu_NewSim);
				availableMenus = (menuOption)(availableMenus | Menu_Options);

				if( false == debug.isLoaded() )
				{
					if( clicked == Menu_Editor ) {
						assert( NULL != UI::Search("/Textbox'Old Name'/") );
						simName = "Resources/Simulation/" + ((Textbox*)UI::Search("/Textbox'Old Name'/"))->GetText();
						if( !debug.Load( simName ) )
						{
							LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
							break;
						}
					} else {
						assert( NULL != UI::Search("/Textbox'New Name'/") );
						simName = "Resources/Simulation/" + ((Textbox*)UI::Search("/Textbox'New Name'/"))->GetText();
						debug.New( simName );
					}

					debug.SetupToEdit();
				}

				UI::Close();

				debug.Edit();
				break;

			case Menu_Quit:
				quitSignal = true;
				break;

			default:
				break;
		}

		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYUP, SDLK_ESCAPE ) ) ) {
			quitSignal = true;
		}

		// Wait until the next click
		Timer::Delay(50);
	}while(!quitSignal);

	LogMsg(INFO, "Epiar shutting down." );
}

