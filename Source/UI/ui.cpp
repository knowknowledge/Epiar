/**\file			ui.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "Utilities/log.h"
#include "UI/ui.h"
#include "Input/input.h"
#include "Utilities/timer.h"

/** \defgroup UI
 * @{
 */

/**\class UI
 * \brief The User Interface is a collection of Widgets
 * \details This is the main Interface for all User Interface manipulations.
 */

/**\brief This container is contains everything on the screen.
 */

Container *UI::currentScreen = NULL;
map<string,Container*> UI::screens;
int UI::zlayer = 0;
list<UI::draw_location> UI::deferred;
list<UI::draw_location> UI::hovering;
Container *UI::backgroundScreen = NULL;
bool UI::modalEnabled = false;

/**\brief This is the default UI Font.
 */
Font *UI::font = NULL;
Sound *UI::beep = NULL;
Sound *UI::hover = NULL;

/**\brief Destroys the UI interface and all UI elements.
 */
UI::~UI() {
	UI::CloseAll();

	delete font;
	delete beep;
	delete hover;
}

/**\brief Initializes the User Interface.
 * \details This Initializes the screen container as the full screen and the loads the default UI font.
 */
bool UI::Initialize( string screenName ) {
	assert( currentScreen == NULL ); ///< This function can only be called once
	currentScreen = NewScreen( screenName );

	// This is the Default UI Font
	font = new Font( SKIN( "Skin/UI/Default/Font" ) );
	font->SetColor( Color( SKIN( "Skin/UI/Default/Color" ) ) );
	font->SetSize( convertTo<int>( SKIN("Skin/UI/Default/Size") ) );

 
	beep = Sound::Get( "Resources/Audio/Interface/28853__junggle__btn043.ogg" );
	hover = Sound::Get( "Resources/Audio/Interface/28820__junggle__btn010.ogg" );

	return true;
}

/**\brief Checks to see if there are UI elements.
*/
bool UI::Active( void ) {
	if( currentScreen->IsEmpty() ) return false;
	return true;
}

/**\brief Adds a Widget to the base UI.
 * \param[in] widget Widget pointer that should be added.
 * \returns The same pointer to the widget that was passed as input.
 */
Widget *UI::Add( Widget *widget ) {
	if( widget == NULL ) {
		LogMsg(ERR, "Attempt to add a NULL Widget! Widget not Added." );
		return (Widget*)NULL;
	}
	
	return UI::currentScreen->AddChild( widget );
}

/**\brief This removes all widgets from the base.
 */
void UI::CloseAll( void ) {
	LogMsg(INFO, "Closing all Widgets." );
	UI::currentScreen->Empty();
	UI::deferred.clear();
	UI::hovering.clear();
}

/**\brief This removes a single widget.
 * \details DelChild will search the UI Tree so the widget does not have to be at the top level.
 *          breadth-first search to find the specified widget.
 */
void UI::Close( Widget *widget ) {
	if( IsAttached( widget ) )
	{
		LogMsg(INFO, "Closing %s named %s.", widget->GetType().c_str(), widget->GetName().c_str() );
		UI::currentScreen->DelChild( widget );
	}
}

void UI::Close( void *unsafe) {
	Close( (Widget*)unsafe );
}

/**\brief Called when a Widget should be drawn later
 * \details Some Widgets should not be drawn "within" their container Widgets,
 * but should instead be drawn above them.
 */
void UI::Defer( Widget* widget, int x, int y ) {
	draw_location location = {widget,x,y};
	deferred.push_back( location );
}

/**\brief Called to flush the Deferred drawwing list
 * \details Some Widgets should not be drawn "within" their container Widgets,
 * but should instead be drawn above them.
 * \warn This is code is not reentrant.
 */
