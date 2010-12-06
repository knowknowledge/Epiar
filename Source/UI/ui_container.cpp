/**\file			ui_container.cpp
 * \author			Maoserr
 * \date			Created: Saturday, March 27, 2010
 * \date			Modified: Saturday, March 27, 2010
 * \brief			Container object can contain other widgets.
 */

#include "includes.h"
#include "UI/ui.h"

/**\class Container
 * \brief Container is a container class for other widgets.
 */

/**\brief Constructor, initializes default values.*/
Container::Container( string _name, bool _mouseHandled ):
	mouseHandled( _mouseHandled ), keyboardFocus( NULL ),mouseHover( NULL ),
	lmouseDown( NULL ), mmouseDown( NULL ), rmouseDown( NULL ){
	name = _name;
}

/**\brief Destructor, destroys all children.
 * \todo Implement a Container::Hide routine that doesn't destroy children.
 * \bug This will cause a segfault on statically allocated widget children
 */
Container::~Container( void ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}

	children.clear();

	keyboardFocus = NULL;
	mouseHover = NULL;
	lmouseDown = NULL;
	mmouseDown = NULL;
	rmouseDown = NULL;
}

/**\brief Adds a child to the current container.
 */
Container *Container::AddChild( Widget *widget ) {
	assert( widget != NULL );
	if( widget != NULL ) {
		children.push_back( widget );
	}
	return this;
}

/**\brief Deletes a child from the current container.
 */
bool Container::DelChild( Widget *widget ){
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ) {
			// FIXME BROKEN Uncommenting this delete causes memory corruption crashes on MSVC 2008 and 2010. PLEASE FIX!
			//cout << "delchild called on " << (*i)->GetType() << endl;
			//delete (*i);
			i = children.erase( i );
			Reset();

			return true;
		}
	}

	return false;
}

/**\brief Empties all children.*/
bool Container::Empty( void ){
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}

	children.clear();
	Reset();

	return true;
}

/**\brief Reset focus and events.*/
bool Container::Reset( void ){
	this->keyboardFocus = NULL;
	this->mouseHover = NULL;
	this->lmouseDown = NULL;
	this->mmouseDown = NULL;
	this->rmouseDown = NULL;

	return true;
}

/**\brief Checks to see if point is inside a child
 *
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i)->Contains(relx, rely) ) {
			return (*i);
		}
	}
	return( NULL );
}

/**\brief Search for a child named
 *
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildNamed( string _name, int mask ) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( ( (*i)->GetName() == _name ) && ( (*i)->GetMask() & mask ) ) {
			return (*i);
		}
	}
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] pos position of the child
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildFromTop( int pos, int mask) {
	int p = 0;
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i)->GetMask() & mask ) {
			if( pos == p ) {
				return (*i);
			}
			++p;
		}
	}
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] pos position of the child
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildFromBottom( int pos, int mask) {
	int p = 0;
	list<Widget *>::iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->GetMask() & mask ) {
			if( pos == p ) {
				return (*i);
			}
			++p;
		}
	}
	return( NULL );
}
/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] widget don't start checking until this widget is found.
 * \param[in] mask Only consider Widgets of this type.
 * \note This checks the children in the order that they are drawn so that Children 'on top' are searched last.
 */
Widget *Container::NextChild( Widget* widget, int mask) {
	list<Widget *>::iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ) {
			// Found the previous widget, now keep going
			++i;
			for(; i != children.end(); ++i ) {
				if( (*i)->GetMask() & mask ) {
					return (*i);
				}
			}
			// Never found another Widget that matched the mask
			return( NULL );
		}
	}
	// Never found the previous Widget
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * \param[in] widget don't start checking until this widget is found.
 * \param[in] mask Only consider Widgets of this type.
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::PrevChild( Widget* widget, int mask) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i) == widget ) {
			// Found the previous widget, now keep going
			++i;
			for(; i != children.rend(); ++i ) {
				if( (*i)->GetMask() & mask ) {
					return (*i);
				}
			}
			// Never found another Widget that matched the mask
			return( NULL );
		}
	}
	// Never found the previous Widget
	return( NULL );
}

/**\brief Draws this widget and all children widgets.
 */
void Container::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); i++ )
		(*i)->Draw( this->x + relx, this->y + rely );
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool Container::MouseMotion( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if ( this->lmouseDown ){
		// Mouse button is held down, send drag event
		this->lmouseDown->MouseDrag(xr,yr);
	}

	if( !event_on ){
		// Not on a widget
		if( this->mouseHover ){
			// We were on a widget, send leave event
			this->mouseHover->MouseLeave();
			this->mouseHover=NULL;
		}
		return this->mouseHandled;
	}
	if( !this->mouseHover ){
		// We're on a widget, but nothing was hovered on before
		// send enter event only
		event_on->MouseEnter( xr,yr );
		this->mouseHover=event_on;
		return true;
	}
	if( this->mouseHover != event_on ){
		// We're on a widget, and leaving another widget
		// send both enter and leave event
		this->mouseHover->MouseLeave();
		event_on->MouseEnter( x,y );
		this->mouseHover=event_on;
	}

	event_on->MouseMotion( xr, yr );

	return true;
}

/**\brief Generic mouse up function.
 */
