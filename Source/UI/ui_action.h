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
		virtual void Activate( int x, int y ) = 0;
};

class LuaAction : public Action {
	public:
		LuaAction( string callback );
		void Activate( int x, int y );
	private:
		string lua_callback;
};

class LuaPositionalAction : public Action {
	public:
		LuaPositionalAction( string callback );
		void Activate( int x, int y );
	private:
		string lua_callback;
};

class VoidAction : public Action {
	public:
		VoidAction( void (*callback)() );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)();
};

class ObjectAction : public Action {
	public:
		ObjectAction( void (*function)(void*), void* value );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*);
		void *object;
};

class MessageAction : public Action {
	public:
		MessageAction( void (*function)(void*,void*), void* obj, void* msg );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*,void*);
		void *object;
		void *message;
};

class PositionalAction : public Action {
	public:
		PositionalAction ( void (*function)(void* obj, void*msg, int x, int y), void* obj, void* msg );
		void Activate( int x, int y );
	private:
		void (*clickCallBack)(void*,void*,int,int);
		void *object;
		void *message;
};

#endif // __H_ACTION__

