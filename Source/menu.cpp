/**\file		menu.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \brief		Runs the main menu
 * \details
 */

#include "includes.h"
#include "Engine/simulation.h"
#include "menu.h"
#include "UI/ui.h"
#include "Utilities/filesystem.h"
#include "Utilities/timer.h"

bool Menu::quitSignal = false;

Simulation Menu::simulation;
PlayerInfo* Menu::playerToLoad = NULL;

Image* Menu::menuSplash = NULL;
Image* Menu::gameSplash = NULL;
Image* Menu::editSplash = NULL;

Picture *Menu::play = NULL;
Picture *Menu::load = NULL;
Picture *Menu::edit = NULL;
Picture *Menu::options = NULL;
Picture *Menu::exit = NULL;
Picture *Menu::continueButton = NULL;

//if(OPTION(int, "options/sound/buttons")) Sound::Get( "Resources/Audio/Interface/28853__junggle__btn043.ogg" )->Play();

/**\class Menu
 *  \brief Epiar's Main Menu
 *
 * 	\details
 *  This runs a while(1) loop collecting user input and drawing the screen.
 *  While similar to the Run Loop in the Simulation, this should be simpler
 *  since there is no HUD, Console or Sprites.
 *
 *  The Main Menu will launch the Simulation with a new or loaded Player.
 *  It can also edit simulations and option.
 *
 *  The Main Menu can be skipped by enabling the "automatic-load" option.
 *
 */

/** The Main Loop for the Menu screen.
 * \note Press ESC or hit the 'Quit' button to quit.
 */
void Menu::Main_Menu( void )
{
	Input inputs;
	list<InputEvent> events;
    
    quitSignal = false;

	Players *players = Players::Instance();
	players->Load( "Resources/Definitions/saved-games.xml", true, true);

	if( OPTION(int,"options/simulation/automatic-load") )
	{
		AutoLoad();
		return;
	}

	SetupGUI();

	// Input Loop
	do {
		// Collect user input events
		events = inputs.Update();
		UI::HandleInput( events );

		// Draw Things
		Video::Erase();
		Video::PreDraw();
		UI::Draw();
		Video::PostDraw();
		Video::Update();

		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) ) {
			quitSignal = true;
		}

		// Wait until the next click
		Timer::Delay(75);
	} while(!quitSignal);
}

/** Load the most recent Player
 * \note When the user leaves the Simulation, the game will quit.
 */
void Menu::AutoLoad()
{
	LogMsg(INFO,"Attempting to automatically load a player.");
	PlayerInfo* info = Players::Instance()->LastPlayer();
	if( info != NULL )
	{
		LogMsg(INFO, "Automatically loading player.", info->GetName().c_str() );
		if( !simulation.Load( info->simulation ) )
		{
			LogMsg(ERR,"Failed to load the Simulation '%s' successfully", info->simulation.c_str() );
			return;
		}
		if( !simulation.SetupToRun() )
		{
			LogMsg(ERR,"Failed to setup the Simulation '%s' successfully.", info->simulation.c_str() );
			return;
		}
		simulation.LoadPlayer( info->GetName() );
		simulation.Run();
	}
}

/** Create the Basic Main Menu
 *  \details The Splash Screen is random.
 */
