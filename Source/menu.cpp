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
#include "UI/widgets.h"
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
		if( AutoLoad() )
		{
			LogMsg(INFO,"AutoLoaded Game Complete. Quitting Epiar.");
			return;
		}
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
 * \returns true if the player was loaded successfully.
 */
bool Menu::AutoLoad()
{
	LogMsg(INFO,"Attempting to automatically load a player.");
	PlayerInfo* info = Players::Instance()->LastPlayer();
	if( info != NULL )
	{
		LogMsg(INFO, "Automatically loading player.", info->GetName().c_str() );
		if( !simulation.Load( info->simulation ) )
		{
			LogMsg(ERR,"Failed to load the Simulation '%s' successfully", info->simulation.c_str() );
			return false;
		}
		if( !simulation.SetupToRun() )
		{
			LogMsg(ERR,"Failed to setup the Simulation '%s' successfully.", info->simulation.c_str() );
			return false;
		}
		simulation.LoadPlayer( info->GetName() );
		simulation.Run();
		return true;
	}
	LogMsg(WARN,"No available players to load.");
	return false;
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
	UI::Add( new Picture( 0,0, Video::GetWidth(), Video::GetHeight(), menuSplash, true) );

	// Add the logo
	UI::Add( new Picture(Video::GetWidth() - 240, Video::GetHeight() - 120, Image::Get("Resources/Art/logo.png") ) );

	// New Button
	play = PictureButton( button_x, 200, Menu::CreateNewWindow,
	                      Image::Get( "Resources/Graphics/txt_new_game_active.png"),
	                      Image::Get( "Resources/Graphics/txt_new_game_inactive.png") );

	// Load Button
	if( (Players::Instance()->Size() > 0) )
	{
		load = PictureButton( button_x, 250, Menu::CreateLoadWindow,
		                       Image::Get( "Resources/Graphics/txt_load_game_active.png"),
		                       Image::Get( "Resources/Graphics/txt_load_game_inactive.png") );
	}

	// Editor Button
	edit = PictureButton( button_x, 300, Menu::CreateEditWindow,
	                       Image::Get( "Resources/Graphics/txt_editor_active.png"),
	                       Image::Get( "Resources/Graphics/txt_editor_inactive.png") );

	// Options Button
	options = PictureButton( button_x, 400, Dialogs::OptionsWindow,
	                          Image::Get( "Resources/Graphics/txt_options_active.png"),
	                          Image::Get( "Resources/Graphics/txt_options_inactive.png") );

	// Exit Button
	exit = PictureButton( button_x, 500, QuitMenu,
	                      Image::Get( "Resources/Graphics/txt_exit_active.png"),
	                      Image::Get( "Resources/Graphics/txt_exit_inactive.png") );

#ifdef EPIAR_UI_TESTS
	// Test that the GUI features work
	UI_Test();
#endif // EPIAR_UI_TESTS

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

	PlayerInfo* last = Players::Instance()->LastPlayer();

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
		if( info == last ) {
			win->SetFormButton( (Button*) win->Search("/Frame/Button'Play'/") );
		}
	}
	win->AddChild( (new Button( 200, 155*names->size() + 20, 100, 30, "Cancel", &UI::Close, win)) );
	win->AddCloseButton();
	UI::RegisterKeyboardFocus(win);
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
		continueButton = PictureButton( Video::GetWidth() - 300, 200, Menu::ContinueGame,
		                                Image::Get( "Resources/Graphics/txt_continue_active.png"),
		                                Image::Get( "Resources/Graphics/txt_continue_inactive.png") );
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
	Widget *widget = UI::Search("/Window'New Game'/Frame/Textbox'Random Universe Seed'/");
	if( widget && widget->GetMask() == WIDGET_TEXTBOX )
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
Picture* Menu::PictureButton( int x, int y, void (*callback)(), Image* activeImage, Image* inactiveImage)
{
	Picture* pic = new Picture( x, y, inactiveImage );
	pic->RegisterAction( Action_MouseLUp, new VoidAction( callback ) );
	pic->RegisterAction( Action_MouseEnter, new MessageAction( ChangePicture, pic,   activeImage) );
	pic->RegisterAction( Action_MouseLeave, new MessageAction( ChangePicture, pic, inactiveImage) );
	UI::Add( pic );
	return pic;
}

/** Quit the Main Menu
 */
void Menu::QuitMenu()
{
    quitSignal = true;
}

#ifdef EPIAR_UI_TESTS

void AddImage( void*widget, void*image, int x, int y )
{
	Container* container = ((Container*)widget);
	container->AddChild( new Picture( x, y, (Image*)image ) );
}

string LOREM =
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor"
	" incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
	"nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
	" Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu"
	" fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in"
	" culpa qui officia deserunt mollit anim id est laborum.";


void ModalityTest() {
	Window* window = new Window( Video::GetWidth()/2-150, Video::GetHeight()/2-150, 300, 300, "Dialog" );
	window->AddChild( (new Paragraph(30, 30, 250, 30, "This is a Modal Dialog.  You should be unable to click elsewhere until you click the 'Release' button below." )) );
	window->AddChild( (new Button(100, 135, 100, 30, "Release", UI::ReleaseModality )) );
	UI::ModalDialog( window );
}
void TestConfirm()
{
	Dialogs::Confirm( LOREM );
}
void TestAlert()
{
	Dialogs::Alert( LOREM );
}

