/**\file			ui_action.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#include "UI/ui_action.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

LuaAction::LuaAction( string callback )
{
	lua_callback = callback;
}

void LuaAction::Activate( int x, int y )
{
	Lua::Run( lua_callback );
}

VoidAction::VoidAction( void (*callback)() )
{
	clickCallBack = callback;
}

void VoidAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack();
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

ObjectAction::ObjectAction(  void (*function)(void*), void* value )
{
	clickCallBack = function;
	object = value;
}

void ObjectAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack(object);
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

MessageAction::MessageAction(  void (*function)(void*,void*), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
}

void MessageAction::Activate( int x, int y )
{
	if( clickCallBack ) {
		clickCallBack(object, message);
		//LogMsg(INFO,"function %p obj %p msg %p\n", clickCallBack, object, message);
	} else {
		LogMsg(WARN,"Attempting to activate a NULL Action." );
	}
}

PositionalAction::PositionalAction( void (*function)(void* obj, void*msg, int x, int y), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
}

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
