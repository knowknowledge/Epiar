/*
 * Filename      : ui_widget.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#include "ui.h"

bool Widget::AddChild( Widget *widget ) {
	if( !widget) return false;
	
	children.push_back( widget );
	
	return true;
}

void Widget::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		cout << "drawing a widget child, i'm at " << x << ", " << y << ", telling child to be at " << (x + relx) << ", " << (y - rely) << endl;
		(*i)->Draw( x + relx, y - rely );
	}
}

void Widget::Update( void ) {

}

// returns a widget if there is a child widget of this widget that was clicked on
Widget *Widget::DetermineMouseFocus( int x, int y ) {
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
						return (*i);
					}
				}
			}
		}
	}
	
	return( NULL );
}