void UI::DrawDeferred( void ) {
	// Ensure that the zlayer has been correctly reset
	assert(zlayer == 0);
	zlayer = 0;
	hovering.clear();

	// Draw the Deferred Widgets
	list<draw_location>::iterator iter = deferred.begin();
	while( deferred.empty() == false ) {
		++zlayer;
		draw_location now_draw = deferred.front();
		hovering.push_back( now_draw );
		deferred.pop_front();

		if( IsAttached(now_draw.widget) ) {
			now_draw.widget->Draw( now_draw.x, now_draw.y );
		}

		// Some widget is broken and refuses to be Drawn.
		// TODO: This could detect and print a warning instead of asserting.
		assert( zlayer < 1000 );
	}

	zlayer = 0;
	assert( deferred.empty() );
}

/**\brief Drawing function.
 * 
 */
void UI::Draw( void ) {
	assert( deferred.empty() );
	if( UI::modalEnabled ) {
		UI::backgroundScreen->Draw();
		UI::DrawDeferred();
		// Draw a transparent grey rectangle over the background screen
		Video::DrawRect( 0,0, Video::GetWidth(), Video::GetHeight(), BLACK, 0.1 );
	}
	UI::currentScreen->Draw();
	UI::DrawDeferred();
}

/**\brief Search the UI for a Widget
 *
 * \see Container::Search
 */
Widget* UI::Search( string query ) {
	return UI::currentScreen->Search( query );
}

/**\brief Check if a Widget attached to the the current UI
 *
 * \warn Do not run any methods on the possible Widget.  It may not exist.
 *       Even calling possible->GetMask() will Segfault.
 * \note If this returns true, then the Widget must be properly allocated, but
 *       if this returns false the the widget may or may not be allocated.
 *       Not all Widgets are added to the User Interface immediately.
 * \see Container::IsAttached
 */
bool UI::IsAttached( Widget* possible ) {
	return UI::currentScreen->IsAttached( possible );
}

/**\brief Export The UI as an XML document.
 *
 */
void UI::Save( void ) {
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL;/* node pointers */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "UI" );
    xmlDocSetRootElement(doc, root_node);

	xmlAddChild( root_node, UI::currentScreen->ToNode() );

	xmlSaveFormatFileEnc( "Master_UI.xml" , doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
}

/**\brief Swap in a different UI Screen
 * \details If there is no screen matching the newname, a new screen is
 *          created. Otherwise, the old screen is reloaded.  Either way, the
 *          current screen is saved for later.
 * \param[in] newname The name of the screen to be loaded.
 * \todo The Background images should be built into the Screens themselves.
 */
void UI::SwapScreens(string newname, Image* oldBackground, Image* newBackground ) {
	Container *oldScreen;
	Container *newScreen;

	// Save the old Screen
	oldScreen = currentScreen;
	screens[ oldScreen->GetName() ] = oldScreen;

	// Load or Create a new Screen
	map<string,Container*>::iterator val = screens.find( newname );
	if( val != screens.end() ){
		newScreen = val->second;
	} else {
		newScreen = NewScreen( newname );
	}
	screens[ newScreen->GetName() ] = oldScreen;

	// Swap in the new Screen
	currentScreen = newScreen;

	// Animate one screen sliding over another
	// Since anything less than 10ms is unreliable from the timer perspective, 
	// There is a 10ms delay between frames.
	// Since "options/timing/screen-swap" is in ms use screen-swap / 10.
	// Only do this animation if it will finish in a finite time, so skip if time=0 or dx=0
	if ( (0 < OPTION(int, "options/timing/screen-swap") / 10)
	  && (0 < Video::GetWidth() / (OPTION(int, "options/timing/screen-swap")/10)) )
	{
		int dx = Video::GetWidth() / (OPTION(int, "options/timing/screen-swap")/10);
		int oldX = 0;
		int newX = Video::GetWidth();
		Timer::Update();
		while( newX > 0 ) {
			Video::PreDraw();

			oldX -= dx;
			newX -= dx;

			// Prevent the screen from going to far and then 'bouncing' back
			if( newX < 0 ) {
				oldX -= newX;
				newX = 0;
			}

			Video::Erase();
			//printf("Ticks: %d old %d new %d total %d\n", SDL_GetTicks(), oldX, newX, newX+oldX);

			oldBackground->DrawStretch( oldX, 0, OPTION( int, "options/video/w" ), OPTION( int, "options/video/h"));
			Image::Get("Resources/Art/logo.png")->Draw(newX + Video::GetWidth() - 240, Video::GetHeight() - 120 );
			oldScreen->SetX( oldX );
			oldScreen->Draw( );
			DrawDeferred();

			newBackground->DrawStretch( newX, 0, OPTION( int, "options/video/w" ), OPTION( int, "options/video/h"));
			Image::Get("Resources/Art/logo.png")->Draw(newX + Video::GetWidth() - 240, Video::GetHeight() - 120 );
			newScreen->SetX( newX );
			newScreen->Draw( );
			DrawDeferred();
			
			Video::PostDraw();
			Video::Update();
			Timer::Delay(10);
			Timer::Update();
		}
		oldScreen->SetX( 0 );
		newScreen->SetX( 0 );
	}
}

