/**\file			ui_widget.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "ui.h"
#include "Utilities/log.h"

/**\class Widget
 * \brief Widgets.
 * \fn Widget::Draw()
 *  \brief Empty function that should be overloaded for drawing the widget.
 * \fn Widget::Update()
 *  \brief Empty function that should be overloaded for drawing the widget.
 */


/**\brief Constructor.
 */
Widget::Widget( void ):
	x( 0 ),y( 0 ),w( 0 ),h( 0 ),
	dragX( 0 ),dragY( 0 ),
	name( "RandomWidget" ),keyactivated( false ),
	keyboardFocus( NULL ),mouseHover( NULL ),
	lmouseDown( NULL ), mmouseDown( NULL ), rmouseDown( NULL ){
}

/**\brief Destroys this widget and all children.
 * \todo Implement a Widget::Hide routine that doesn't destroy children.
 * \bug This will cause a segfault on statically allocated widget children
 */
Widget::~Widget( void ){
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}
}

/**\brief Adds a child to the current widget.
 */
bool Widget::AddChild( Widget *widget ) {
	if( !widget)
		return false;
	children.push_back( widget );
	return true;
}

/**\brief Deletes a child from the current widget.
 */
bool Widget::DelChild( Widget *widget ){
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ){
			delete (*i);
			this->children.erase( i );
			return true;
		}
	}
	return false;
}

/**\brief Empties all children.*/
bool Widget::Empty( void ){
	list<Widget *>::iterator i;
	for( i = this->children.begin(); 
			i != this->children.end(); ++i ) {
		delete (*i);
	}
	this->children.clear();
}

/**\brief Tests if point is within a rectangle.
 */
bool Widget::Contains(int relx, int rely) {
	return WITHIN_BOUNDS(relx,rely,this->x,this->y,this->w,this->h);
}

/**\brief Checks to see if point is inside a child
 */
Widget *Widget::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->Contains(relx, rely) ) {
			return (*i);
		}
	}

	return( NULL );
}

/**\brief Draws this widget and all children widgets.
 */
void Widget::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); i++ )
		(*i)->Draw( this->x + relx, this->y + rely );
}

/**\brief Widget is currently being dragged.
 */
bool Widget::MouseDrag( int xi,int yi ){
	Log::Message("Mouse drag detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	if( this->lmouseDown )
		this->lmouseDown->MouseDrag( xr,yr );
	return true;
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool Widget::MouseMotion( int xi, int yi ){
	Log::Message("Mouse motion detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( !event_on ){
		// Not on a widget
		if( this->mouseHover ){
			// We were on a widget, send leave event
			this->mouseHover->MouseLeave();
			this->mouseHover=NULL;
		}
		return true;
	}
	if( !this->mouseHover ){
		// We're on a widget, but nothing was hovered on before
		// send enter event only
		event_on->MouseEnter( xr,yr );
		this->mouseHover=event_on;
		return true;
	}
	// We're on a widget, and leaving another widget
	// send both enter and leave event
	this->mouseHover->MouseLeave();
	event_on->MouseEnter( x,y );
	this->mouseHover=event_on;
	return true;
}

/**\brief Event is triggered on mouse enter.
 */
bool Widget::MouseEnter( int xi,int yi ){
	Log::Message("Mouse enter detect in %s.",this->name.c_str());
	// Don't do anything by default
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Widget::MouseLeave( void ){
	Log::Message("Mouse leave detect in %s.",this->name.c_str());
	// Don't do anything by default
	return true;
}


/**\brief Generic mouse up function.
 */
bool Widget::MouseLUp( int xi, int yi ){
	Log::Message("Mouse Left up detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( this->lmouseDown ){
		if( this->lmouseDown == event_on )
			// Mouse up is on the same widget as the ouse down, send event
			event_on->MouseLUp( xr,yr );
	}
	this->lmouseDown = NULL;
	return true;
}

/**\brief Generic mouse down function.
 */
bool Widget::MouseLDown( int xi, int yi ) {
	Log::Message("Mouse Left down detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	// update drag coordinates in case this is draggable
	dragX = xr;
	dragY = yr;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( !event_on  ){
		// Nothing was clicked on
		if( this->keyboardFocus )
			this->keyboardFocus->KeyboardLeave();
		this->keyboardFocus = NULL;
		return true;
	}
	// We clicked on a widget
	event_on->MouseLDown( xr, yr );
	this->lmouseDown = event_on;
	if( (this->keyboardFocus) && (this->keyboardFocus != event_on) ){
		// We changed keyboard focus
		this->keyboardFocus->KeyboardLeave();
		event_on->KeyboardEnter();
	}
	this->keyboardFocus = event_on;
	return true;
}

/**\brief Generic middle mouse up function.
 */
bool Widget::MouseMUp( int xi, int yi ){
	Log::Message("Mouse Middle up detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->mmouseDown ){
		if( this->mmouseDown == event_on )
			// Mouse up is on the same widget as mouse down, send event
			event_on->MouseMUp( xr, yr );
	}
	this->mmouseDown = NULL;
	return true;
}

/**\brief Generic middle mouse down function.
 */
bool Widget::MouseMDown( int xi, int yi ){
	Log::Message("Mouse Middle down detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		event_on->MouseMDown( xr, yr );
		this->mmouseDown=event_on;
	}
	return true;
}

/**\brief Generic right mouse up function.
 */
bool Widget::MouseRUp( int xi, int yi ){
	Log::Message("Mouse Right up detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->rmouseDown ){
		if( this->rmouseDown == event_on )
			// Mouse up is on the same widget as mouse down, send event
			event_on->MouseRUp( xr, yr );
	}
	this->mmouseDown = NULL;
	return true;
}

/**\brief Generic right mouse down function.
 */
bool Widget::MouseRDown( int xi, int yi ){
	Log::Message("Mouse Right down detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		event_on->MouseMDown( xr, yr );
		this->mmouseDown=event_on;
	}
	return true;
}

/**\brief Generic mouse wheel up function.
 */
bool Widget::MouseWUp( int xi, int yi ){
	Log::Message("Mouse Wheel up detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on )
		event_on->MouseWUp( xr,yr );
	return true;
}

/**\brief Generic mouse wheel down function.
 */
bool Widget::MouseWDown( int xi, int yi ){
	Log::Message("Mouse Wheel down detect in %s.",this->name.c_str());
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on )
		event_on->MouseWDown( xr,yr );
	return true;
}

/**\brief Generic keyboard focus function.
 */
bool Widget::KeyboardEnter( void ){
	Log::Message("Keyboard enter detect in %s.",this->name.c_str());
	this->keyactivated=true;
	if( this->keyboardFocus )
		this->keyboardFocus->KeyboardEnter();
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Widget::KeyboardLeave( void ){
	Log::Message("Keyboard leave detect in %s.",this->name.c_str());
	this->keyactivated=false;
	if( this->keyboardFocus )
		this->keyboardFocus->KeyboardLeave();
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Widget::KeyPress( SDLKey key ) {
	Log::Message("Key press detect in %s.",this->name.c_str());
	if( keyboardFocus ) return keyboardFocus->KeyPress( key );
	return false;
}
