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
#include "ui_slider.h"
#include "Engine/models.h"

/**\class UI_Lua
 * \brief Lua bridge fro UI. */

void UI_Lua::RegisterUI(lua_State *L){
	// These Functions create new UI Elements
	// Call them like:
	// win = UI.newWindow( ... )
	static const luaL_Reg uiFunctions[] = {
		// Creation
		{"newWindow", &UI_Lua::newWindow},
		{"newButton", &UI_Lua::newButton},
		{"newLabel", &UI_Lua::newLabel},
		{"newPicture", &UI_Lua::newPicture},
		{"newTextbox", &UI_Lua::newTextbox},
		{"newCheckbox", &UI_Lua::newCheckbox},
		{"newSlider", &UI_Lua::newSlider},
		{NULL, NULL}
	};

	// These Functions inspect or modify UI Elements
	// Call them like:
	// win:add( ... )
	static const luaL_Reg uiMethods[] = {
		// Widget Getters
		{"IsChecked", &UI_Lua::IsChecked},
		{"GetText", &UI_Lua::GetText},

		// Widget Setters
		// Windowing Layout
		{"add", &UI_Lua::add},
		{"close", &UI_Lua::close},
		// Picture Modification
		{"rotatePicture", &UI_Lua::rotatePicture},
		{"setPicture", &UI_Lua::setPicture},
		// Label Modification
		{"setLabel", &UI_Lua::setLabel},
		{"setText", &UI_Lua::setText},
		// Checkbox Modification
		{"setChecked", &UI_Lua::setChecked},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_UI);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, uiMethods, 0);

	luaL_openlib(L, EPIAR_UI, uiFunctions, 0);
}