void Menu::SetupGUI()
{
	int button_x = Video::GetWidth() - 300;

	string splashScreens[] = {
		"Resources/Art/menu1.png",
		"Resources/Art/menu2.png",
		"Resources/Art/menu3.png",
		"Resources/Art/menu4.png",
		"Resources/Art/menu5.png",
	};

	int numScreens = (sizeof(splashScreens) / sizeof(splashScreens[0]));
	int screenNum = rand();
	menuSplash = Image::Get( splashScreens[(screenNum+0) % numScreens] );
	gameSplash = Image::Get( splashScreens[(screenNum+1) % numScreens] );
	editSplash = Image::Get( splashScreens[(screenNum+2) % numScreens] );

	// Add the splash screen
	UI::Add( new Picture( 0,0, Video::GetWidth(), Video::GetHeight(), menuSplash) );

	// Add the logo
	UI::Add( new Picture(Video::GetWidth() - 240, Video::GetHeight() - 120, Image::Get("Resources/Art/logo.png") ) );

	// New Button
	play = new Picture( button_x, 200, "Resources/Graphics/txt_new_game_inactive.png");
	play->RegisterAction( Action_MouseLUp, new VoidAction( Menu::CreateNewWindow ) );
	SetPictureHover( play, Image::Get( "Resources/Graphics/txt_new_game_active.png"),
	                       Image::Get( "Resources/Graphics/txt_new_game_inactive.png") );
	UI::Add( play );

	// Load Button
	if( (Players::Instance()->Size() > 0) )
	{
		load = new Picture(button_x, 250, "Resources/Graphics/txt_load_game_inactive.png");
		load->RegisterAction( Action_MouseLUp, new VoidAction( Menu::CreateLoadWindow ) );
		SetPictureHover( load, Image::Get( "Resources/Graphics/txt_load_game_active.png"),
		                       Image::Get( "Resources/Graphics/txt_load_game_inactive.png") );
		UI::Add( load );
	}

	// Editor Button
	edit = new Picture(button_x, 300, "Resources/Graphics/txt_editor_inactive.png");
	edit->RegisterAction( Action_MouseLUp, new VoidAction( Menu::CreateEditWindow ) );
	SetPictureHover( edit, Image::Get( "Resources/Graphics/txt_editor_active.png"),
	                       Image::Get( "Resources/Graphics/txt_editor_inactive.png") );
	UI::Add( edit );

	// Options Button
	options = new Picture(button_x, 400, "Resources/Graphics/txt_options_inactive.png");
	options->RegisterAction( Action_MouseLUp, new VoidAction( Dialogs::OptionsWindow ) );
	SetPictureHover( options, Image::Get( "Resources/Graphics/txt_options_active.png"),
	                          Image::Get( "Resources/Graphics/txt_options_inactive.png") );
	UI::Add( options );

	// Exit Button
	exit = new Picture(button_x, 500, "Resources/Graphics/txt_exit_inactive.png");
	exit->RegisterAction( Action_MouseLUp, new VoidAction( QuitMenu ) );
	SetPictureHover( exit, Image::Get( "Resources/Graphics/txt_exit_active.png"),
	                       Image::Get( "Resources/Graphics/txt_exit_inactive.png") );
	UI::Add( exit );
}

/** This Window is used to create new Players.
 */
void Menu::CreateNewWindow()
{
	if( UI::Search("/Window'New Game'/") != NULL ) return;

	Window* win = new Window(300, 150, 250, 370, "New Game");
	UI::Add( win );

	// Player Name
	win->AddChild( (new Label(30, 30, "Player Name:")) )
		->AddChild( (new Textbox(130, 30, 100, 1, "", "Player Name:")) );

	// Simulation Picker
	win->AddChild( (new Frame( 30, 90, 200, 150 ))
		->AddChild( (new Label(15, 15, "Simulation:")) )
		->AddChild( (new Dropdown( 80, 15, 100, 30 ))
			->AddOptions( Filesystem::Enumerate("Resources/Simulation/") ) )
		->AddChild( (new Checkbox(15, 60, 0, "Random Universe")) )
		->AddChild( (new Label(15, 80, "Seed:")) )
		->AddChild( (new Textbox(50, 80, 80, 1, "0", "Random Universe Seed")) )
		->AddChild( (new Button(50, 100, 80, 30, "Randomize", RandomizeSeed )) )
	);
	win->AddChild( (new Button( 10, 330, 100, 30, "Cancel", &UI::Close, win)) );
	win->AddChild( (new Button(140, 330, 100, 30, "Create", &CreateNewPlayer )) );
    win->AddCloseButton();
}

