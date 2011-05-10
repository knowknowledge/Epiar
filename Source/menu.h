#ifndef H_MENU
#define H_MENU

/**\file		menu.h
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created: Tuesday, April 26, 2011
 * \brief		Export Main_Menu function.
 * \details
 */

class Menu {
	public:
	static void Main_Menu( void ); // Run the Main Menu

	private:
	// Menu Action Flags
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
	static menuOption menu_New;
	static menuOption menu_Load;
	static menuOption menu_Confirm_New;
	static menuOption menu_Continue;
	static menuOption menu_Options;
	static menuOption menu_Editor;
	static menuOption menu_Confirm_Editor;
	static menuOption menu_Exit;
	static menuOption clicked;

	// Images
	static Image* menuSplash;
	static Image* gameSplash;
	static Image* editSplash;

	// GUI Functions
	static void SetupGUI();
    static void SetPictureHover( void* picture, void* activeImage, void* inactiveImage);
    static void SetMenuOption( void* value );

	// GUI Callbacks
    static void ChangePicture( void* picture, void* image);
    static void CloseEditorUI( void* value );
    static void CloseLoadGameUI( void* value );
    static void CloseNewGameUI( void* value );
    static void ErasePlayer( void *value );
    static void LoadPlayer( void* value );
    static void RandomizeSeed( void* value );

};


#endif // H_MENU

