/**\file			ui.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "Graphics/video.h"
#include "common.h"
#include "Utilities/log.h"
#include "UI/ui.h"

// for ui_demo()
#include "Input/input.h"
#include "Utilities/timer.h"

/**\class UI
 * \brief UI. */

list<Widget *> UI::children;
Widget UI::master;
Widget	*UI::keyboardFocus=NULL,
		*UI::mouseHover=NULL,
		*UI::lmouseDown=NULL,
		*UI::mmouseDown=NULL,
		*UI::rmouseDown=NULL;

/**\brief This constructor resets the input.
 */
UI::UI() {
	ResetInput();
}


/**\brief Destroys the UI interface and all UI elements.
 */
UI::~UI() {
}

/**\brief Checks to see if there are UI elements.
 */
bool UI::Active( void ) {
	if( UI::master.IsEmpty() ) return false;
	return true;
}

/**\brief Adds a Widget to the base UI.*/
bool UI::Add( Widget *widget ) {
	if( !widget ) return false;
	
	UI::master.AddChild( widget );
	
	return true;
}

/**\brief Update all widgets.*/
void UI::Run( void ) {
	list<Widget *>::iterator i;
	
	if( !UI::master.IsEmpty() ){
		Video::EnableMouse();
		UI::master.Update();
	}
}

/**\brief This removes all widgets from the base.*/
void UI::Close( void ) {
	UI::master.Empty();
	ResetInput();
}

void UI::Close( Widget *widget ) {
	UI::master.DelChild( widget );
	ResetInput();
}

void UI::Draw( void ) {
	UI::master.Draw( );
}

/**\brief Handles Input Events from the event queue.
 * \details
 * List of events is passed from main input handler.
 * We remove the events we handle and leave the rest 
 * to be handled by the next input handler.
 * The order of input handlers is in input.cpp.
 */
void UI::HandleInput( list<InputEvent> &events ) {
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
}

/**\brief Handles UI keyboard events.*/
bool UI::HandleKeyboard( InputEvent &i ){
	bool handled=false;
	switch(i.kstate) {
		case KEYTYPED:
			if( keyboardFocus ) {
				handled = keyboardFocus
					->KeyPress( i.key );
			}
			break;
		
		default:
			break;
	}
	return handled;
}

/**\brief Handles UI mouse events.*/
bool UI::HandleMouse( InputEvent &i ){
	int x, y;
	
	// mouse coordinates associated with the mouse event
	x = i.mx;
	y = i.my;
	// Determine where mouse is on
	Widget *widgetOn = DetermineMouseFocus( x, y );
	
	switch(i.mstate) {
		case MOUSEMOTION:			// Movement of the mouse
			if ( UI::lmouseDown ){
				// Mouse button is held down, send drag event
				UI::lmouseDown->MouseDrag(x,y);
				return true;
			}
			// Mouse is moving without button down
			if ( !widgetOn ){
				// Not on a widget
				if ( UI::mouseHover ){
					// We were on a widget, send leave event
					UI::mouseHover->MouseLeave();
					UI::mouseHover=NULL;
				}
				return true;
			}
			if ( !UI::mouseHover ){
				// We're on a widget, but nothing was hovered on before
				// send enter event.
				widgetOn->MouseEnter(x,y);
				UI::mouseHover = widgetOn;
				return true;
			}
			// We're on a widget, and leaving another widget
			// send both enter and leave event
			UI::mouseHover->MouseLeave();
			widgetOn->MouseEnter( x,y );
			UI::mouseHover = widgetOn;
			return true;
		case MOUSELUP:			// Left button up
			if( UI::lmouseDown ){
				if( UI::lmouseDown == widgetOn )
				// Mouse up is on the same widget as mouse down, send event
					widgetOn->MouseLUp( x, y );
			}
			UI::lmouseDown = NULL;
			return true;
		case MOUSELDOWN:		// Left button down
			if( !widgetOn ){
				// Nothing was clicked on
				if( UI::keyboardFocus )
					UI::keyboardFocus->KeyboardLeave();
				UI::keyboardFocus = NULL;
				return true;
			}
			// We clicked on a widget
			widgetOn->MouseLDown( x,y );
			UI::lmouseDown = widgetOn;
			if ( (UI::keyboardFocus) && (UI::keyboardFocus != widgetOn) ){
				// We changed keyboard focus
				UI::keyboardFocus->KeyboardLeave();
				widgetOn->KeyboardEnter();
			}
			UI::keyboardFocus = widgetOn;
			return true;
		case MOUSEMUP:			// Middle button up
			if( UI::mmouseDown ){
				if( UI::mmouseDown == widgetOn )
				// Mouse up is on the same widget as mouse down, send event
					widgetOn->MouseMUp( x, y );
			}
			UI::mmouseDown = NULL;
			return true;
		case MOUSEMDOWN:		// Middle button down
			if ( widgetOn ){
				widgetOn->MouseMDown( x,y );
				UI::mmouseDown = widgetOn;
			}
			return true;
		case MOUSERUP:			// Right button up
			if( UI::rmouseDown ){
				if( UI::rmouseDown == widgetOn )
				// Mouse up is on the same widget as mouse down, send event
					widgetOn->MouseRUp( x, y );
			}
			UI::rmouseDown = NULL;
			break;
		case MOUSERDOWN:
			if ( widgetOn ){	// Right button down
				widgetOn->MouseRDown( x,y );
				UI::rmouseDown = widgetOn;
			}
			return true;
		case MOUSEWUP:			// Scroll wheel up
			if( widgetOn )
				widgetOn->MouseWUp( x, y );
			break;
		case MOUSEWDOWN:		// Scroll wheel down
			if( widgetOn )
				widgetOn->MouseWDown( x, y );
			break;
		default:
			Log::Warning("Unhandled UI input detected.");
		}
	return false;
}



// Clears current input to prevent accidental usage of invalid values
// Use this whenever the UI removes focusable widgets
void UI::ResetInput() {
	keyboardFocus=NULL;
	mouseHover=NULL;
	lmouseDown=NULL;
	mmouseDown=NULL;
	rmouseDown=NULL;
}

void UI::RegisterKeyboardFocus( Widget *widget ) {
	keyboardFocus = widget;
}