/** This Window shows a list of potential Players.
 */
void Menu::CreateLoadWindow()
{
	if( UI::Search("/Window'Load Game'/") != NULL ) return;

	list<string> *names = Players::Instance()->GetNames();

	Window* win = new Window(250, 50, 500, 70 + (names->size() * 150), "Load Game");
	UI::Add( win );

	// Create a new Frame for each Player
	int p = 0;
	list<string>::iterator iter;
	for( iter = names->begin(); iter != names->end(); ++iter, ++p ) {
		PlayerInfo *info = Players::Instance()->GetPlayerInfo( *iter );
		win->AddChild( (new Frame( 50, 155*p + 30, 400, 130 ))
			->AddChild( (new Picture(20, 20, 80, 80, info->avatar )) )
			->AddChild( (new Label(120, 20, "Player Name:" )) ) ->AddChild( (new Label(210, 20, info->GetName() )) )
			->AddChild( (new Label(120, 45, "Simulation:" )) ) ->AddChild( (new Label(210, 45, info->simulation )) )
			->AddChild( (new Button(280, 80, 100, 30, "Play", StartGame, info )) )
			->AddChild( (new Button(170, 80, 100, 30, "Erase", ErasePlayer, info ) ) )
		);
	}
	win->AddChild( (new Button( 200, 155*names->size() + 20, 100, 30, "Cancel", &UI::Close, win)) );
	win->AddCloseButton();
	return;
}

/** Start a Game Simulation
 *  \details
 *  The PlayerInfo describes which Simulation to load and which Player to load.
 *  If the Player doesn't already exist, a default player is created by Simulation.
 */
void Menu::StartGame( void *info )
{
    Players *players = Players::Instance();

    playerToLoad = (PlayerInfo*)info;

	UI::Close( play ); // Play
	UI::Close( load ); // Load
	UI::Close( edit ); // Edit
	play = NULL;
	load = NULL;
	edit = NULL;

	// Gather Player Information
    string simName = playerToLoad->simulation;
    string playerName = playerToLoad->GetName();
    int israndom = (playerToLoad->seed != 0); // This is probably wrong...

    SETOPTION( "options/simulation/random-universe", israndom );
    SETOPTION( "options/simulation/random-seed", playerToLoad->seed );
	
	// Load the Simulation
	if( !simulation.Load( simName ) )
	{
		LogMsg(ERR,"Failed to load the Simulation '%s' successfully",simName.c_str());
		return;
	}

	if( !simulation.SetupToRun() )
	{
		LogMsg(ERR,"Failed to setup the Simulation '%s' successfully.",simName.c_str());
		return;
	}

	// Close all Windows
	while( UI::Search("/Window/") )
		UI::Close( UI::Search("/Window/") );

	UI::SwapScreens( "In Game", menuSplash, gameSplash );
	
	// Create or Load the Player
	if( players->PlayerExists(playerName) ) {
		simulation.LoadPlayer( playerName );
	} else{
		simulation.CreateDefaultPlayer( playerName );
		Lua::Call("intro");
	}
	
	// Run the Simulation
	bool alive = simulation.Run();
	UI::SwapScreens( "Main Screen", gameSplash, menuSplash );

	if( alive )
	{
		// Continue Button
		Picture *continueButton = NULL;
		continueButton = new Picture(Video::GetWidth() - 300, 200, "Resources/Graphics/txt_continue_inactive.png");
		continueButton->RegisterAction( Action_MouseLUp, new VoidAction( Menu::ContinueGame ) );
		SetPictureHover( continueButton, Image::Get( "Resources/Graphics/txt_continue_active.png"),
							  Image::Get( "Resources/Graphics/txt_continue_inactive.png") );
		UI::Add( continueButton );
	}
}

/** Continue a background Simulation
 */
