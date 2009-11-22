/*
 * Filename      : UI/ui_lua.cpp
 * Author(s)     : Matt Zweig (thezweig@gmail.com)
 * Date Created  : Thursday, October 29, 2009
 * Last Modified : Friday, November 14, 2009
 * Purpose       : Lua bridge for UI objects
 * Notes         :
 */
  
#include "UI/ui.h"
#include "ui_lua.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"
#include "ui_picture.h"

#include "Engine/models.h"

void UI_Lua::RegisterUI(lua_State *L){
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
	luaL_newmetatable(L, EPIAR_UI);
	luaL_openlib(L, EPIAR_UI, uiFunctions,0);  
}

int UI_Lua::newWindow(lua_State *L){
	int arg;
	int n = lua_gettop(L);  // Number of arguments
	if (n < 6)
		return luaL_error(L, "Got %d arguments expected 6 (class, x, y, w, h, caption, [ Widgets ... ])", n);

	int x = int(luaL_checknumber (L, 2));
	int y = int(luaL_checknumber (L, 3));
	int w = int(luaL_checknumber (L, 4));
	int h = int(luaL_checknumber (L, 5));
	string caption = luaL_checkstring (L, 6);

	// Allocate memory for a pointer to object
	Window **win = (Window**)lua_newuserdata(L, sizeof(Window*));
	*win = new Window(x,y,w,h,caption);
	//(*win)->AddChild( new Button( 152, 262, 96, 25, "OK" ) );

	// Add this Window
	UI::Add(*win);

	// Collect 'extra' widgets and Add them as children
	for(arg=7; arg<=n;arg++){
		Widget** widget= (Widget**)lua_touserdata(L,arg);
		(*win)->AddChild(*widget);
	}
	
	return 1;
}

int UI_Lua::close(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 1) {
		UI::Close();
	}
	else if(n == 2) {
		Widget** window= (Widget**)lua_touserdata(L,2);
		UI::Close(*window);
	}
	else {
		luaL_error(L, "Got %d arguments expected 1 or 2 (class, [window])", n); 
	}
	return 0;
}

int UI_Lua::newButton(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( (n != 6) && (n != 7) )
		return luaL_error(L, "Got %d arguments expected 6 or 7 (class, x, y, w, h, caption [Lua_code])", n);

	int x = int(luaL_checknumber (L, 2));
	int y = int(luaL_checknumber (L, 3));
	int w = int(luaL_checknumber (L, 4));
	int h = int(luaL_checknumber (L, 5));
	string caption = luaL_checkstring (L, 6);
	string code = "";
	if(n==7) code = luaL_checkstring (L, 7);

	// Allocate memory for a pointer to object
	Button **button= (Button**)lua_newuserdata(L, sizeof(Button*));
	*button = new Button(x,y,w,h,caption,code);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newTextbox(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( n != 6 )
		return luaL_error(L, "Got %d arguments expected 6 (class, x, y, w, h, text)", n);

	int x = int(luaL_checknumber (L, 2));
	int y = int(luaL_checknumber (L, 3));
	int w = int(luaL_checknumber (L, 4));
	int h = int(luaL_checknumber (L, 5));
	string text = luaL_checkstring (L, 6);
	string code = "";
	if(n==7) code = luaL_checkstring (L, 7);

	// Allocate memory for a pointer to object
	Textbox **textbox = (Textbox**)lua_newuserdata(L, sizeof(Textbox*));
	*textbox = new Textbox(x, y, w, h, text);

	// Note: We're not putting this button anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newLabel(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ((n != 4) && (n != 5))
		return luaL_error(L, "Got %d arguments expected 4 or 5 (class, x, y, caption, [centered] )", n);

	int x = int(luaL_checknumber (L, 2));
	int y = int(luaL_checknumber (L, 3));
	string caption = luaL_checkstring (L, 4);

	// Allocate memory for a pointer to object
	Label **label= (Label**)lua_newuserdata(L, sizeof(Label*));
	if(n==4){
		*label = new Label(x,y,caption);
	} else if(n==5) {
		bool centered = bool(luaL_checknumber (L, 5));
		*label = new Label(x,y,caption,centered);
	}

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::newPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 6)
		return luaL_error(L, "Got %d arguments expected 6 (class, x, y, w, h, modelname )", n);

	int x = int(luaL_checknumber (L, 2));
	int y = int(luaL_checknumber (L, 3));
	int w = int(luaL_checknumber (L, 4));
	int h = int(luaL_checknumber (L, 5));
	string modelname = luaL_checkstring (L, 6);

	Models *models = Models::Instance();

	// Allocate memory for a pointer to object
	Picture **pic= (Picture**)lua_newuserdata(L, sizeof(Picture*));
	*pic = new Picture(x,y,w,h, models->GetModel(modelname)->GetImage() );

	// Note: We're not putting this Label anywhere!
	//       Lua will have to do that for us.
	//       This may be a bad idea (memory leaks from bad lua scripts)

	return 1;
}

int UI_Lua::setPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2){
		Picture** pic = (Picture**)lua_touserdata(L,1);
		string modelname = luaL_checkstring (L, 2);
		(*pic)->Set( Models::Instance()->GetModel(modelname)->GetImage() );
	
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, ModelName)", n); 
	}
	return 0;
}

int UI_Lua::add(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2){
		Widget** ptrOuter = (Widget**)lua_touserdata(L,1);
		Widget** ptrInner = (Widget**)lua_touserdata(L,2);
		(*ptrOuter)->AddChild(*ptrInner);
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, widget)", n); 
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

	Label **label= (Label**)lua_touserdata(L,1);
	string text = luaL_checkstring(L, 2);
	(*label)->setText(text);

	return 1;
}
