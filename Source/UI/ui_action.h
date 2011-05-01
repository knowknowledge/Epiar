/**\file			ui_action.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Saturday, April 30, 2011
 * \date			Modified: Saturday, April 30, 2011
 * \brief
 * \details
 */

#ifndef __H_ACTION__
#define __H_ACTION__

#include "includes.h"
#include "Utilities/lua.h"

class Action {
	public:
		virtual void Activate() = 0;
};

class LuaAction : public Action {
	public:
		LuaAction( string callback );
		void Activate();
	private:
		string lua_callback;
};

class VoidAction : public Action {
	public:
		VoidAction( void (*callback)() );
		void Activate();
	private:
		void (*c_callback)();
};

class ObjectAction : public Action {
	public:
		ObjectAction( void (*function)(void*), void* value );
		void Activate();
	private:
		void (*clickCallBack)(void*);
		void *object;
};

#endif // __H_ACTION__

