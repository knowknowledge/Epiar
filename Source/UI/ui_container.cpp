/**\file			ui_container.cpp
 * \author			Maoserr
 * \date			Created: Saturday, March 27, 2010
 * \date			Modified: Saturday, March 27, 2010
 * \brief			Container object can contain other widgets.
 */

#include "includes.h"
#include "UI/ui.h"

/**\class UIContainer
 * \brief UIContainer is a container class for other widgets.
 */

/**\brief Constructor, initializes default values.*/
UIContainer::UIContainer( string _name, bool _mouseHandled ):
	mouseHandled( _mouseHandled ), keyboardFocus( NULL ),mouseHover( NULL ),
	lmouseDown( NULL ), mmouseDown( NULL ), rmouseDown( NULL ){
	name = _name;
}

/**\brief Destructor, destroys all children.
 * \todo Implement a UIContainer::Hide routine that doesn't destroy children.
 * \bug This will cause a segfault on statically allocated widget children
 */
UIContainer::~UIContainer( void ) {
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
Widget *UIContainer::AddChild( Widget *widget ) {
	if( !widget)
		return NULL;
	children.push_back( widget );
	return widget;
}

/**\brief Deletes a child from the current container.
 */
bool UIContainer::DelChild( Widget *widget ){
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
bool UIContainer::Empty( void ){
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}

	children.clear();
	Reset();

	return true;
}

/**\brief Reset focus and events.*/
bool UIContainer::Reset( void ){
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
Widget *UIContainer::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i)->Contains(relx, rely) ) {
			return (*i);
		}
	}
	return( NULL );
}

/**\brief Draws this widget and all children widgets.
 */
void UIContainer::Draw( int relx, int rely ) {
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); i++ )
		(*i)->Draw( this->x + relx, this->y + rely );
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool UIContainer::MouseMotion( int xi, int yi ){
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
bool UIContainer::MouseLUp( int xi, int yi ){
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
bool UIContainer::MouseLDown( int xi, int yi ) {
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
bool UIContainer::MouseLRelease( void ){
	// Pass event onto children if needed
	if( this->lmouseDown )
		return this->lmouseDown->MouseLRelease();
	//LogMsg(INFO,"Left Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse up function.
 */
bool UIContainer::MouseMUp( int xi, int yi ){
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
bool UIContainer::MouseMDown( int xi, int yi ){
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
bool UIContainer::MouseMRelease( void ){
	// Pass event onto children if needed
	if( this->mmouseDown )
		return this->mmouseDown->MouseMRelease();
	//LogMsg(INFO,"Middle Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse up function.
 */
bool UIContainer::MouseRUp( int xi, int yi ){
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
bool UIContainer::MouseRDown( int xi, int yi ){
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
bool UIContainer::MouseRRelease( void ){
	// Pass event onto children if needed
	if( this->rmouseDown )
		return this->rmouseDown->MouseRRelease();
	//LogMsg(INFO,"Right Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic mouse wheel up function.
 */
bool UIContainer::MouseWUp( int xi, int yi ){
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
bool UIContainer::MouseWDown( int xi, int yi ){
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
bool UIContainer::KeyboardEnter( void ){
	this->keyactivated=true;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardEnter();
	//LogMsg(INFO,"Keyboard enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool UIContainer::KeyboardLeave( void ){
	this->keyactivated=false;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardLeave();
	//LogMsg(INFO,"Keyboard leave detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool UIContainer::KeyPress( SDLKey key ) {
	if( keyboardFocus ) 
		return keyboardFocus->KeyPress( key );
	//LogMsg(INFO,"Key press detect in %s.",this->name.c_str());
	return false;
}

xmlNodePtr UIContainer::ToNode() {
	xmlNodePtr thisNode;
	thisNode = xmlNewNode(NULL, BAD_CAST this->GetType().c_str() );
	xmlSetProp( thisNode, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		xmlAddChild(thisNode, (*i)->ToNode()  );
	}
	return thisNode;

}