int UI_Lua::newWindow(lua_State *L){
	int arg;
	int n = lua_gettop(L);  // Number of arguments
	if (n < 5)
		return luaL_error(L, "Got %d arguments expected 5 (x, y, w, h, caption, [ Widgets ... ])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string caption = luaL_checkstring (L, 5);

	// Allocate memory for a pointer to object
	Window **win = (Window**)lua_newuserdata(L, sizeof(Window**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	*win = new Window(x,y,w,h,caption);
	//(*win)->AddChild( new Button( 152, 262, 96, 25, "OK" ) );

	// Add this Window
	UI::Add(*win);

	// Collect 'extra' widgets and Add them as children
	for(arg=6; arg<=n;arg++){
		Widget** widget= (Widget**)lua_touserdata(L,arg);
		(*win)->AddChild(*widget);
	}
	
	return 1;
}

int UI_Lua::close(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 0) {
		UI::Close();
	}
	else if(n == 1) {
		Widget** window= (Widget**)lua_touserdata(L,1);
		UI::Close(*window);
	}
	else {
		luaL_error(L, "Got %d arguments expected 0 or 1 ([window])", n); 
	}
	return 0;
}

int UI_Lua::newButton(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( (n != 5) && (n != 6) )
		return luaL_error(L, "Got %d arguments expected 5 or 6 (x, y, w, h, caption [Lua_code])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string caption = luaL_checkstring (L, 5);
	string code = "";
	if(n==6) code = luaL_checkstring (L, 6);

	// Allocate memory for a pointer to object
	Button **button= (Button**)lua_newuserdata(L, sizeof(Button**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	*button = new Button(x,y,w,h,caption,code);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newSlider(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5 )
		return luaL_error(L, "Got %d arguments expected 5 (x, y, w, h, label)", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string label = luaL_checkstring (L, 5);
	
	// Allocate memory for a pointer to object
	Slider **slider= (Slider**)lua_newuserdata(L, sizeof(Slider**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	*slider = new Slider(x,y,w,h,label);
	
	return 1;
}

int UI_Lua::newTextbox(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( n < 4  )
		return luaL_error(L, "Got %d arguments expected 3, 4, or 5 (x, y, w, h, [text], [code])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string code = "";
	string text = "";
	if(n>=5) text = luaL_checkstring (L, 5);
	if(n>=7) code = luaL_checkstring (L, 7);

	// Allocate memory for a pointer to object
	Textbox **textbox = (Textbox**)lua_newuserdata(L, sizeof(Textbox**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	*textbox = new Textbox(x, y, w, h, text);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newLabel(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ((n != 3) && (n != 4))
		return luaL_error(L, "Got %d arguments expected 3 or 4 (x, y, caption, [centered] )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	string caption = luaL_checkstring (L, 3);

	// Allocate memory for a pointer to object
	Label **label= (Label**)lua_newuserdata(L, sizeof(Label**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	if(n==3){
		*label = new Label(x,y,caption);
	} else if(n==4) {
		bool centered = luaL_checknumber (L, 4) != 0.;
		*label = new Label(x,y,caption,centered);
	}

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5)
		return luaL_error(L, "Got %d arguments expected 5 (x, y, w, h, modelname )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string picname = luaL_checkstring (L, 5);

	// Allocate memory for a pointer to object
	Picture **pic= (Picture**)lua_newuserdata(L, sizeof(Picture**));
    luaL_getmetatable(L, EPIAR_UI);
    lua_setmetatable(L, -2);
	*pic = new Picture(x,y,w,h, picname );

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newCheckbox(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 4){
		int x = int(luaL_checknumber (L, 1));
		int y = int(luaL_checknumber (L, 2));
		bool checked = luaL_checknumber(L, 3) != 0;
		string labeltext = luaL_checkstring (L, 4);
		
		Checkbox **checkbox = (Checkbox**)lua_newuserdata(L, sizeof(Checkbox**));
		luaL_getmetatable(L, EPIAR_UI);
		lua_setmetatable(L, -2);
		*checkbox = new Checkbox(x, y, checked, labeltext);
	} else {
		return luaL_error(L, "Got %d arguments expected 4 (x, y, chekced, labeltext)", n);
	}
	
	return 1;
}

int UI_Lua::setPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2){
		Picture** pic = (Picture**)lua_touserdata(L,1);
		string picname = luaL_checkstring (L, 2);
		(*pic)->Set( picname );
	
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, picname)", n);
	}
	return 0;
}

int UI_Lua::add(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n >= 2){
		Widget** ptrOuter = (Widget**)lua_touserdata(L,1);
		for(int i=2; i<=n; i++){
			Widget** ptrInner = (Widget**)lua_touserdata(L,i);
			(*ptrOuter)->AddChild(*ptrInner);
		}
	} else {
		luaL_error(L, "Got %d arguments expected 2 or more (self, widget [...])", n);
	}
	return 0;
}

int UI_Lua::rotatePicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, angle )", n);

	Picture **pic= (Picture**)lua_touserdata(L,1);
	double angle = luaL_checknumber (L, 2);
	(*pic)->Rotate(angle);

	return 1;
}

int UI_Lua::setText(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, text)", n);

	Textbox **box= (Textbox**)lua_touserdata(L,1);
	string text = luaL_checkstring(L, 2);
	(*box)->SetText(text);

	return 1;
}

int UI_Lua::setLabel(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, text)", n);

	Label **label= (Label**)lua_touserdata(L,1);
	string text = luaL_checkstring(L, 2);
	(*label)->setText(text);

	return 1;
}

int UI_Lua::IsChecked(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);

	Checkbox **box= (Checkbox**)lua_touserdata(L,1);
	lua_pushboolean(L, (int) (*box)->IsChecked() );

	return 1;
}

int UI_Lua::setChecked(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, value)", n);

	Checkbox **box= (Checkbox**)lua_touserdata(L,1);
	bool checked = lua_toboolean(L, 2) != 0;
	(*box)->Set(checked);

	return 0;
}

int UI_Lua::GetText(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);

	Textbox **box= (Textbox**)lua_touserdata(L,1);
	lua_pushstring(L, (*box)->GetText().c_str() );

	return 1;
}