void UI_Test() {
	// Example of Nestable UI Creation
	UI::Add(
		(new Window( 20, 20, 600, 600, "A Window"))
		->AddChild( (new Tabs( 50, 50, 500, 500, "TEST TABS"))
			->AddChild( (new Tab( "Nested Frames" ))
				->AddChild( (new Button(10, 10, 100, 30, "Dummy 1",    NULL    )) )
				->AddChild( (new Frame( 50,50,400,400 ))
					->AddChild( (new Button(10, 10, 100, 30, "Dummy 2",    NULL    )) )
					->AddChild( (new Frame( 50,50,300,300 ))
						->AddChild( (new Button(10, 10, 100, 30, "Dummy 3",    NULL    )) )
						->AddChild( (new Frame( 50,50,200,200 ))
							->AddChild( (new Button(10, 10, 100, 30, "Dummy 4",    NULL    )) )
						)
					)
				)
			)
			->AddChild( (new Tab( "Scoll to Buttons" ))
				->AddChild( (new Label(10,   0, "Scroll Down")) )
				->AddChild( (new Frame( 150, 50, 200, 300 ))
					->AddChild( (new Label(10,   0, "Scroll Down")) )
					->AddChild( (new Button(10, 300, 100, 30, "Dummy",    NULL    )) )
					->AddChild( (new Label(10, 600, "Scroll Up")) )
				)
				->AddChild( (new Label(10, 600, "Scroll Up")) )
			)
			->AddChild( (new Tab("A Picture"))
				->AddChild( (new Picture(10, 0, 400, 400, "Resources/Art/menu2.png")) )
			)
			->AddChild( (new Tab("Inputs"))
				->AddChild( (new Textbox(30, 30, 100, 1, "Some Text Goes Here", "A Textbox")) )
				->AddChild( (new Checkbox(30, 100, 0, "A Checkbox")) )
				->AddChild( (new Slider(30, 200, 200, 100, "A Slider", 0.4f )) )
				->AddChild( (new Button(10, 300, 100, 30, "Dummy", NULL )) )
				->AddChild( (new Dropdown(200, 200, 100, 30))
					->AddOption("Lorem")
					->AddOption("Ipsum")
					->AddOption("Dolar")
					->AddOption("Sit")
				)
				->AddChild( (new Dropdown(300, 200, 100, 20))
					->AddOption("One Fish")
					->AddOption("Two Fish")
					->AddOption("Red Fish")
					->AddOption("Blue Fish")
				)
				->AddChild( (new Paragraph(300, 250, 100, 20, LOREM)) )
				->AddChild( (new Textarea(10, 300, 250, 500, LOREM, "A Textarea")) )
			)
			->AddChild( (new Tab("Dialogs"))
				->AddChild( (new Button(10, 10, 100, 30, "Modality Test", ModalityTest )) )
				->AddChild( (new Button(10, 50, 100, 30, "Confirm Test", TestConfirm )) )
				->AddChild( (new Button(10, 90, 100, 30, "Alert Test", TestAlert )) )
			)
		)
	);

	Tab* clickTestTab = new Tab("Click Test");
	clickTestTab->RegisterAction( Action_MouseLUp, new PositionalAction( AddImage, clickTestTab, Image::Get("Resources/Graphics/shuttle.png") ) );
	((Container*)UI::Search("/'A Window'/'TEST TABS'/"))->AddChild( clickTestTab );

	// Check that the UI Searching is working
	assert( NULL != UI::Search("/[0]/") );
	assert( NULL != UI::Search("/Window/") );
	assert( NULL != UI::Search("/Window/Tabs/") );
	assert( NULL != UI::Search("/(100,100)/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab[1]/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/Frame/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/[0]/(60,60)/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Button/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Frame/Button/") );
	assert( NULL != UI::Search("/'A Window'/'TEST TABS'/Tab/Frame/Frame/Frame/Button/") );

	// Check Odd but acceptable Queries
	// Slashes at the beginning and end are optional but expected
	assert( NULL != UI::Search("Window") );
	assert( NULL != UI::Search("/Window") );
	assert( NULL != UI::Search("Window/") );
	// Extra Slashes just grab the first child
	assert( NULL != UI::Search("/Window//") );
	assert( NULL != UI::Search("/Window//Tab/") );

	// Check that Bad Inputs find nothing
	assert( NULL == UI::Search("/[-1]/") );
	//assert( NULL == UI::Search("/[fdsa]/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/(foo,bar)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/WindowWindow/") );
	assert( NULL == UI::Search("/'foobar'/") );
	//assert( NULL == UI::Search("/[]/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int
	//assert( NULL == UI::Search("/(,)/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int

	// Check that Malformed Queries Fail and return nothing.
	// Malformed Indexes
	//assert( NULL == UI::Search("/[/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/]/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/[1)/") );
	assert( NULL == UI::Search("/[100,100]/") );
	// Malformed Coordinates
	//assert( NULL == UI::Search("/(/") ); // TODO: Should fail, but Asserts in string convert
	//assert( NULL == UI::Search("/)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/,/") ); 
	//assert( NULL == UI::Search("/(100 100)/") ); // TODO: Should fail, but Asserts in string convert
	assert( NULL == UI::Search("/(100,100]/") );
	// Malformed Strings
	assert( NULL == UI::Search("/Window'/") );
	assert( NULL == UI::Search("/Window\"/") );
	assert( NULL == UI::Search("/'Window/") );
	assert( NULL == UI::Search("/\"Window/") );

	// Set a test Form button
	Tab* inputTab = (Tab*)UI::Search("/'A Window'/'TEST TABS'/Tab'Inputs'/");
	assert( NULL != inputTab );
	assert( inputTab->GetMask() & WIDGET_TAB );
	inputTab->SetFormButton(
		(Button*) UI::Search("/'A Window'/'TEST TABS'/Tab'Some Inputs'/Button'Dummy'/")
	);
}

#endif // EPIAR_UI_TESTS
