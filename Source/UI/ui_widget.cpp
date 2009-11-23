/**\file			ui_widget.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "ui.h"

/**\class Widget
 * \brief Widgets. */

bool Widget::AddChild( Widget *widget ) {
	if( !widget) return false;
	
	children.push_back( widget );
	
	return true;
}

void Widget::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Draw( x + relx, y + rely );
	}
}

void Widget::Update( void ) {

}

// returns a widget if there is a child widget of this widget that was clicked on
Widget *Widget::DetermineMouseFocus( int x, int y ) {
	list<Widget *>::iterator i;

	//cout << "widget determine mouse focus at " << x << ", " << y << endl;

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
						return (*i);
					}
				}
			}
		}
	}

	return( NULL );
}

void Widget::Focus( int x, int y ) {
	// update drag coordinates in case this is draggable
	dragX = x;
	dragY = y;

	//cout << "widget focus on " << x << ", " << y << endl;
	
	Widget *mouseFocus = DetermineMouseFocus( x, y );
	
	if(mouseFocus) mouseFocus->Focus( x - mouseFocus->GetX(), y - mouseFocus->GetY() );
}

void Widget::MouseDown( int x, int y ) {
	//cout << "mouse down event on widget, relative at " << x << ", " << y << endl;
	Widget *down_on = DetermineMouseFocus( x, y );
	if(down_on) {
		//cout << "mouse down on child of widget" << endl;
		down_on->MouseDown( x, y );
	} else {
		//cout << "mouse NOT down on child of widget" << endl;
	}
}

// when a widget loses focus, so do all of its children
void Widget::Unfocus( void ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Unfocus();
	}
}