void Menu::ContinueGame()
{
    // Only attempt to Run if the Simulation has loaded
    assert( simulation.isLoaded() );
    UI::SwapScreens( "In Game", menuSplash, gameSplash );
    bool alive = simulation.Run();
    UI::SwapScreens( "Main Screen", gameSplash, menuSplash );
    if( !alive )
    {
        UI::Close( continueButton );
        continueButton = NULL;
    }
}

/** This Window will launch the editor.
 *  \details The User can either create a new Simulation from scratch, or edit an existing simulation.
 */
void Menu::CreateEditWindow()
{
	// Return to Editor if it has alread been loaded
	if( simulation.isLoaded() ) {
		UI::SwapScreens( "Editor", menuSplash, editSplash );
		simulation.Edit();
		UI::SwapScreens( "Main Screen", editSplash, menuSplash );
		return;
	}

	// Don't create a Window if it already exists
	if( UI::Search("/Window'Editor'/") != NULL ) {
		return;
	}

	char seed[20];
	snprintf(seed, sizeof(seed), "%d", rand() );

	Window *editorWnd = NULL;
	UI::Add( editorWnd = (new Window(200, 200, 250, 300, "Editor"))
		->AddChild( (new Tabs( 10, 40, 230, 210, "EDIT TABS"))
			->AddChild( (new Tab( "Edit" ))
				->AddChild( (new Label(15, 15, "Pick the Simulation to Edit:")) )
				->AddChild( (new Dropdown( 45, 45, 100, 30 ))
					->AddOptions( Filesystem::Enumerate("Resources/Simulation/") ) )
			)
			->AddChild( (new Tab( "Create" ))
				->AddChild( (new Label(15, 10, "Simulation Name:")) )
				->AddChild( (new Textbox(40, 40, 80, 1, "", "Simulation Name")) )
				->AddChild( (new Checkbox(15, 90, 0, "Start With Random Universe")) )
				->AddChild( (new Label(15, 120, "Seed:")) )
				->AddChild( (new Textbox(50, 120, 80, 1, seed, "Random Universe Seed")) )
			)
		)
	);
	editorWnd->AddChild( new Button(10, 260, 100, 30, "Cancel", &UI::Close, editorWnd ) );
	editorWnd->AddChild( new Button(140, 260, 100, 30, "Edit", Menu::StartEditor ) );
	editorWnd->AddCloseButton();
}

/** Start an Editor Simulation
 */
void Menu::StartEditor()
{
	string simName = "default";
	assert( UI::Search("/Window'Editor'/Tabs/Tab/") != NULL );
	assert( false == simulation.isLoaded() );

	UI::Close( play ); // Play
	UI::Close( load ); // Load
	play = NULL;
	load = NULL;

	// Since the Random Universe Editor is currently broken, disable this feature here.
	SETOPTION( "options/simulation/random-universe", 0 );

	Tab* activeTab = ((Tabs*)UI::Search("/Window'Editor'/Tabs/"))->GetActiveTab();
	if( activeTab->GetName() == "Edit" ) {
		simName = ((Dropdown*)activeTab->Search("/Dropdown/"))->GetText();
		if( !simulation.Load( simName ) )
		{
			LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
			return;
		}
	} else { // Create
		simName = ((Textbox*)activeTab->Search("/Textbox'Simulation Name'/"))->GetText();

		// Random Universe options
		int israndom = ((Checkbox*)activeTab->Search("/Checkbox'Start With Random Universe'/"))->IsChecked();
		int seed = atoi( ((Textbox*)activeTab->Search("/Textbox'Random Universe Seed'/"))->GetText().c_str() );
		SETOPTION( "options/simulation/random-universe", israndom );
		SETOPTION( "options/simulation/random-seed", seed );

		simulation.New( simName );
	}

	if( !simulation.SetupToEdit() )
	{
		LogMsg(ERR,"Failed to setup the Simulation '%s' successfully.",simName.c_str());
		return;
	}

	// Only attempt to Edit if the Simulation has loaded
	assert( simulation.isLoaded() );

	// Close all Windows
	while( UI::Search("/Window/") )
		UI::Close( UI::Search("/Window/") );
	
	UI::SwapScreens( "Editor", menuSplash, editSplash );
	simulation.Edit();
	UI::SwapScreens( "Main Screen", editSplash, menuSplash );
}

