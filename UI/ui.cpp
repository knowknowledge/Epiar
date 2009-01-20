/*
 * Filename      : ui.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "Graphics/video.h"
#include "includes.h"
#include "UI/ui.h"

list<Widget *> UI::children;
Widget *UI::mouseFocus, *UI::keyboardFocus; // remembers which widgets last had these focuses

UI::UI() {
	mouseFocus = NULL;
	keyboardFocus = NULL;
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
	Widget *child = children.front();
	children.pop_front();
	
	// Free all widgets
	while( child ) {
		delete child;
		
		child = children.front();
		children.pop_front();
	}
	
	Video::DisableMouse();
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
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		int wx, wy, w, h;
		
		wx = (*i)->GetX();
		wy = (*i)->GetY();
		w = (*i)->GetWidth();
		h = (*i)->GetHeight();
		
		if( x > wx ) {
			if( y < wy ) {
				if( x < (wx + w ) ) {
					if( y > (wy - h ) ) {
						cout << "mouse focus is on a widget" << endl;
						return (*i);
					}
				}
			}
		}
	}
	
	cout << "mouse cannot focus on any widget" << endl;
	return( NULL );
}

// list of events is passed from main input handler. we remove the events we handle and leave the rest to be handled
// by the next input handler. the order of input handlers is in input.cpp
void UI::HandleInput( list<InputEvent> & events ) {
	list<InputEvent>::iterator i;
	
	// Go through all input events to see if they apply to any top-level widget. top-level widgets
	// (like windows) will then take the input and pass it to any children (like the ok button in the window)
	// where appropriate
	for( i = events.begin(); i != events.end(); ++i ) {
		switch( i->type ) {
		case KEY:
			
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
					mouseFocus->SetY( y + dy );
					
					// remove input from list, as we've handled it
					events.remove( *i );
					i = events.begin(); // removing elements mid-list upsets iterators
				}
				break;
			case MOUSEUP:
				// release focus if needed
				if( mouseFocus ) {
					// let the focused widget know it's no longer focused
					mouseFocus->Unfocus();
					mouseFocus = NULL;

					// remove input from list, as we've handled it
					events.remove( *i );
					i = events.begin(); // removing elements mid-list upsets iterators
				}
				break;
			case MOUSEDOWN:
				Widget *mouseFocusWidget = DetermineMouseFocus( x, y );
				
				// did they click a different widget than the one already in mouse focus?
				if( mouseFocus != mouseFocusWidget ) {
					// A new widget now has mouse focus
					if( mouseFocus )
						mouseFocus->Unfocus();
					
					mouseFocus = mouseFocusWidget;
					
					if( mouseFocus ) {
						mouseFocus->Focus( x, y );
					}
				}
				
				// pass the event to the widget
				if( mouseFocus ) {
					mouseFocus->MouseDown( x - mouseFocus->GetX(), mouseFocus->GetY() - y );
					
					// remove input from list, as we've handled it
					events.remove( *i );
					i = events.begin(); // removing elements mid-list upsets iterators					
				}
				break;
			}
			break;
		}
	}
}

void ui_demo( void ) {
	Window *wnd = new Window( 200, 700, 500, 400, "UI Demo" );
	UI::Add( wnd );
	wnd->AddChild( new Button( 50, 10, 100, 25, "Hello" ) );
	
}
