/**\file			ui_action.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#include "UI/ui_action.h"

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

//

VoidAction::VoidAction( void (*callback)() )
{
	c_callback = callback;
}

void VoidAction::Activate( int x, int y )
{
	c_callback();
}

ObjectAction::ObjectAction(  void (*function)(void*), void* value )
{
	clickCallBack = function;
	object = value;
}

void ObjectAction::Activate( int x, int y )
{
	clickCallBack(object);
}

MessageAction::MessageAction(  void (*function)(void*,void*), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
	//printf("function %p obj %p msg %p\n", clickCallBack, object, message);
}

void MessageAction::Activate( int x, int y )
{
	clickCallBack(object, message);
	//printf("function %p obj %p msg %p\n", clickCallBack, object, message);
}

PositionalAction::PositionalAction( void (*function)(void* obj, void*msg, int x, int y), void* obj, void* msg )
{
	clickCallBack = function;
	object = obj;
	message = msg;
	//printf("function %p obj %p msg %p\n", clickCallBack, object, message);
}

void PositionalAction::Activate( int x, int y )
{
	clickCallBack(object, message, x, y);
	//printf("function %p obj %p msg %p at (%d,%d)\n", clickCallBack, object, message, x, y);
}

/** @} */
