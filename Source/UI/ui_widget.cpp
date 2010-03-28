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
 * \brief A user interface widget, widgets do not have children.
 * \fn Widget::Draw()
 *  \brief Empty function that should be overloaded for drawing the widget.
 * \fn Widget::Update()
 *  \brief Empty function that should be overloaded for drawing the widget.
 */


/**\brief Constructor.
 */
Widget::Widget( void ):
	hidden( false ), disabled( false ),
	x( 0 ),y( 0 ),w( 0 ),h( 0 ),
	dragX( 0 ),dragY( 0 ),
	name( "UnspecifiedWidget" ),keyactivated( false ){
}

/**\brief Tests if point is within a rectangle.
 */
bool Widget::Contains(int relx, int rely) {
	return WITHIN_BOUNDS(relx,rely,this->x,this->y,this->w,this->h);
}

/**\brief Widget is currently being dragged.
 */
bool Widget::MouseDrag( int xi,int yi ){
	return true;
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool Widget::MouseMotion( int xi, int yi ){
	return true;
}

/**\brief Event is triggered on mouse enter.
 */
bool Widget::MouseEnter( int xi,int yi ){
	Log::Message("Mouse enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Widget::MouseLeave( void ){
	Log::Message("Mouse leave detect in %s.",this->name.c_str());
	return true;
}


/**\brief Generic mouse up function.
 */
bool Widget::MouseLUp( int xi, int yi ){
	Log::Message("Mouse Left up detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic mouse down function.
 */
bool Widget::MouseLDown( int xi, int yi ) {
	return true;
}

/**\brief Generic mouse release function.
 */
bool Widget::MouseLRelease( void ){
	Log::Message("Left Mouse released in %s",this->name.c_str());
	return true;
}

/**\brief Generic middle mouse up function.
 */
bool Widget::MouseMUp( int xi, int yi ){
	Log::Message("Mouse Middle up detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic middle mouse down function.
 */
bool Widget::MouseMDown( int xi, int yi ){
	Log::Message("Mouse Middle down detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic middle mouse release function.
 */
bool Widget::MouseMRelease( void ){
	Log::Message("Middle Mouse released in %s",this->name.c_str());
	return true;
}

/**\brief Generic right mouse up function.
 */
bool Widget::MouseRUp( int xi, int yi ){
	Log::Message("Mouse Right up detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic right mouse down function.
 */
bool Widget::MouseRDown( int xi, int yi ){
	Log::Message("Mouse Right down detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic right mouse release function.
 */
bool Widget::MouseRRelease( void ){
	Log::Message("Right Mouse released in %s",this->name.c_str());
	return true;
}

/**\brief Generic mouse wheel up function.
 */
bool Widget::MouseWUp( int xi, int yi ){
	Log::Message("Mouse Wheel up detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic mouse wheel down function.
 */
bool Widget::MouseWDown( int xi, int yi ){
	Log::Message("Mouse Wheel down detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard focus function.
 */
bool Widget::KeyboardEnter( void ){
	Log::Message("Keyboard enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Widget::KeyboardLeave( void ){
	Log::Message("Keyboard leave detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Widget::KeyPress( SDLKey key ) {
	Log::Message("Key press detect in %s.",this->name.c_str());
	return true;
}
