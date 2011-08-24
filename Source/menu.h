/**\file		menu.h
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \brief		Export Main_Menu function.
 * \details
 */

#ifndef __H_MENU
#define __H_MENU

#include "Engine/simulation.h"
#include "Graphics/image.h"
#include "UI/ui.h"
#include "UI/ui_picture.h"

class Menu {
	public:
	static void Main_Menu( void ); // Run the Main Menu

	private:
	static bool quitSignal;
	// The Simulation
	static Simulation simulation;
	static PlayerInfo* playerToLoad;

	// Images
	static Image* menuSplash;
	static Image* gameSplash;
	static Image* editSplash;

	// Menu Buttons
	// These pointers are only necessary because UI::Search does not support Picture paths.
	static Picture *play;
	static Picture *load;
	static Picture *edit;
	static Picture *options;
	static Picture *exit;
	static Picture *continueButton;

	// Skip straight to the Game
	static bool AutoLoad( void );

	// GUI Setup and Actions
	static void SetupGUI();
	static void CreateNewWindow();
	static void CreateLoadWindow();
	static void StartGame( void* playerInfo );
	static void ContinueGame();
	static void CreateEditWindow();
	static void StartEditor();
	static void QuitMenu();

	// Player Management
	static void CreateNewPlayer();
	static void ErasePlayer( void *playerInfo );

    // GUI Helpers
	static Picture* PictureButton( int x, int y, void (*callback)(), Image* activeImage, Image* inactiveImage);
	static void ChangePicture( void* picture, void* image);
	static void RandomizeSeed();

};

#ifdef EPIAR_UI_TESTS
void UI_Test();
void ModalityTest();
#endif // EPIAR_UI_TESTS

#endif // __H_MENU