/** Erase a Player
 * \note The user must confirm their choice first.
 */
void Menu::ErasePlayer( void* playerInfo )
{
	bool choice = Dialogs::Confirm("Are you sure you want erase this player?");

	if(choice) {
		string playerName = ((PlayerInfo*)playerInfo)->GetName();
		Players *players = Players::Instance();

		if(players->DeletePlayer(playerName))
			Dialogs::Alert("Player deleted successfully.");
		else
			Dialogs::Alert("A problem occurred while deleting the player.");

	    UI::Close( UI::Search("/Window'Load Game'/") );
	}
}

/** Create a new Player
 *  \details
 *  This Validates that the player names is valid.
 *  - Player names cannot be duplicates of another Player.
 *  - Player names cannot include characters reserved by filesystems.
 *  \note This doesn't actually create the Payer, this creates a PlayerInfo and
 *  selects the Simulation.  StartGame is where the Player object is first created.
 */
void Menu::CreateNewPlayer( )
{
	Players *players = Players::Instance();

	string playerName = ((Textbox*)UI::Search("/Window'New Game'/Textbox'Player Name:'/"))->GetText();
    string simName = ((Dropdown*)UI::Search("/Window'New Game'/Frame/Dropdown/"))->GetText();
    int israndom = ((Checkbox*)UI::Search("/Window'New Game'/Frame/Checkbox'Random Universe'/"))->IsChecked();
    int seed = atoi( ((Textbox*)UI::Search("/Window'New Game'/Frame/Textbox'Random Universe Seed'/"))->GetText().c_str() );

	if(OPTION(int, "options/sound/buttons")) Sound::Get( "Resources/Audio/Interface/28853__junggle__btn043.ogg" )->Play();

	if(players->PlayerExists(playerName)) {
		Dialogs::Alert("A player with that name exists.");
		return;
	}

	if(Filesystem::FilenameIsSafe(playerName) == false) {
		Dialogs::Alert("The following cannot be used: <>:\"/\\|?*");
		return;
	}

    SETOPTION( "options/simulation/random-universe", israndom);
    SETOPTION( "options/simulation/random-seed", seed );

    playerToLoad = new PlayerInfo( playerName, simName, seed );

    StartGame( playerToLoad );
}

/** Inserts a random integer into the Universe Seed Textbox
 */
void Menu::RandomizeSeed( )
{
	char seed[20];
	snprintf(seed, sizeof(seed), "%d", rand() );
	Widget *widget = UI::Search("/Window/Frame/Textbox'Random Universe Seed'/");
	if( widget->GetMask() == WIDGET_TEXTBOX )
	{
		Textbox* seedBox = (Textbox*)widget;
		seedBox->SetText( seed );
	}
}

/** Change a Picture to a different Image
 */
void Menu::ChangePicture( void* picture, void* image)
{
	((Picture*)picture)->Set( (Image*)image );
}

/** Make a Picture change images when the mouse hovers over it.
 */
void Menu::SetPictureHover( void* picture, void* activeImage, void* inactiveImage)
{
	Picture* pic = ((Picture*)picture);
	pic->RegisterAction( Action_MouseEnter, new MessageAction( ChangePicture, pic,   activeImage) );
	pic->RegisterAction( Action_MouseLeave, new MessageAction( ChangePicture, pic, inactiveImage) );
}

/** Quit the Main Menu
 */
void Menu::QuitMenu()
{
    quitSignal = true;
}

