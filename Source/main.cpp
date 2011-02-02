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
void Main_Load_Settings     (); ///< Load the settings files
void Main_Init_Singletons   (); ///< Initialize global Singletons
void Main_Parse_Args        ( int argc, char **argv ); ///< Parse Command Line Arguments
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
	Main_Load_Settings();

	// Respond to Command Line Arguments
	Main_Parse_Args( argc, argv );
	Main_Log_Environment();

	// THE GAME
	Main_Init_Singletons();
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
	argparser->SetOpt(LONGOPT,  "ui-demo",       "Runs the UI demo.");

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
	Menu_DoNothing      = 1<<0,
	Menu_New            = 1<<1,
	Menu_Load           = 1<<2,
	Menu_Continue       = 1<<3,
	Menu_Options        = 1<<4,
	Menu_Editor         = 1<<5,
	Menu_Quit           = 1<<6,
	Menu_Confirm_New    = 1<<7,
	Menu_Confirm_Load   = 1<<8,
	Menu_ALL            = 0xFFFF,
} menuOption;

PlayerInfo* playerToLoad = NULL;
menuOption clicked = Menu_DoNothing;

// Currently Static functions are the only way I could think of to have C only 
void setMenuOption( void* value ) { clicked = *((menuOption*)value); }
void LoadPlayer( void* value ) {
	clicked = Menu_Confirm_Load;
	playerToLoad = (PlayerInfo*)value;
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
	menuOption availableMenus = (menuOption)(Menu_New | Menu_Load | Menu_Editor | Menu_Quit);
	int screenNum, numScreens;
	int button_x = OPTION( int, "options/video/w" ) - 200;

	// These are instances of the menuOptions so that they can be passed to the Buttons as values
	menuOption menu_New            = Menu_New;
	menuOption menu_Load           = Menu_Load;
	menuOption menu_Confirm_New    = Menu_Confirm_New;
	menuOption menu_Continue       = Menu_Continue;
	menuOption menu_Options        = Menu_Options;
	menuOption menu_Editor         = Menu_Editor;
	menuOption menu_Quit           = Menu_Quit;

	string splashScreens[] = {
		"Resources/Art/menu1.png",
		"Resources/Art/menu2.png",
		"Resources/Art/menu3.png",
		"Resources/Art/menu4.png",
		"Resources/Art/menu5.png",
	};
	numScreens = (sizeof(splashScreens) / sizeof(splashScreens[0]));
	
	screenNum = rand() % numScreens;
	Image* menuSplash = Image::Get( splashScreens[screenNum] );
	screenNum = (screenNum+1) % numScreens;
	Image* gameSplash = Image::Get( splashScreens[screenNum] );
	screenNum = (screenNum+1) % numScreens;
	Image* editSplash = Image::Get( splashScreens[screenNum] );

	string playerName;
	string simName = "Resources/Simulation/default";
	Simulation debug;

	Players *players = Players::Instance();
	players->Load( "Resources/Definitions/saved-games.xml", true, true);

	// Input Loop
	do {
		if (screenNeedsReset) {
			UI::Close();

			// Create UI
			if( availableMenus & Menu_New )
				UI::Add( new Button(button_x, 200, 100, 30, "New", setMenuOption, &menu_New) );
			if( (availableMenus & Menu_Load) && (players->Size() > 0) )
				UI::Add( new Button(button_x, 250, 100, 30, "Load", setMenuOption, &menu_Load) );
			if( availableMenus & Menu_Continue )
				UI::Add( new Button(button_x, 200, 100, 30, "Continue", setMenuOption, &menu_Continue) );
			if( availableMenus & Menu_Editor )
				UI::Add( new Button(button_x, 300, 100, 30, "Editor", setMenuOption, &menu_Editor) );
			if( availableMenus & Menu_Options )
				UI::Add( new Button(button_x, 400, 100, 30, "Options", setMenuOption, &menu_Options) );
			if( availableMenus & Menu_Quit )
				UI::Add( new Button(button_x, 500, 100, 30, "Quit", setMenuOption, &menu_Quit) );

			if( argparser->HaveLong("ui-demo") ) {
				UI_Test();
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
		menuSplash->DrawStretch(0,0,OPTION( int, "options/video/w" ),OPTION( int, "options/video/h"));
		// Draw the "logo"
		Image::Get("Resources/Art/logo.png")->Draw(Video::GetWidth() - 240, Video::GetHeight() - 120 );
		UI::Draw();
		Video::Update();

		switch(clicked){
			case Menu_New:
			{
				char seed[20];
				snprintf(seed, sizeof(seed), "%d", rand() );
				UI::Add(
					(new Window(200, 200, 250, 300, "Create New Player"))
					->AddChild( (new Label(30, 30, "Player Name:")) )
					->AddChild( (new Textbox(130, 30, 100, 1, "", "Player Name:")) )
					->AddChild( (new Frame( 30, 90, 200, 70 ))
						->AddChild( (new Checkbox(15, 15, 0, "Random Universe")) )
						->AddChild( (new Label(15, 30, "Seed:")) )
						->AddChild( (new Textbox(50, 30, 80, 1, seed, "Random Universe Seed")) )
					)
					->AddChild( (new Button(10, 250, 100, 30, "Create", setMenuOption, &menu_Confirm_New)) )
				);
				break;
			}

			case Menu_Load:
			{
				Window* win = new Window(250, 50, 500, 700, "Load A Player");
				UI::Add( win );
				// Create a new Frame for each Player
				int p = 0;
				list<string>::iterator iter;
				list<string> *names = players->GetNames();
				for( iter = names->begin(); iter != names->end(); ++iter, ++p ) {
					PlayerInfo *info = players->GetPlayerInfo( *iter );
					win->AddChild( (new Frame( 50, 150*p + 30, 400, 120 ))
						->AddChild( (new Picture(10, 10, 80, 80, info->avatar )) )
						->AddChild( (new Label(100, 30, "Player Name:" )) ) ->AddChild( (new Label(200, 30, info->GetName() )) )
						->AddChild( (new Label(100, 60, "Simulation:" )) ) ->AddChild( (new Label(200, 60, info->simulation )) )
						->AddChild( (new Button(280, 80, 100, 30, "Load", LoadPlayer, info )) )
					);
				}
				break;
			}

			case Menu_Confirm_New:
			case Menu_Confirm_Load:
			{
				screenNeedsReset = true;
				availableMenus = (menuOption)(availableMenus & ~Menu_New);
				availableMenus = (menuOption)(availableMenus & ~Menu_Load);
				availableMenus = (menuOption)(availableMenus & ~Menu_Editor);
				availableMenus = (menuOption)(availableMenus | Menu_Continue);
				availableMenus = (menuOption)(availableMenus | Menu_Options);
				
				// Gather Player Information
				if( Menu_Confirm_New == clicked )
				{
					int israndom = ((Checkbox*)UI::Search("/Window'Create New Player'/Frame/Checkbox'Random Universe'/"))->IsChecked();
					int seed = atoi( ((Textbox*)UI::Search("/Window'Create New Player'/Frame/Textbox'Random Universe Seed'/"))->GetText().c_str() );
					SETOPTION( "options/simulation/random-universe", israndom );
					SETOPTION( "options/simulation/random-seed", seed );
					playerName = ((Textbox*)UI::Search("/Window'Create New Player'/Textbox'Player Name:'/"))->GetText();
				}
				else if( Menu_Confirm_Load == clicked )
				{
					int israndom = (playerToLoad->simulation == "random") ? 1 : 0;
					SETOPTION( "options/simulation/random-universe", israndom );
					SETOPTION( "options/simulation/random-seed", playerToLoad->seed );
					playerName = playerToLoad->GetName();
				}
				
				// Load the Simulation
				if( !debug.Load( simName ) )
				{
					LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
					break;
				}
				debug.SetupToRun();

				UI::SwapScreens( "In Game", menuSplash, gameSplash );
				
				// Create or Load the Player
				if( Menu_Confirm_New == clicked ) {
					debug.CreateDefaultPlayer( playerName );
					Lua::Call("intro");
				} else if( Menu_Confirm_Load == clicked ) {
					debug.LoadPlayer( playerName );
				}
				
				// Run the Simulation
				debug.Run();
				UI::SwapScreens( "Main Screen", gameSplash, menuSplash );
				break;
			}

			case Menu_Continue:
			{
				// Only attempt to Run if the Simulation has loaded
				assert( debug.isLoaded() );
				UI::SwapScreens( "In Game", menuSplash, gameSplash );
				debug.Run();
				UI::SwapScreens( "Main Screen", gameSplash, menuSplash );
				break;
			}

			case Menu_Options:
			{
				assert( Lua::CurrentState() != NULL );
				Lua::Call("options");
				break;
			}

			case Menu_Editor:
			{
				screenNeedsReset = true;
				availableMenus = (menuOption)(availableMenus & ~Menu_New);
				availableMenus = (menuOption)(availableMenus & ~Menu_Load);
				availableMenus = (menuOption)(availableMenus | Menu_Options);
				
				if( false == debug.isLoaded() )
				{
					if( !debug.Load( simName ) )
					{
						LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
						break;
					}
					debug.SetupToEdit();
				}

				// Only attempt to Edit if the Simulation has loaded
				assert( debug.isLoaded() );
				
				UI::SwapScreens( "Editor", menuSplash, editSplash );
				debug.Edit();
				UI::SwapScreens( "Main Screen", editSplash, menuSplash );
				
				break;
			}

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
	} while(!quitSignal);

	LogMsg(INFO, "Epiar shutting down." );
}

