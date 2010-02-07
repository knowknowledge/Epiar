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
Widget *UI::mouseFocus, *UI::keyboardFocus; // remembers which widgets last had focus

UI::UI() {
	ResetInput();
}

UI::~UI() {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}
	
	children.clear();
}

bool UI::Add( Widget *widget ) {
	if( !widget ) return false;
	
	children.push_back( widget );
	
	return true;
}

void UI::Run( void ) {
	list<Widget *>::iterator i;
	
	if( children.size() )
		Video::EnableMouse();
	
	// Update all widgets
	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Update();
	}
}

void UI::Close( void ) {
	// Free all widgets
	std::for_each(children.begin(), children.end(), create_delete_functor());
	children.clear();
	ResetInput();
}

void UI::Close( Widget *widget ) {
	children.remove(widget);
	ResetInput();
}

void UI::Draw( void ) {
	list<Widget *>::iterator i;
	
	// Draw all widgets
	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Draw();
	}
}

// Uses the child list and (x,y) to determine over which widget (x,y) is
// If widgets overlap, the one first added will have priority per behavior of STL list template
Widget *UI::DetermineMouseFocus( int x, int y ) {
	list<Widget *>::reverse_iterator i;

	for( i = children.rbegin(); i != children.rend(); ++i ) {
		int wx, wy, w, h;
		
		wx = (*i)->GetX();
		wy = (*i)->GetY();
		w = (*i)->GetWidth();
		h = (*i)->GetHeight();
		
		if( x > wx ) {
			if( y > wy ) {
				if( x < (wx + w ) ) {
					if( y < (wy + h ) ) {
						return (*i);
					}
				}
			}
		}
	}
	
	return( NULL );
}

// list of events is passed from main input handler. we remove the events we handle and leave the rest to be handled
// by the next input handler. the order of input handlers is in input.cpp
void UI::HandleInput( list<InputEvent> & events ) {
	// Go through all input events to see if they apply to any top-level widget. top-level widgets
	// (like windows) will then take the input and pass it to any children (like the ok button in the window)
	// where appropriate
	list<InputEvent>::iterator i = events.begin();
	while( i != events.end() )
	{
		bool eventWasHandled = false;
	
		switch( i->type ) {
		case KEY:
		
			switch(i->kstate) {
				case KEYTYPED:
					if( keyboardFocus ) { 
						bool handled = keyboardFocus->KeyPress( i->key );
						
						// if the input was handled, we need to remove it from the queue so no other
						// subsystem sees it and acts on it
						if( handled ) {
							events.erase(i);
							i = events.begin();
						}
					}
					break;
				
				default:
					break;
			}

			break;
		case MOUSE:
			int x, y;
			
			// mouse coordinates associated with the mouse event
			x = i->mx;
			y = i->my;

			switch(i->mstate) {
			case MOUSEMOTION:
				// if a widget has mouse focus, we are dragging
				if( mouseFocus ) {
					int dx, dy; // original point of click for the drag
					
					dx = mouseFocus->GetDragX();
					dy = mouseFocus->GetDragY();

					mouseFocus->SetX( x - dx );
					mouseFocus->SetY( y - dy );
					
					eventWasHandled = true;
				}
				break;
			case MOUSEUP:
				// release focus if needed
				if( mouseFocus ) {
					// let the focused widget know it's no longer focused
					mouseFocus->UnfocusMouse();
					mouseFocus = NULL;

					eventWasHandled = true;
				}
				break;
			case MOUSEDOWN:
				Widget *focusedWidget = DetermineMouseFocus( x, y );
				
				// did they click a different widget than the one already in focus?
				if( mouseFocus != focusedWidget ) {
					// A new widget now has focus
					if( mouseFocus )
						mouseFocus->UnfocusMouse();
					
					mouseFocus = focusedWidget;
					
					if( mouseFocus ) {
						mouseFocus->FocusMouse( x - mouseFocus->GetX(), y - mouseFocus->GetY() );
					}
				}
				// mouse down also changes keyboard focus (e.g. clicked on a new text field)
				if( keyboardFocus != focusedWidget ) {
					if( keyboardFocus ) keyboardFocus->UnfocusKeyboard();
					
					keyboardFocus = focusedWidget;
					
					if( keyboardFocus ) {
						keyboardFocus->FocusKeyboard();
					}
				}
				
				// pass the event to the widget
				if( mouseFocus ) {
					mouseFocus->MouseDown( x - mouseFocus->GetX(), y - mouseFocus->GetY() );
					
					eventWasHandled = true;				
				}
				break;
			}
			break;
		}

		if( eventWasHandled ) {
			i = events.erase( i );
		} else {
			i++;
		}
	}
}

// Clears current input to prevent accidental usage of invalid values
// Use this whenever the UI removes focusable widgets
void UI::ResetInput() {
	mouseFocus = NULL;
	keyboardFocus = NULL;
}

void UI::RegisterKeyboardFocus( Widget *widget ) {
	keyboardFocus = widget;
}

void ui_demo( bool in_loop ) {
	static bool ran_once = false;

	if(ran_once) return;
	
	ran_once = true; // don't make more than one window

	Window *wnd = new Window( 200, 100, 400, 300, "User Interface Demo" );
	UI::Add( wnd );
	wnd->AddChild( new Button( 152, 262, 96, 25, "OK" ) );
	wnd->AddChild( new Textbox( 50, 50, 100, 1 ) );
	wnd->AddChild( new Checkbox( 50, 100, 0, "Toggle This" ) );
}