/**\brief Handles Input Events from the event queue.
 * \details
 * List of events is passed from main input handler.
 * We remove the events we handle and leave the rest
 * to be handled by the next input handler.
 * The order of input handlers is in input.cpp.
 */
void UI::HandleInput( list<InputEvent> & events ) {
	// Go through all input events to see if they apply to any top-level widget. top-level widgets
	// (like windows) will then take the input and pass it to any children (like the ok button in the window)
	// where appropriate
	list<InputEvent>::iterator i = events.begin();
	while( i != events.end() ){
		bool eventWasHandled = false;
	
		switch( i->type ) {
			case KEY:
				eventWasHandled = UI::HandleKeyboard( *i );
				break;
			case MOUSE:
				eventWasHandled = UI::HandleMouse( *i );
				break;
		}

		if( eventWasHandled ) {
			i = events.erase( i );
		} else {
			i++;
		}
	}

	// On Escape, close the top Widget
	Widget* topContainer = UI::currentScreen->ChildFromTop(0, WIDGET_CONTAINER);
	if( topContainer != NULL ) {
		if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) ) {
			UI::Close( topContainer );
		}
	}
}

Container* UI::NewScreen( string name ) {
	Container* screen = new Container(name, false);
	// The currentScreen Container contains all other Widgets
	screen->SetX( 0 );
	screen->SetY( 0 );
	screen->SetW( Video::GetWidth() );
	screen->SetH( Video::GetHeight() );
	screen->ResetInput();
	return screen;
}

/**\brief Handles UI keyboard events.*/
bool UI::HandleKeyboard( InputEvent i ) {
	if(i.kstate == KEYTYPED) {
		// Attempt to send Messages to Floating Widgets
		list<draw_location>::iterator iter;
		for( iter = hovering.begin(); iter != hovering.end(); ++iter)
		{
			if( IsAttached( iter->widget ) )
			{
				if( (iter->widget)->KeyPress( i.key ) )
					return true;
			}
		}
		return UI::currentScreen->KeyPress( i.key );
	}
	return false;
}

/**\brief Handles UI mouse events.
 */
bool UI::HandleMouse( InputEvent i ) {
	// Attempt to send Messages to Floating Widgets
	list<draw_location>::iterator iter;
	for( iter = hovering.begin(); iter != hovering.end(); ++iter)
	{
		if( IsAttached( iter->widget ) && (iter->widget)->Contains(i.mx,i.my) )
		{
			if( DispatchMouse( iter->widget, i ) )
				return true;
		}
	}
	return DispatchMouse( UI::currentScreen, i );
}