bool Container::MouseLUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( this->lmouseDown ){
		if (this->lmouseDown == event_on) {
			// Mouse up is on the same widget as the mouse down, send up event
			event_on->MouseLUp( xr,yr );
			this->lmouseDown = NULL;
			return true;
		}else{
			// Mouse up is on a different widget, send release event to old
			this->lmouseDown->MouseLRelease();
			this->lmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Left up detect in %s.",this->name.c_str());

	return this->mouseHandled;
}

/**\brief Generic mouse down function.
 */
bool Container::MouseLDown( int xi, int yi ) {
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	// update drag coordinates in case this is draggable
	dragX = xr;
	dragY = yr;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( !event_on  ){
		//LogMsg(INFO,"Mouse Left down detect in %s.",this->name.c_str());
		// Nothing was clicked on
		if( this->keyboardFocus )
			this->keyboardFocus->KeyboardLeave();
		this->keyboardFocus = NULL;
		return this->mouseHandled;
	}
	// We clicked on a widget
	event_on->MouseLDown( xr, yr );
	this->lmouseDown = event_on;
	if( !this->keyboardFocus )
		// No widget had keyboard focus before
		event_on->KeyboardEnter();
	else if( this->keyboardFocus != event_on ){
		// keyboard focus changed
		this->keyboardFocus->KeyboardLeave();
		event_on->KeyboardEnter();
	}
	this->keyboardFocus = event_on;

	return true;
}

/**\brief Generic mouse release function.
 * \details Unlike the MouseLUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseLRelease( void ){
	// Pass event onto children if needed
	if( this->lmouseDown )
		return this->lmouseDown->MouseLRelease();
	//LogMsg(INFO,"Left Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse up function.
 */
bool Container::MouseMUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->mmouseDown ){
		if ( this->mmouseDown == event_on ){
			// Mouse up is on the same widget as mouse down, send event
			this->mmouseDown = NULL;
			return event_on->MouseMUp( xr, yr );
		}else{
			// Mouse up is on a different widget, send release event to old
			this->mmouseDown->MouseMRelease( );
			this->mmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Middle up detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse down function.
 */
bool Container::MouseMDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		this->mmouseDown=event_on;
		return event_on->MouseMDown( xr, yr );
	}
	//LogMsg(INFO,"Mouse Middle down detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse release function.
 * \details Unlike the MouseMUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseMRelease( void ){
	// Pass event onto children if needed
	if( this->mmouseDown )
		return this->mmouseDown->MouseMRelease();
	//LogMsg(INFO,"Middle Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse up function.
 */
bool Container::MouseRUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->rmouseDown ){
		if ( this->rmouseDown == event_on ){
			// Mouse up is on the same widget as mouse down, send event
			this->rmouseDown = NULL;
			return event_on->MouseRUp( xr, yr );
		}else{
			// Mouse up is on a different widget, send release event to old
			this->rmouseDown->MouseRRelease();
			this->rmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Right up detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse down function.
 */
bool Container::MouseRDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		this->rmouseDown=event_on;
		return event_on->MouseRDown( xr, yr );
	}
	//LogMsg(INFO,"Mouse Right down detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse release function.
 * \details Unlike the MouseRUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseRRelease( void ){
	// Pass event onto children if needed
	if( this->rmouseDown )
		return this->rmouseDown->MouseRRelease();
	//LogMsg(INFO,"Right Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic mouse wheel up function.
 */
bool Container::MouseWUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on )
		return event_on->MouseWUp( xr,yr );
	//LogMsg(INFO,"Mouse Wheel up detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic mouse wheel down function.
 */
bool Container::MouseWDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on )
		return event_on->MouseWDown( xr,yr );
	//LogMsg(INFO,"Mouse Wheel down detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic keyboard focus function.
 */
bool Container::KeyboardEnter( void ){
	this->keyactivated=true;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardEnter();
	//LogMsg(INFO,"Keyboard enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Container::KeyboardLeave( void ){
	this->keyactivated=false;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardLeave();
	//LogMsg(INFO,"Keyboard leave detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Container::KeyPress( SDLKey key ) {
	Widget *next;
	if( keyboardFocus ) {
		
		// If this key is a TAB and the keyboard is currently focused on a Textbox,
		// then move to the next textbox
		if( (key == SDLK_TAB) && ( keyboardFocus->GetMask() & (WIDGET_TEXTBOX) ) ) {
			
			next = NextChild( keyboardFocus, WIDGET_TEXTBOX );
			if( next == NULL ) {
				// Wrap around to the top
				next = ChildFromBottom( 0, WIDGET_TEXTBOX );
			}
			if( next ) {
				keyboardFocus->KeyboardLeave();
				keyboardFocus = next;
				keyboardFocus->KeyboardEnter();
			}
			return true;
		}

		// Otherwise, pass the key Press normally
		return keyboardFocus->KeyPress( key );
	}
	//LogMsg(INFO,"Key press detect in %s.",this->name.c_str());
	return false;
}

xmlNodePtr Container::ToNode() {
	xmlNodePtr thisNode;
	char buff[256];

	thisNode = xmlNewNode(NULL, BAD_CAST this->GetType().c_str() );
	xmlSetProp( thisNode, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetX() );
	xmlSetProp( thisNode, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetY() );
	xmlSetProp( thisNode, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetW() );
	xmlSetProp( thisNode, BAD_CAST "w", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetH() );
	xmlSetProp( thisNode, BAD_CAST "h", BAD_CAST buff );

	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		xmlAddChild(thisNode, (*i)->ToNode()  );
	}
	return thisNode;

}

