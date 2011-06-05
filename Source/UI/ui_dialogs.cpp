/**\file			ui_dialogs.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Thusday, May 5, 2011
 * \date			Modified: Thursday, May 5, 2011
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Dialogs
 * \brief Common UI dialogs, e.g. "Yes/No", "Ok". */

void CancelConfirmDialog( void* value ) {
	*(int *)value = 0;
	UI::ReleaseModality();
}
void OKConfirmDialog( void* value ) {
	*(int *)value = 1;
	UI::ReleaseModality();
}
void OKAlertDialog( void* value ) {
	UI::ReleaseModality();
}

/**\brief Presents a question with "Ok/Cancel" buttons.
 * \return Returns true on "Ok", false otherwise
 */
bool Dialogs::Confirm( const char *message )
{
	Window* win = new Window(325, 265, 325, 130, "Confirm");
	static int value = 0;

	win->AddChild( ( new Label( 45, 35, message ) ) )
	->AddChild( (new Button( 65, 90, 80, 30, "Cancel", CancelConfirmDialog, &value ) ) )
	->AddChild( (new Button( 190, 90, 80, 30, "OK", OKConfirmDialog, &value ) ) );

	UI::ModalDialog( win );

	return value;
}

/**\brief Presents a message with a single "Ok" button.
 */
void Dialogs::Alert( const char *message )
{
	Window* win = new Window(325, 265, 325, 130, "Alert");

	win->AddChild( ( new Label( 45, 35, message ) ) )
	->AddChild( (new Button( 130, 90, 80, 30, "OK", OKAlertDialog, NULL ) ) );

	UI::ModalDialog( win );
}

/**\brief Create a Presents a message with a single "Ok" button.
 */
void Dialogs::Options()
{
	int yoff;
	int width = 300;
	int height = 400;
	int tabwidth = width - 20;
	int tabheight = height - 100;

	if( UI::Search("/Window'Options'/'") )
	{
		LogMsg(INFO, "Options Window already open. Closing it.");
		UI::Close( UI::Search("/Window'Options'/'") );
	}

	Window *window = new Window( 30, 100, width, height, "Options");
	Tabs *optionTabs = new Tabs( 10, 30, tabwidth, tabheight, "Options Tabs" );
	Button *accept = new Button( 60, height-50, 60, 30, "Save"); // TODO: Make it do something
	Button *cancel = new Button( 160, height-50, 60, 30, "Cancel"); // TODO: Make it do something

	window->AddChild(optionTabs);
	window->AddChild(accept);
	window->AddChild(cancel);
	window->AddCloseButton();
	window->SetFormButton(accept);

	// Game Options
	yoff = 10;
	optionTabs->AddChild( (new Tab("Game") )
		->AddChild( new Label( 20, 5, "Game Options:", 0 ) )
		->AddChild( new Checkbox( 20, (yoff+=20), OPTION(int, "options/video/fullscreen"), "Run as Full Screen" ) )
		->AddChild( new Checkbox( 20, (yoff+=20), OPTION(int, "options/simulation/random-universe"), "Create a Random Universe" ) )
		->AddChild( new Checkbox( 20, (yoff+=20), OPTION(int, "options/simulation/automatic-load"), "Automatically Load the last Player") )
	);

	// Sound Options
	yoff = 0;
	optionTabs->AddChild( (new Tab("Sounds") )
		->AddChild( new Label( 20, 5, "Sound Options:", 0 ) )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/sound/background"), "Background sounds") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/sound/weapons"), "Weapons sounds") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/sound/engines"), "Engines sounds") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/sound/explosions"), "Explosions sounds") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/sound/buttons"), "Buttons sounds") )
		->AddChild( new Slider( 20, 40 + 20*(yoff), 80, 16, "Sound Volume") )
		->AddChild( new Label( 105, 30 + 20*(yoff++), "Sound Volume", false) )
		->AddChild( new Slider( 20, 40 + 20*(yoff), 80, 16, "Music Volume") )
		->AddChild( new Label( 105, 30 + 20*(yoff++), "Music Volume", false) )
	);

	// Developer Options
	yoff = 0;
	optionTabs->AddChild( (new Tab("Developer") )
		->AddChild( new Label( 20, 5, "Developer Options:", 0 ) )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/log/xml"), "Save Log Messages") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/log/out"), "Print Log Messages") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/log/alert"), "Alert Log Messages") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/log/ui"), "Save UI as XML") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/log/sprites"), "Save Sprites as XML") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/development/debug-ai"), "Display AI State Machine") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/development/debug-ui"), "Display UI Debug Information") )
		->AddChild( new Checkbox( 20, 30 + 20*(yoff++), OPTION(int, "options/development/ships-worldmap"), "Display Ships on the Universe Map") )
	);

	// Keyboard Options
	yoff = 0;
	optionTabs->AddChild( (new Tab("Keyboard") )
		->AddChild( new Label( 20, 5, "Keyboard Options:", 0 ) )
	);

	UI::Add(window);
}

/** @} */
