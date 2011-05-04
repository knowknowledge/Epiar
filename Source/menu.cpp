/**\file		menu.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \brief		Runs the main menu
 * \details
 */

#include "Engine/simulation.h"
#include "menu.h"
#include "UI/ui.h"
#include "Utilities/filesystem.h"
#include "Utilities/timer.h"

typedef enum {
	Menu_DoNothing      = 1<<0,
	Menu_New            = 1<<1,
	Menu_Load           = 1<<2,
	Menu_Continue       = 1<<3,
	Menu_Options        = 1<<4,
	Menu_Editor         = 1<<5,
	Menu_Exit           = 1<<6,
	Menu_Confirm_New    = 1<<7,
	Menu_Confirm_Load   = 1<<8,
	Menu_Confirm_Editor = 1<<9,
	Menu_ALL            = 0xFFFF,
} menuOption;

PlayerInfo* playerToLoad = NULL;
menuOption clicked = Menu_DoNothing;

// Currently Static functions are the only way I could think of to have C only 
void setMenuOption( void* value ) {
	clicked = *((menuOption*)value);
	Sound::Get( "Resources/Audio/Interface/28853__junggle__btn043.ogg" )->Play();
}
void LoadPlayer( void* value ) {
	clicked = Menu_Confirm_Load;
	playerToLoad = (PlayerInfo*)value;
}
void CloseNewGameUI( void* value ) {
	Widget *newGameWnd = UI::Search("/Window'New Game'/");
	UI::Close( newGameWnd );
}
void CloseLoadGameUI( void* value ) {
	Widget *newGameWnd = UI::Search("/Window'Load Game'/");
	UI::Close( newGameWnd );
}
void CloseEditorUI( void* value ) {
	Widget *editorWnd = UI::Search("/Window'Editor'/");
	UI::Close( editorWnd );
}
void ChangePicture( void* picture, void* image) {
	((Picture*)picture)->Set( (Image*)image );
}
void SetPictureHover( void* picture, void* activeImage, void* inactiveImage) {
	Picture* pic = ((Picture*)picture);
	pic->RegisterAction( Widget::Action_MouseEnter, new MessageAction( ChangePicture, pic,   activeImage) );
	pic->RegisterAction( Widget::Action_MouseLeave, new MessageAction( ChangePicture, pic, inactiveImage) );
}

/** Epiar's Main Menu
 *
 *  This runs a while(1) loop collecting user input and drawing the screen.
 *  While similar to the Run Loop in the Simulation, this should be simpler
 *  since there is no HUD, Console or Sprites.
 *
 */
