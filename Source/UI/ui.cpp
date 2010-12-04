/**\file			ui.cpp
 * \author			Chris Thielen (chris@epiar.net)
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

UIContainer UI::master("Master", false);

/**\brief This constructor resets the input.
 */
UI::UI() {
	//UI::master = UIContainer("Master", false);
	ResetInput();
}


/**\brief Destroys the UI interface and all UI elements.
 */
UI::~UI() {
	UI::Close();
}

/**\brief Checks to see if there are UI elements.
 */
bool UI::Active( void ) {
	if( UI::master.IsEmpty() ) return false;
	return true;
}

/**\brief Adds a Widget to the base UI.
 * \param[in] widget Widget pointer that should be added.
 * \returns The same pointer to the widget that was passed as input.
 */
Widget *UI::Add( Widget *widget ) {
	if( !widget ) return false;
	
	return UI::master.AddChild( widget );
}

/**\brief This removes all widgets from the base.*/
void UI::Close( void ) {
	UI::master.Empty();
	ResetInput();
}

/**\brief This removes a single base widget.*/
void UI::Close( Widget *widget ) {
	UI::master.DelChild( widget );
}

/**\brief Drawing function.*/
void UI::Draw( void ) {
	UI::master.Draw( );
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

	xmlAddChild( root_node, UI::master.ToNode() );

	xmlSaveFormatFileEnc( "Master_UI.xml" , doc, "ISO-8859-1", 1);
	xmlFreeDoc( doc );
}

/**\brief Handles Input Events from the event queue.
 * \details
 * List of events is passed from main input handler.
 * We remove the events we handle and leave the rest 
 * to be handled by the next input handler.
 * The order of input handlers is in input.cpp.
 */
void UI::HandleInput( list<InputEvent> *events ) {
	// Go through all input events to see if they apply to any top-level widget. top-level widgets
	// (like windows) will then take the input and pass it to any children (like the ok button in the window)
	// where appropriate
	list<InputEvent>::iterator i = events->begin();
	while( i != events->end() ){
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
			i = events->erase( i );
		} else {
			i++;
		}
	}
}

/**\brief Handles UI keyboard events.*/
bool UI::HandleKeyboard( InputEvent i ) {
	switch(i.kstate) {
		case KEYTYPED:
			return UI::master.KeyPress( i.key );
		default:
			return false;
	}
}

/**\brief Handles UI mouse events.*/
bool UI::HandleMouse( InputEvent i ) {
	int x, y;
	
	// mouse coordinates associated with the mouse event
	x = i.mx;
	y = i.my;
	
	switch(i.mstate) {
		case MOUSEMOTION:		// Movement of the mouse
			return UI::master.MouseMotion( x, y );
		case MOUSELUP:			// Left button up
			return UI::master.MouseLUp( x, y );
		case MOUSELDOWN:		// Left button down
			return UI::master.MouseLDown( x, y );
		case MOUSEMUP:			// Middle button up
			return UI::master.MouseMUp( x, y );
		case MOUSEMDOWN:		// Middle button down
			return UI::master.MouseMDown( x, y );
		case MOUSERUP:			// Right button up
			return UI::master.MouseRUp( x, y );
		case MOUSERDOWN:		// Right button down
			return UI::master.MouseRDown( x, y );
		case MOUSEWUP:			// Scroll wheel up
			return UI::master.MouseWUp( x, y );
		case MOUSEWDOWN:		// Scroll wheel down
			return UI::master.MouseWDown( x, y );
		default:
			LogMsg(WARN, "Unhandled UI mouse input detected.");
		}

	return false;
}



// Clears current input to prevent accidental usage of invalid values
// Use this whenever the UI removes focusable widgets
void UI::ResetInput() {
	UI::master.Reset();
}

