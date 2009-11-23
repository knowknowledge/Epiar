/**\file			ui_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Lua bridge for UI objects
 * \details
 */

#include "includes.h"
#include "UI/ui.h"
#include "ui_lua.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"
#include "ui_picture.h"
#include "Engine/models.h"

/**\class UI_Lua
 * \brief Lua bridge fro UI. */

void UI_Lua::RegisterUI(lua_State *luaVM){
	static const luaL_Reg uiFunctions[] = {
		// Creation
		{"newWindow", &UI_Lua::newWindow},
		{"newButton", &UI_Lua::newButton},
		{"newLabel", &UI_Lua::newLabel},
		{"newPicture", &UI_Lua::newPicture},
		{"newTextbox", &UI_Lua::newTextbox},
		// Windowing Layout
		{"add", &UI_Lua::add},
		{"close", &UI_Lua::close},
		// Picture Modification
		{"rotatePicture", &UI_Lua::rotatePicture},
		{"setPicture", &UI_Lua::setPicture},
		// Label Modification
		{"setText", &UI_Lua::setText},
		{NULL, NULL}
	};
	luaL_newmetatable(luaVM, "EpiarLua.UI");
	luaL_openlib(luaVM, "EpiarLua.UI", uiFunctions,0);  
}

int UI_Lua::newWindow(lua_State *luaVM){
	int arg;
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n < 6)
		return luaL_error(luaVM, "Got %d arguments expected 6 (class, x, y, w, h, caption, [ Widgets ... ])", n);

	int x = int(luaL_checknumber (luaVM, 2));
	int y = int(luaL_checknumber (luaVM, 3));
	int w = int(luaL_checknumber (luaVM, 4));
	int h = int(luaL_checknumber (luaVM, 5));
	string caption = luaL_checkstring (luaVM, 6);

	// Allocate memory for a pointer to object
	Window **win = (Window**)lua_newuserdata(luaVM, sizeof(Window*));
	*win = new Window(x,y,w,h,caption);
	//(*win)->AddChild( new Button( 152, 262, 96, 25, "OK" ) );

	// Add this Window
	UI::Add(*win);

	// Collect 'extra' widgets and Add them as children
	for(arg=7; arg<=n;arg++){
		Widget** widget= (Widget**)lua_touserdata(luaVM,arg);
		(*win)->AddChild(*widget);
	}
	
	return 1;
}

int UI_Lua::close(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments

	if (n == 1) {
		UI::Close();
	}
	else if(n == 2) {
		Widget** window= (Widget**)lua_touserdata(luaVM,2);
		UI::Close(*window);
	}
	else {
		luaL_error(luaVM, "Got %d arguments expected 1 or 2 (class, [window])", n); 
	}
	return 0;
}

int UI_Lua::newButton(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if ( (n != 6) && (n != 7) )
		return luaL_error(luaVM, "Got %d arguments expected 6 or 7 (class, x, y, w, h, caption [Lua_code])", n);

	int x = int(luaL_checknumber (luaVM, 2));
	int y = int(luaL_checknumber (luaVM, 3));
	int w = int(luaL_checknumber (luaVM, 4));
	int h = int(luaL_checknumber (luaVM, 5));
	string caption = luaL_checkstring (luaVM, 6);
	string code = "";
	if(n==7) code = luaL_checkstring (luaVM, 7);

	// Allocate memory for a pointer to object
	Button **button= (Button**)lua_newuserdata(luaVM, sizeof(Button*));
	*button = new Button(x,y,w,h,caption,code);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newTextbox(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if ( n != 6 )
		return luaL_error(luaVM, "Got %d arguments expected 6 (class, x, y, w, h, text)", n);

	int x = int(luaL_checknumber (luaVM, 2));
	int y = int(luaL_checknumber (luaVM, 3));
	int w = int(luaL_checknumber (luaVM, 4));
	int h = int(luaL_checknumber (luaVM, 5));
	string text = luaL_checkstring (luaVM, 6);
	string code = "";
	if(n==7) code = luaL_checkstring (luaVM, 7);

	// Allocate memory for a pointer to object
	Textbox **textbox = (Textbox**)lua_newuserdata(luaVM, sizeof(Textbox*));
	*textbox = new Textbox(x, y, w, h, text);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newLabel(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 4)
		return luaL_error(luaVM, "Got %d arguments expected 4 (class, x, y, caption)", n);

	int x = int(luaL_checknumber (luaVM, 2));
	int y = int(luaL_checknumber (luaVM, 3));
	string caption = luaL_checkstring (luaVM, 4);

	// Allocate memory for a pointer to object
	Label **label= (Label**)lua_newuserdata(luaVM, sizeof(Label*));
	*label = new Label(x,y,caption);

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newPicture(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 6)
		return luaL_error(luaVM, "Got %d arguments expected 6 (class, x, y, w, h, caption )", n);

	int x = int(luaL_checknumber (luaVM, 2));
	int y = int(luaL_checknumber (luaVM, 3));
	int w = int(luaL_checknumber (luaVM, 4));
	int h = int(luaL_checknumber (luaVM, 5));
	string filename = luaL_checkstring (luaVM, 6);

	// Allocate memory for a pointer to object
	Picture **pic= (Picture**)lua_newuserdata(luaVM, sizeof(Picture*));
	*pic = new Picture(x,y,w,h,filename);

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::setPicture(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n == 2){
		Picture** pic = (Picture**)lua_touserdata(luaVM,1);
		string modelname = luaL_checkstring (luaVM, 2);
		(*pic)->Set( Models::Instance()->GetModel(modelname)->GetImage() );
	
	} else {
		luaL_error(luaVM, "Got %d arguments expected 2 (self, ModelName)", n); 
	}
	return 0;
}

int UI_Lua::add(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n == 2){
		Widget** ptrOuter = (Widget**)lua_touserdata(luaVM,1);
		Widget** ptrInner = (Widget**)lua_touserdata(luaVM,2);
		(*ptrOuter)->AddChild(*ptrInner);
	} else {
		luaL_error(luaVM, "Got %d arguments expected 2 (self, widget)", n); 
	}
	return 0;
}

int UI_Lua::rotatePicture(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 2)
		return luaL_error(luaVM, "Got %d arguments expected 2 (self, angle )", n);

	Picture **pic= (Picture**)lua_touserdata(luaVM,1);
	double angle = luaL_checknumber (luaVM, 2);
	(*pic)->Rotate(angle);

	return 1;
}

int UI_Lua::setText(lua_State *luaVM){
	int n = lua_gettop(luaVM);  // Number of arguments
	if (n != 2)
		return luaL_error(luaVM, "Got %d arguments expected 2 (self, text)", n);

	Label **label= (Label**)lua_touserdata(luaVM,1);
	string text = luaL_checkstring(luaVM, 2);
	(*label)->setText(text);

	return 1;
}
