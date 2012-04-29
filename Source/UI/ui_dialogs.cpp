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
#include "UI/widgets.h"
#include "Utilities/log.h"
#include "Audio/audio.h"
#include "Audio/sound.h"

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
void OKAlertDialog() {
	UI::ReleaseModality();
}

/**\brief Presents a question with "Ok/Cancel" buttons.
 * \return Returns true on "Ok", false otherwise
 */
bool Dialogs::Confirm( const string message )
{
	static int value = 0;
	int width = 360;
	int height = 90;

	// First See how large the Paragraph is going to be.
	Paragraph* p = new Paragraph( 30, 30, 300, 130, message );
	height += p->GetH();

	Window* win = new Window( Video::GetWidth()/2-width/2, Video::GetHeight()/2-height/2, width, height, "Confirm");
	Button* no = new Button( (  width/3) -40, height-40, 80, 30, "Cancel", CancelConfirmDialog, &value );
	Button* ok = new Button( (2*width/3) -40, height-40, 80, 30, "OK", OKConfirmDialog, &value );

	win->AddChild( p );
	win->AddChild( no );
	win->AddChild( ok );
	win->SetFormButton( ok );
	win->RegisterAction(Action_Close, new VoidAction(UI::ReleaseModality) );
	win->AddCloseButton();

	UI::ModalDialog( win );

	LogMsg(DEBUG2,"Player has chosen: %s", (value?"OK":"Cancel"));

	return value;
}

/**\brief Presents a message with a single "Ok" button.
 */
void Dialogs::Alert( const string message )
{
	int width = 360;
	int height = 90;

	Paragraph* p = new Paragraph( 30, 30,300, 130, message );
	height += p->GetH();

	Window* win = new Window( Video::GetWidth()/2-width/2, Video::GetHeight()/2-height/2, width, height, "Alert");
	Button* ok = new Button( width/2-80/2, height-40, 80, 30, "OK", OKAlertDialog );

	win->AddChild( p );
	win->AddChild( ok );
	win->SetFormButton( ok );
	win->RegisterAction(Action_Close, new VoidAction(OKAlertDialog) );
	win->AddCloseButton();

	UI::ModalDialog( win );
}

void CheckOption( void* checkbox, void*option )
{
	Checkbox *box = (Checkbox *)checkbox;
	SETOPTION( (char*)option, box->IsChecked() );
}

Checkbox* OptionBox( const char* option, string name, int x, int y )
{
	Checkbox *box = new Checkbox( x, y, OPTION(int, option), name);
	box->RegisterAction( Action_MouseLUp, new MessageAction( CheckOption, box, (void*)option ) );
	return box;
}

void SlideOption( void* slider, void*option )
{
	float ratio = ((Slider*)slider)->GetVal();
	SETOPTION( (char*)option, ratio );
}

Slider* OptionSlider( const char* option, string name, int x, int y )
{
	Slider *slider = new Slider( x, y, 80, 16, name, OPTION(float, option) );
	slider->RegisterAction( Action_MouseDrag, new MessageAction( SlideOption, slider, (void*)option ) );
	return slider;
}

void SetSoundVolume( void* slider )
{
	Audio::Instance().SetSoundVol( ((Slider*)slider)->GetVal() );
}

void SetMusicVolume( void* slider )
{
	Audio::Instance().SetMusicVol( ((Slider*)slider)->GetVal() );
}

void CloseOptions() { UI::Close( UI::Search("/Window'Options'/") ); }

void SaveOptions() { Options::Save(); CloseOptions(); }

void ResetOptions() { Options::RestoreDefaults(); CloseOptions(); Dialogs::OptionsWindow(); }

/**\brief Create a Presents a message with a single "Ok" button.
 */
void Dialogs::OptionsWindow()
{
	int yoff;
	int width = 300;
	int height = 400;
	int tabwidth = width - 20;
	int tabheight = height - 100;

	if( UI::Search("/Window'Options'/") )
	{
		LogMsg(INFO, "Options Window already open. Closing it.");
		UI::Close( UI::Search("/Window'Options'/") );
		return;
	}

	Window *window = new Window( width, height, "Options");
	Tabs *optionTabs = new Tabs( 10, 30, tabwidth, tabheight, "Options Tabs" );
	Button *cancel = new Button(  20, height-50, 80, 30, "Cancel", CloseOptions );
	Button *reset  = new Button( 110, height-50, 80, 30, "Restore", ResetOptions );
	Button *accept = new Button( 200, height-50, 80, 30, "Save", SaveOptions );

	window->AddChild(optionTabs);
	window->AddChild(accept);
	window->AddChild(cancel);
	window->AddChild(reset);
	window->AddCloseButton();
	window->SetFormButton(accept);

	// Game Options
	{
	yoff = 10;
	Tab* tab =  new Tab("Game");
	tab->AddChild( new Label( 20, 5, "Game Options:", 0 ) );
	tab->AddChild( OptionBox( "options/video/fullscreen", "Run as Full Screen", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/simulation/random-universe", "Create a Random Universe", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/simulation/automatic-load", "Automatically Load the last Player", 20, (yoff+=20) ) );
	optionTabs->AddChild( tab );
	}

	// Sound Options
	{
	yoff = 10;
	Tab* tab =  new Tab("Sound");
	optionTabs->AddChild( tab );
	tab->AddChild( new Label( 20, 5, "Sound Options:", 0 ) );
	tab->AddChild( OptionBox( "options/sound/background", "Background sounds", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/sound/weapons", "Weapons sounds", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/sound/engines", "Engines sounds", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/sound/explosions", "Explosions sounds", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/sound/buttons", "Buttons sounds", 20, (yoff+=20) ) );

	Slider *sound = OptionSlider( "options/sound/soundvolume", "Sound Volume", 20, (yoff+=30));
	sound->RegisterAction( Action_MouseLUp, new ObjectAction(SetSoundVolume, sound) );
	tab->AddChild( sound );

	Slider *music = OptionSlider( "options/sound/musicvolume", "Music Volume", 20, (yoff+=30));
	music->RegisterAction( Action_MouseLUp, new ObjectAction(SetMusicVolume, music) );
	tab->AddChild( music );
	}

	// Developer Options
	{
	yoff = 10;
	Tab* tab =  new Tab("Developer");
	optionTabs->AddChild( tab );
	tab->AddChild( new Label( 20, 5, "Developer Options:", 0 ) );
	tab->AddChild( OptionBox( "options/log/xml", "Save Log Messages", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/log/out", "Print Log Messages", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/log/alert", "Alert Log Messages", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/log/ui", "Save UI as XML", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/log/sprites", "Save Sprites as XML", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/development/debug-ai", "Display AI State Machine", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/development/debug-ui", "Display UI Debug Information", 20, (yoff+=20) ) );
	tab->AddChild( OptionBox( "options/development/ships-worldmap", "Display Ships on the Universe Map", 20, (yoff+=20) ) );
	}

	// Keyboard Options
	/*
	{
	yoff = 0;
	Tab* tab =  new Tab("Sound");
	optionTabs->AddChild( tab );
	tab->AddChild( new Label( 20, 5, "Keyboard Options:", 0 ) );
	// TODO: Figure out how to populate this table.
	//       By design we may not have loaded Lua at this point.
	}
	*/

	UI::Add(window);
}

/** @} */
