/**\file			ui_action.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#include "UI/ui_action.h"

LuaAction::LuaAction( string callback )
{
	lua_callback = callback;
}

void LuaAction::Activate()
{
	Lua::Run( lua_callback );
}

//

VoidAction::VoidAction( void (*callback)() )
{
	c_callback = callback;
}

void VoidAction::Activate()
{
	c_callback();
}

ObjectAction::ObjectAction(  void (*function)(void*), void* value )
{
	clickCallBack = function;
	object = value;
}

void ObjectAction::Activate()
{
	clickCallBack(object);
}
