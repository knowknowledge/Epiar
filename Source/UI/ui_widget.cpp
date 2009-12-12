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
		(*i)->Draw( x + relx, y + rely );
	}
}

void Widget::Update( void ) {

}

bool Widget::Contains(int relx, int rely) {
	bool insideLeftBorder = ( x < relx );
	bool insideRightBorder = ( relx < x+this->GetWidth() );
	bool insideTopBorder = 	( y < rely );  // Remember that the origin (0,0) is in the UPPER left, not lower left
	bool insideBottomBorder = ( rely < y+this->GetHeight() );
	return insideLeftBorder && insideRightBorder && insideTopBorder && insideBottomBorder;
}

// returns a widget if there is a child widget of this widget that was clicked on
Widget *Widget::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::iterator i;

	//cout << "widget determine mouse focus at " << x << ", " << y << endl;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->Contains(relx,rely) )
			return (*i);
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
		//cout << "mouse down on child "<< down_on->GetName() <<" of "<<GetName() << endl;
		down_on->MouseDown( x, y );
	} else {
		//cout << "mouse NOT down on child of "<<GetName() << endl;
	}
}

// when a widget loses focus, so do all of its children
void Widget::Unfocus( void ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		(*i)->Unfocus();
	}
}