void Main_Menu( void ) {
	bool quitSignal = false;
	bool screenNeedsReset = true;
	Input inputs;
	list<InputEvent> events;
	menuOption availableMenus = (menuOption)(Menu_New | Menu_Load | Menu_Editor | Menu_Exit);
	int screenNum, numScreens;
	int button_x = OPTION( int, "options/video/w" ) - 300;

	// These are instances of the menuOptions so that they can be passed to the Buttons as values
	menuOption menu_New            = Menu_New;
	menuOption menu_Load           = Menu_Load;
	menuOption menu_Confirm_New    = Menu_Confirm_New;
	menuOption menu_Continue       = Menu_Continue;
	menuOption menu_Options        = Menu_Options;
	menuOption menu_Editor         = Menu_Editor;
	menuOption menu_Confirm_Editor = Menu_Confirm_Editor;
	menuOption menu_Exit           = Menu_Exit;

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
	string simName = "default";
	Simulation debug;

	Players *players = Players::Instance();
	players->Load( "Resources/Definitions/saved-games.xml", true, true);

	// Input Loop
	do {
		if (screenNeedsReset) {
			UI::CloseAll();

			// Create UI
			if( availableMenus & Menu_New )
			{
				Picture *pic = new Picture( button_x, 200, "Resources/Graphics/txt_new_game_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_New ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_new_game_active.png"),
				                      Image::Get( "Resources/Graphics/txt_new_game_inactive.png") );
				UI::Add( pic );
			}
			if( (availableMenus & Menu_Load) && (players->Size() > 0) )
			{
				Picture *pic = new Picture(button_x, 250, "Resources/Graphics/txt_load_game_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_Load ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_load_game_active.png"),
				                      Image::Get( "Resources/Graphics/txt_load_game_inactive.png") );
				UI::Add( pic );
			}
			if( availableMenus & Menu_Continue )
			{
				Picture *pic = new Picture(button_x, 200, "Resources/Graphics/txt_continue_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_Continue ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_continue_active.png"),
				                      Image::Get( "Resources/Graphics/txt_continue_inactive.png") );
				UI::Add( pic );
			}
			if( availableMenus & Menu_Editor )
			{
				Picture *pic = new Picture(button_x, 300, "Resources/Graphics/txt_editor_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_Editor ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_editor_active.png"),
				                      Image::Get( "Resources/Graphics/txt_editor_inactive.png") );
				UI::Add( pic );
			}
			if( availableMenus & Menu_Options )
			{
				Picture *pic = new Picture(button_x, 400, "Resources/Graphics/txt_options_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_Options ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_options_active.png"),
				                      Image::Get( "Resources/Graphics/txt_options_inactive.png") );
				UI::Add( pic );
			}
			if( availableMenus & Menu_Exit )
			{
				Picture *pic = new Picture(button_x, 500, "Resources/Graphics/txt_exit_inactive.png");
				pic->RegisterAction( Widget::Action_MouseLUp, new ObjectAction( setMenuOption, &menu_Exit ) );
				SetPictureHover( pic, Image::Get( "Resources/Graphics/txt_exit_active.png"),
				                      Image::Get( "Resources/Graphics/txt_exit_inactive.png") );
				UI::Add( pic );
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
				if( UI::Search("/Window'New Game'/") != NULL ) break;

				Window* win = new Window(300, 150, 250, 370, "New Game");
				UI::Add( win );

				// Player Name
				win->AddChild( (new Label(30, 30, "Player Name:")) )
					->AddChild( (new Textbox(130, 30, 100, 1, "", "Player Name:")) );

				// Simulation Picker
				char seed[20];
				snprintf(seed, sizeof(seed), "%d", rand() );
				win->AddChild( (new Frame( 30, 90, 200, 120 ))
					->AddChild( (new Label(15, 15, "Simulation:")) )
					->AddChild( (new Dropdown( 80, 15, 100, 30 ))
						->AddOptions( Filesystem::Enumerate("Resources/Simulation/") ) )
					->AddChild( (new Checkbox(15, 60, 0, "Random Universe")) )
					->AddChild( (new Label(15, 80, "Seed:")) )
					->AddChild( (new Textbox(50, 80, 80, 1, seed, "Random Universe Seed")) )
				);
				win->AddChild( (new Button(10, 330, 100, 30, "Cancel", &CloseNewGameUI, NULL )) );
				win->AddChild( (new Button(140, 330, 100, 30, "Create", setMenuOption, &menu_Confirm_New)) );
				win->AddCloseButton();

				break;
			}

			case Menu_Load:
			{
				if( UI::Search("/Window'Load Game'/") != NULL ) break;

				Window* win = new Window(250, 50, 500, 700, "Load Game");
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
				win->AddChild( (new Button( 200, 630, 100, 30, "Cancel", &CloseLoadGameUI, NULL ) ) );
				win->AddCloseButton();
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
					int israndom = ((Checkbox*)UI::Search("/Window'New Game'/Frame/Checkbox'Random Universe'/"))->IsChecked();
					int seed = atoi( ((Textbox*)UI::Search("/Window'New Game'/Frame/Textbox'Random Universe Seed'/"))->GetText().c_str() );
					SETOPTION( "options/simulation/random-universe", israndom);
					SETOPTION( "options/simulation/random-seed", seed );
					playerName = ((Textbox*)UI::Search("/Window'New Game'/Textbox'Player Name:'/"))->GetText();
					simName = ((Dropdown*)UI::Search("/Window'New Game'/Frame/Dropdown/"))->GetText();
					simName = simName;
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
				// Return to Editor if it has alread been loaded
				if( debug.isLoaded() ) {
					UI::SwapScreens( "Editor", menuSplash, editSplash );
					debug.Edit();
					UI::SwapScreens( "Main Screen", editSplash, menuSplash );
					break;
				}

				// Don't create a Window if it already exists
				if( UI::Search("/Window'Editor'/") != NULL ) {
					break;
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
				editorWnd->AddChild( new Button(140, 260, 100, 30, "Edit", setMenuOption, &menu_Confirm_Editor ) );
				editorWnd->AddChild( new Button(10, 260, 100, 30, "Cancel", &CloseEditorUI, NULL ) );
				
				break;
			}

			case Menu_Confirm_Editor:
			{
				assert( UI::Search("/Window'Editor'/Tabs/Tab/") != NULL );
				assert( false == debug.isLoaded() );
				screenNeedsReset = true;
				availableMenus = (menuOption)(availableMenus & ~Menu_New);
				availableMenus = (menuOption)(availableMenus & ~Menu_Load);
				availableMenus = (menuOption)(availableMenus | Menu_Options);

				// Since the Random Universe Editor is currently broken, disable this feature here.
				SETOPTION( "options/simulation/random-universe", 0 );

				Tab* activeTab = ((Tabs*)UI::Search("/Window'Editor'/Tabs/"))->GetActiveTab();
				printf( "Active Tab: %s\n", activeTab->GetName().c_str() );
				if( activeTab->GetName() == "Edit" ) {
					simName = ((Dropdown*)activeTab->Search("/Dropdown/"))->GetText();
					if( !debug.Load( simName ) )
					{
						LogMsg(ERR,"Failed to load '%s' successfully",simName.c_str());
						break;
					}
				} else { // Create
					simName = ((Textbox*)activeTab->Search("/Textbox'Simulation Name'/"))->GetText();

					// Random Universe options
					int israndom = ((Checkbox*)activeTab->Search("/Checkbox'Start With Random Universe'/"))->IsChecked();
					int seed = atoi( ((Textbox*)activeTab->Search("/Textbox'Random Universe Seed'/"))->GetText().c_str() );
					SETOPTION( "options/simulation/random-universe", israndom );
					SETOPTION( "options/simulation/random-seed", seed );

					debug.New( simName );
				}

				debug.SetupToEdit();

				// Only attempt to Edit if the Simulation has loaded
				assert( debug.isLoaded() );
				
				UI::SwapScreens( "Editor", menuSplash, editSplash );
				debug.Edit();
				UI::SwapScreens( "Main Screen", editSplash, menuSplash );
				
				break;
			}

			case Menu_Exit:
				quitSignal = true;
				break;

			default:
				break;
		}

		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYUP, SDLK_ESCAPE ) ) ) {
			quitSignal = true;
		}

		// Wait until the next click
		Timer::Delay(75);
	} while(!quitSignal);
}

