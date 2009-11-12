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
#include "common.h"
#include "Utilities/log.h"
#include "UI/ui.h"
#include "Utilities/camera.h" //Temp for testing camera shaking

// for ui_demo()
#include "Input/input.h"
#include "Utilities/timer.h"

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
	Widget *child;
	
	// Free all widgets
	std::for_each(children.begin(), children.end(), create_delete_functor());
	children.clear();

	mouseFocus = NULL;
	keyboardFocus = NULL;
	
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
			if( y > wy ) {
				if( x < (wx + w ) ) {
					if( y < (wy + h ) ) {
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
	// Go through all input events to see if they apply to any top-level widget. top-level widgets
	// (like windows) will then take the input and pass it to any children (like the ok button in the window)
	// where appropriate
	list<InputEvent>::iterator i = events.begin();
	while( i != events.end() )
	{
		bool eventWasHandled = false;
	
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

					cout << "mouse motion, dx dy is " << dx << ", " << dy << endl;
					cout << "mouse focus will set x y to " << x - dx << ", " << y - dy << endl;

					mouseFocus->SetX( x - dx );
					mouseFocus->SetY( y - dy );
					
					eventWasHandled = true;
				}
				break;
			case MOUSEUP:
				// release focus if needed
				if( mouseFocus ) {
					// let the focused widget know it's no longer focused
					mouseFocus->Unfocus();
					mouseFocus = NULL;

					eventWasHandled = true;
				}
				break;
			case MOUSEDOWN:
				Widget *mouseFocusWidget = DetermineMouseFocus( x, y );
				
				Camera::Shake(100, 30);//Temp for testing camera shaking
					
				// did they click a different widget than the one already in mouse focus?
				if( mouseFocus != mouseFocusWidget ) {
					// A new widget now has mouse focus
					if( mouseFocus )
						mouseFocus->Unfocus();
					
					mouseFocus = mouseFocusWidget;
					
					if( mouseFocus ) {
						mouseFocus->Focus( x - mouseFocus->GetX(), y - mouseFocus->GetY() );
						cout << "setting mouse focus point to " << x - mouseFocus->GetX() << ", " << y - mouseFocus->GetY() << endl;
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

void ui_demo( bool in_loop ) {
	bool quit = false;
	Input inputs;

	Window *wnd = new Window( 200, 100, 400, 300, "UI Demo" );
	UI::Add( wnd );
	wnd->AddChild( new Button( 152, 262, 96, 25, "OK" ) );

	// main game loop
	while( !quit && in_loop ) {
		// Do this first, so that the frame capping works correctly
		Timer::Update();

		quit = inputs.Update();
		
		// Erase cycle
		Video::Erase();
		
		// Update cycle
		UI::Run(); // runs only a few loops
		
		// Draw cycle
		UI::Draw();
		Video::Update();
		
		// Don't kill the CPU (play nice)
		Timer::Delay();
	}
}