bool UI::DispatchMouse( Widget* widget, InputEvent i ) {
	int x, y;
	
	// mouse coordinates associated with the mouse event
	x = i.mx;
	y = i.my;
	
	switch(i.mstate) {
		case MOUSEMOTION:		// Movement of the mouse
			return widget->MouseMotion( x, y );
		case MOUSELUP:			// Left button up
			return widget->MouseLUp( x, y );
		case MOUSELDOWN:		// Left button down
			return widget->MouseLDown( x, y );
		case MOUSEMUP:			// Middle button up
			return widget->MouseMUp( x, y );
		case MOUSEMDOWN:		// Middle button down
			return widget->MouseMDown( x, y );
		case MOUSERUP:			// Right button up
			return widget->MouseRUp( x, y );
		case MOUSERDOWN:		// Right button down
			return widget->MouseRDown( x, y );
		case MOUSEWUP:			// Scroll wheel up
			return widget->MouseWUp( x, y );
		case MOUSEWDOWN:		// Scroll wheel down
			return widget->MouseWDown( x, y );
		default:
			LogMsg(WARN, "Unhandled UI mouse input detected.");
		}

	return false;
}

void UI::ModalDialog( Container *widget ) {
	Input inputs;
	list<InputEvent> events;

	assert( modalEnabled == false );
	assert( backgroundScreen == NULL );
	modalEnabled = true;

	// Setup
	backgroundScreen = currentScreen;
	currentScreen = NewScreen("Foreground");
	currentScreen->AddChild( widget );

	// UI Input
	while( modalEnabled )
	{
		// Draw Things
		Video::Erase();
		Video::PreDraw();
		UI::Draw();
		Video::PostDraw();
		Video::Update();

		// Wait for some input
		Timer::Delay(75);

		// Collect user input events
		events = inputs.Update();
		UI::HandleInput( events );
	}

	// Cleanup
	Close( currentScreen );
	delete currentScreen;
	currentScreen = backgroundScreen;
	backgroundScreen = NULL;

	// Update the Timer so that the time spent inside the Modal widget does not
	// count as 'Lag Time'.
	Timer::Update();
}

void UI::ReleaseModality() {
	UI::modalEnabled = false;
}

void AddImage( void*widget, void*image, int x, int y )
{
	Container* container = ((Container*)widget);
	container->AddChild( new Picture( x, y, (Image*)image ) );
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
			->AddChild( (new Tab("Some Inputs"))
				->AddChild( (new Textbox(30, 30, 100, 2, "Some Text\nGoes Here", "A Textbox")) )
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
	assert( NULL == UI::Search("/[fdsa]/") );
	assert( NULL == UI::Search("/(foo,bar)/") );
	assert( NULL == UI::Search("/WindowWindow/") );
	assert( NULL == UI::Search("/'foobar'/") );
	//assert( NULL == UI::Search("/[]/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int
	//assert( NULL == UI::Search("/(,)/") ); // TODO: This should fail but doesn't. The empty string is converted to an Int

	// Check that Malformed Queries Fail and return nothing.
	// Malformed Indexes
	assert( NULL == UI::Search("/[/") );
	assert( NULL == UI::Search("/]/") );
	assert( NULL == UI::Search("/[1)/") );
	assert( NULL == UI::Search("/[100,100]/") );
	// Malformed Coordinates
	assert( NULL == UI::Search("/(/") );
	assert( NULL == UI::Search("/)/") );
	assert( NULL == UI::Search("/,/") );
	assert( NULL == UI::Search("/(100 100)/") );
	assert( NULL == UI::Search("/(100,100]/") );
	// Malformed Strings
	assert( NULL == UI::Search("/Window'/") );
	assert( NULL == UI::Search("/Window\"/") );
	assert( NULL == UI::Search("/'Window/") );
	assert( NULL == UI::Search("/\"Window/") );

	// Set a test Form button
	((Tab*)( UI::Search("/'A Window'/'TEST TABS'/Tab'Some Inputs'/"))) ->SetFormButton(
		(Button*) UI::Search("/'A Window'/'TEST TABS'/Tab'Some Inputs'/Button'Dummy'/")
	);
}

void ModalityTest() {
	Window* window = new Window( Video::GetWidth()/2-150, Video::GetHeight()/2-150, 300, 300, "Dialog" );
	window->AddChild( (new Button(100, 135, 100, 30, "Release", UI::ReleaseModality )) );
	UI::ModalDialog( window );
}

/** @} */
