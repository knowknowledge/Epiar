/*
 * Filename      : UI/ui_lua.h
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Friday, November 14, 2009
 * Purpose       : Lua bridge for UI objects
 * Notes         :
 */

#ifndef __H_UI_LUA
#define __H_UI_LUA

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "UI/ui.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"

class UI_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterUI(lua_State *luaVM);

		// Functions to create new Widgets
		static int newWindow(lua_State *luaVM);
		static int newButton(lua_State *luaVM);
		static int newTextbox(lua_State *luaVM);
		static int newLabel(lua_State *luaVM);
		static int newPicture(lua_State *luaVM);

		// Functions to manipulate Widgets
		static int add(lua_State *luaVM);
		static int close(lua_State *luaVM);
		static int rotatePicture(lua_State *luaVM);
		static int setText(lua_State *luaVM);
		static int setPicture(lua_State *luaVM);
	private:
};
#endif /*__H_UI_LUA*/