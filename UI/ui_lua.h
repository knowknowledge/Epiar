/*
 * Filename      : UI/ui_lua.h
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Thursday, October 29, 2009
 * Purpose       : Lua bridge for UI objects
 * Notes         :
 */

#ifndef __H_UI_LUA
#define __H_UI_LUA

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "UI/ui.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"

class UI_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterUI(lua_State *luaVM);

		static int newWindow(lua_State *luaVM);
		static int newButton(lua_State *luaVM);
		static int newLabel(lua_State *luaVM);
		static int add(lua_State *luaVM);
		static int close(lua_State *luaVM);
	private:
};
#endif /*__H_UI_LUA*/
