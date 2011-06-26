/**\file			ui_action.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#include "includes.h"
#include "UI/ui_action.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Action
 * \brief A way to register generic callbacks to user actions on widgets
 * \details
 * Each Action type defines a different type of Callback.
 * The Callback will be called whenever the Widget Activates the Action.
 * Actions are our way of creating more full featured first class lambda functions.
 * \warn Actions use void* pointers.
 *       The programmer needs to make sure not to incorrectly cast the inputs.
 *       While useful, Actions make it easy to shoot yourself in the foot.
 * \warn Avoid using NULL arguments for Callback values.
 *       It's better to just create a new Action type than to try to force an
 *       existing Action type to fit your workload.
 *       This will help the comiler check your work.
 */

/** \class LuaAction
 *  \brief Action to activate preformatted lua code.
 */
LuaAction::LuaAction( string callback )
{
	lua_callback = callback;
}

/** \brief Activate preformatted lua code.
 */
void LuaAction::Activate( int x, int y )
{
	Lua::Run( lua_callback );
}

/** \class LuaPositionalAction
 *  \brief Action to activate lua code that accepts a position
 */
LuaPositionalAction::LuaPositionalAction( string callback )
{
	lua_callback = callback;
}

/** \brief Activate lua code that accepts a position
 */
void LuaPositionalAction::Activate( int x, int y )
{
	Lua::Call( lua_callback.c_str(), "ii", x, y );
}

/** \class VoidAction
 *  \brief Action to call a void(void) callback
 * \details This callback should be completely self contained, and only access Globals.
 */
VoidAction::VoidAction( void (*callback)() )
{
	clickCallBack = callback;
}

/** \brief Call a void(void) callback
 */
void VoidAction::Activate( int x, int y )
{ if( clickCallBack ) {
		clickCallBack();
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

/** \class ObjectAction
 *  \brief Action to call a void(void*) callback
 * \details This callback accepts one void pointer.
 */
ObjectAction::ObjectAction(  void (*function)(void*), void* value )
{
	clickCallBack = function;
	object = value;
}

/** \brief Call a void(void*) callback
 */
void ObjectAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack(object);
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

/** \class MessageAction
 *  \brief Action to call a void(void*,void*) callback
 * \details This callback accepts two void pointers.
 *          Useful for calling class instance functions.
 */
MessageAction::MessageAction(  void (*function)(void*,void*), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
}

/** \brief Call a void(void*,void*) callback
 */
void MessageAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack(object, message);
		//LogMsg(INFO,"function %p obj %p msg %p\n", clickCallBack, object, message);
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

/** \class PositionalAction
 *  \brief Action to call a void(void*,void*,int,int) callback
 * \details This callback accepts two void pointers and an x,y location.
 *          Useful for detecting the position of mouse clicks in a Widget.
 */
PositionalAction::PositionalAction( void (*function)(void* obj, void*msg, int x, int y), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
}

/** \brief Call a void(void*,void*,int,int) callback
 */
void PositionalAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack(object, message, x, y);
		//LogMsg(INFO,"function %p obj %p msg %p at (%d,%d)\n", clickCallBack, object, message, x, y);
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

/** @} */
