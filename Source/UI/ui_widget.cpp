/**\file			ui_widget.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "ui.h"
#include "Utilities/log.h"
#include "Graphics/video.h"

/** \addtogroup UI
 * @{
 */

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
	name( "UnspecifiedWidget" ),
	hovering( false ),
	hidden( false ),
	disabled( false ),
	keyactivated( false ),
	x( 0 ), y( 0 ),
	w( 0 ), h( 0 ),
	dragX( 0 ), dragY( 0 ),
	parent( NULL )
{
}

void Widget::Draw( int relx, int rely ) {
	if( hovering && OPTION(int,"options/development/debug-ui") ) {
		int absx, absy;
		char xbuff[6];
		char ybuff[6];
		absx = x + relx;
		absy = y + rely;
		Video::DrawBox( absx, absy, w, h, GREEN, 1 );
		Video::DrawLine( relx, absy, absx, absy, RED, 1 );
		Video::DrawLine( absx, rely, absx, absy, RED, 1 );

		snprintf( xbuff, sizeof(xbuff), "%d", x );
		snprintf( ybuff, sizeof(xbuff), "%d", y );

		Video::DrawRect( relx + x/2, y, UI::font->TextWidth(xbuff), UI::font->LineHeight(), BLACK, 1 );
		UI::font->RenderTight( relx + x/2, y, xbuff );

		Video::DrawRect( absx, rely + y/2, UI::font->TextWidth(ybuff), UI::font->LineHeight(), BLACK, 1 );
		UI::font->RenderTight( absx, rely + y/2, ybuff );
	}
}

/**\brief Tests if point is within a rectangle.
 */
int Widget::GetAbsX( void ) {
	int absx = GetX();
	Widget* theParent = parent;
	while( theParent != NULL ) {
		assert( theParent->GetMask() & WIDGET_CONTAINER );
		absx += theParent->GetX();
		theParent = theParent->parent;
	}
	return absx;
}

/**\brief Tests if point is within a rectangle.
 */
int Widget::GetAbsY( void ) {
	int absy = GetY();
	Widget* theParent = parent;
	while( theParent != NULL ) {
		assert( theParent->GetMask() & WIDGET_CONTAINER );
		absy += theParent->GetY();
		theParent = theParent->parent;
	}
	return absy;
}

/**\brief Tests if point is within a rectangle.
 */
bool Widget::Contains(int relx, int rely) {
	return WITHIN_BOUNDS(relx, rely, this->x, this->y, this->w, this->h);
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
bool Widget::MouseEnter( int xi, int yi ){
	hovering = true;
	LogMsg(INFO,"Mouse enter detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Widget::MouseLeave( void ){
	hovering = false;
	LogMsg(INFO,"Mouse leave detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic mouse up function.
 */
bool Widget::MouseLUp( int xi, int yi ){
	LogMsg(INFO,"Mouse Left up detect in %s named %s.", GetType().c_str(), GetName().c_str() );
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
	LogMsg(INFO,"Left Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic middle mouse up function.
 */
bool Widget::MouseMUp( int xi, int yi ){
	LogMsg(INFO,"Mouse Middle up detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic middle mouse down function.
 */
bool Widget::MouseMDown( int xi, int yi ){
	LogMsg(INFO,"Mouse Middle down detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic middle mouse release function.
 */
bool Widget::MouseMRelease( void ){
	LogMsg(INFO,"Middle Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic right mouse up function.
 */
bool Widget::MouseRUp( int xi, int yi ){
	LogMsg(INFO,"Mouse Right up detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic right mouse down function.
 */
bool Widget::MouseRDown( int xi, int yi ){
	LogMsg(INFO,"Mouse Right down detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic right mouse release function.
 */
bool Widget::MouseRRelease( void ){
	LogMsg(INFO,"Right Mouse released in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}

/**\brief Generic mouse wheel up function.
 */
bool Widget::MouseWUp( int xi, int yi ){
	LogMsg(INFO,"Mouse Wheel up detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return false;
}

/**\brief Generic mouse wheel down function.
 */
bool Widget::MouseWDown( int xi, int yi ){
	LogMsg(INFO,"Mouse Wheel down detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return false;
}

/**\brief Generic keyboard focus function.
 */
bool Widget::KeyboardEnter( void ){
	LogMsg(INFO,"Keyboard enter detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	keyactivated = true;
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Widget::KeyboardLeave( void ){
	LogMsg(INFO,"Keyboard leave detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	keyactivated = false;
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Widget::KeyPress( SDLKey key ) {
	LogMsg(INFO,"Key press detect in %s named %s.", GetType().c_str(), GetName().c_str() );
	return true;
}


xmlNodePtr Widget::ToNode() {
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
	snprintf(buff, sizeof(buff), "%p", this );
	xmlSetProp( thisNode, BAD_CAST "address", BAD_CAST buff );

	if(hidden) xmlSetProp( thisNode, BAD_CAST "hidden", BAD_CAST "true" );
	if(disabled) xmlSetProp( thisNode, BAD_CAST "disabled", BAD_CAST "true" );
	
	return thisNode;
}

/** @} */
