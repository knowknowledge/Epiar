/**\file			ui_lua.cpp
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Lua bridge for UI objects
 * \details
 */

#include "includes.h"
#include "UI/ui.h"
#include "UI/ui_lua.h"
#include "UI/widgets.h"
#include "Engine/simulation_lua.h"

/** \class UI_Lua
 *  \brief Lua bridge for working with the UI.
 *  \todo Widgets are being stored as plain userdata pointers.
 *        This is very unsafe as it allows Lua to attempt to use deallocated memory.
 *  \todo The Widget interface should be better defined.
 *        There should be more utility functions to allow creative uses of the UI.
 */

void UI_Lua::RegisterUI(lua_State *L){
	// Register Actions
	Lua::RegisterGlobal("Action_MouseDrag", Action_MouseDrag);
	Lua::RegisterGlobal("Action_MouseMotion", Action_MouseMotion);
	Lua::RegisterGlobal("Action_MouseEnter", Action_MouseEnter);
	Lua::RegisterGlobal("Action_MouseLeave", Action_MouseLeave);
	Lua::RegisterGlobal("Action_MouseLUp", Action_MouseLUp);
	Lua::RegisterGlobal("Action_MouseLDown", Action_MouseLDown);
	Lua::RegisterGlobal("Action_MouseLRelease", Action_MouseLRelease);
	Lua::RegisterGlobal("Action_MouseMUp", Action_MouseMUp);
	Lua::RegisterGlobal("Action_MouseMDown", Action_MouseMDown);
	Lua::RegisterGlobal("Action_MouseMRelease", Action_MouseMRelease);
	Lua::RegisterGlobal("Action_MouseRUp", Action_MouseRUp);
	Lua::RegisterGlobal("Action_MouseRDown", Action_MouseRDown);
	Lua::RegisterGlobal("Action_MouseRRelease", Action_MouseRRelease);
	Lua::RegisterGlobal("Action_MouseWUp", Action_MouseWUp);
	Lua::RegisterGlobal("Action_MouseWDown", Action_MouseWDown);
	Lua::RegisterGlobal("Action_KeyboardEnter", Action_KeyboardEnter);
	Lua::RegisterGlobal("Action_KeyboardLeave", Action_KeyboardLeave);
	Lua::RegisterGlobal("Action_Close", Action_Close);

	// These Functions create new UI Elements
	// Call them like:
	// win = UI.newWindow( ... )
	static const luaL_Reg uiFunctions[] = {
		// Create Widgets
		{"newWindow", &UI_Lua::newWindow},
		{"newFrame", &UI_Lua::newFrame},
		{"newButton", &UI_Lua::newButton},
		{"newLabel", &UI_Lua::newLabel},
		{"newPicture", &UI_Lua::newPicture},
		{"newTextbox", &UI_Lua::newTextbox},
		{"newTextarea", &UI_Lua::newTextarea},
		{"newCheckbox", &UI_Lua::newCheckbox},
		{"newSlider", &UI_Lua::newSlider},
		{"newTabContainer", &UI_Lua::newTabContainer},
		{"newTab", &UI_Lua::newTab},
		{"newDropdown", &UI_Lua::newDropdown},
		{"newParagraph", &UI_Lua::newParagraph},
		{"newMap", &UI_Lua::newMap},

		// Create Modal Dialogs
		{"newConfirm", &UI_Lua::newConfirm},
		{"newAlert", &UI_Lua::newAlert},

		{"add", &UI_Lua::addWidget},
		{"search", &UI_Lua::search},
		{NULL, NULL}
	};

	// These Functions inspect or modify UI Elements
	// Call them like:
	// win:add( ... )
	static const luaL_Reg uiMethods[] = {
		// Widget Getters
		{"IsChecked", &UI_Lua::IsChecked},
		{"GetText", &UI_Lua::GetText},
		{"GetEdges", &UI_Lua::GetEdges},
		{"GetX", &UI_Lua::GetX},
		{"GetY", &UI_Lua::GetY},
		{"GetW", &UI_Lua::GetW},
		{"GetH", &UI_Lua::GetH},

		// Generic Widget Modification
		{"move", &UI_Lua::move},
		{"SetX", &UI_Lua::SetX},
		{"SetY", &UI_Lua::SetY},
		{"SetW", &UI_Lua::SetW},
		{"SetH", &UI_Lua::SetH},
		{"close", &UI_Lua::close},
		{"addCallback", &UI_Lua::AddCallback},
		{"addPosCallback", &UI_Lua::AddPosCallback},

		// Container Modification
		{"add", &UI_Lua::add},
		{"setFormButton", &UI_Lua::setFormButton},

		// Window Modification
		{"addCloseButton", &UI_Lua::AddCloseButton},

		// Picture Modification
		{"rotatePicture", &UI_Lua::rotatePicture},
		{"setPicture", &UI_Lua::setPicture},
		{"setBackground", &UI_Lua::setBackground},

		// Label Modification
		{"setText", &UI_Lua::setText},

		// Checkbox Modification
		{"setChecked", &UI_Lua::setChecked},
		{"setSliderValue", &UI_Lua::setSliderValue},

		// Dropdown Modification
		{"addOption", &UI_Lua::AddOption},

		// Map Functions
		{"getWorldPosition", &UI_Lua::getWorldPosition},
		{"setZoomable", &UI_Lua::SetZoomable},
		{"setPannable", &UI_Lua::SetPannable},

		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_UI);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, uiMethods, 0);

	luaL_openlib(L, EPIAR_UI, uiFunctions, 0);

	lua_pop(L,2);
}

/**\brief Validate that a userdata is a Widget
 * \todo We need a good way to confirm that the Widget object has not been deallocated.
 */
Widget* UI_Lua::checkWidget(lua_State *L, int index){
	Widget** widget = (Widget**)luaL_checkudata(L, index, EPIAR_UI);
	luaL_argcheck(L, widget != NULL, index, "`EPIAR_UI' expected");
	// Not all Widgets need to be attached to the Screen at all times, so we don't check UI::IsAttached
	// luaL_argcheck(L, UI::IsAttached( widget ), 1, "This Widget is not attached to the User Interface.");
	return *widget;
}

/** \brief Create a new Window
 *
 *  \note Unlike most UI_Lua Widget creators, this implicitely Adds the Window to the UI.
 *  \returns Lua userdata containing pointer to Window.
 */
int UI_Lua::newWindow(lua_State *L){
	int arg = 6;
	int n = lua_gettop(L);  // Number of arguments
	if (n < 5)
		return luaL_error(L, "Got %d arguments expected 5 (x, y, w, h, caption,[draggable], [ Widgets ... ])", n);

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

	if( lua_isboolean(L, 6) ){
		bool draggable = lua_toboolean(L, 6);
		(*win)->SetDragability( draggable );
		arg = 7;
	}

	UI::Add(*win);

	// Collect 'extra' widgets and Add them as children
	for(; arg <= n;arg++){
		Widget** widget= (Widget**)lua_touserdata(L,arg);
		(*win)->AddChild(*widget);
	}
	
	return 1;
}

/** \brief Create a new Frame
 *
 *  \returns Lua userdata containing pointer to Frame.
 */
int UI_Lua::newFrame(lua_State *L){
	int arg;
	int n = lua_gettop(L);  // Number of arguments
	if (n < 4)
		return luaL_error(L, "Got %d arguments expected 4 (x, y, w, h, [ Widgets ... ])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));

	// Allocate memory for a pointer to object
	Frame **frame = (Frame**)lua_newuserdata(L, sizeof(Frame**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	*frame = new Frame(x, y, w, h);

	UI::Add(*frame);

	// Collect 'extra' widgets and Add them as children
	for(arg=5; arg <= n;arg++){
		Widget** widget= (Widget**)lua_touserdata(L,arg);
		if( widget == NULL ) {
			return luaL_error(L, "argument %d to newFrame is NULL", arg);
		}
		(*frame)->AddChild(*widget);
	}
	
	return 1;
}

/** \brief Close a Widget
 *
 *  \details When passed no arguments, it will close all Widgets.
 *           When passed one argument, it will close that Widget.
 */
int UI_Lua::close(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments

	if (n == 0) {
		UI::CloseAll();
	}
	else if(n == 1) {
		Widget* widget = checkWidget(L,1);
		luaL_argcheck(L, UI::IsAttached( widget ), 1, "This Widget is not attached to the User Interface. It may have already been closed. Use UI.search to confirm check if a Widget still exists.");

		UI::Close( widget );
	}
	else {
		luaL_error(L, "Got %d arguments expected 0 or 1 ([widget])", n);
	}
	return 0;
}

int UI_Lua::AddCallback(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if(n == 3) {
		Widget* widget = checkWidget(L,1);
		int action = luaL_checkinteger(L,2);
		luaL_argcheck(L, ((0 <= action) && (action < Action_Last)) , 2, "Invalid action number.");
		string callback = luaL_checkstring(L,3);
		
		widget->RegisterAction( (action_type)action, new LuaAction(callback) );
	}
	else {
		luaL_error(L, "Got %d arguments expected 3 (widget, actiontype, callback )", n);
	}
	return 0;
}

int UI_Lua::AddPosCallback(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if(n == 3) {
		Widget* widget = checkWidget(L,1);
		int action = luaL_checkinteger(L,2);
		luaL_argcheck(L, ((0 <= action) && (action < Action_Last)) , 2, "Invalid action number.");
		string callback = luaL_checkstring(L,3);
		
		widget->RegisterAction( (action_type)action, new LuaPositionalAction(callback) );
	}
	else {
		luaL_error(L, "Got %d arguments expected 3 (widget, action, callback)", n);
	}
	return 0;
}

/** \brief Create a new Button
 *
 *  \returns Lua userdata containing pointer to Button.
 */
int UI_Lua::newButton(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( (n != 5) && (n != 6) )
		return luaL_error(L, "Got %d arguments expected 5 or 6 (x, y, w, h, caption, [Lua_code])", n);

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

	UI::Add(*button);

	return 1;
}

/** \brief Create a new Slider
 *
 *  \returns Lua userdata containing pointer to Slider.
 */
int UI_Lua::newSlider(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( (n != 5) && (n != 6) && (n != 7) )
		return luaL_error(L,
		"Got %d arguments expected 5 or 6(x, y, w, h, label, [position], [callback] )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string label = luaL_checkstring (L, 5);
	float position = 0.5f;
	string callback = "";
	for(int index=6; index<=7; ++index )
	{
		if ( lua_isnumber(L, index) ) {
			position = TO_FLOAT( luaL_checknumber(L,  index) );
		} else if ( lua_isstring(L, index) ) {
			callback = luaL_checkstring(L,  index);
		}
	}

	// Allocate memory for a pointer to object
	Slider **slider= (Slider**)lua_newuserdata(L, sizeof(Slider**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	
	if (n == 7) {
		*slider = new Slider(x,y,w,h,label,position);
		// TODO Calbacks are not implemented correctly right now.
		LogMsg(WARN, "Slider Callbacks are not currently implemented.");
	} else if (n == 6) {
		*slider = new Slider(x,y,w,h,label,position);
	} else {
		*slider = new Slider(x,y,w,h,label);
	}

	UI::Add(*slider);
	
	return 1;
}

/** \brief Create a new Tabs Widget
 *
 *  \returns Lua userdata containing pointer to Tabs Widget.
 */
int UI_Lua::newTabContainer(lua_State *L){
	int n = lua_gettop(L);	// Number of arguments
	if ( (n != 5 ) )
		return luaL_error(L,
		"Got %d arguments expected 5 (x, y, w, h, label)", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string name = luaL_checkstring (L, 5);
	Tabs **tabs = (Tabs**)lua_newuserdata(L, sizeof(Tabs**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);

	*tabs = new Tabs(x,y,w,h,name);
	UI::Add(*tabs);

	return 1;
}

/** \brief Create a new Tab Widget
 *
 *  \returns Lua userdata containing pointer to Tab Widget.
 */
int UI_Lua::newTab(lua_State *L){
	int n = lua_gettop(L);	// Number of arguments
	if ( (n != 1 ) )
		return luaL_error(L,
		"Got %d arguments expected 1 (caption)", n);

	string name = luaL_checkstring (L, 1);
	
	Tab **tab = (Tab**)lua_newuserdata(L, sizeof(Tab**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);

	*tab = new Tab(name);
	UI::Add(*tab);

	return 1;
}

/** \brief Create a new Textbox
 *
 *  \returns Lua userdata containing pointer to Textbox.
 */
int UI_Lua::newTextbox(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( n < 4  )
		return luaL_error(L, "Got %d arguments expected 3, 4, or 5 (x, y, w, h, [text], [name])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string text = "";
	string name = "";
	if(n>=5) text = luaL_checkstring (L, 5);
	if(n>=6) name = luaL_checkstring (L, 6);

	// Allocate memory for a pointer to object
	Textbox **textbox = (Textbox**)lua_newuserdata(L, sizeof(Textbox**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	*textbox = new Textbox(x, y, w, h, text, name);

	UI::Add(*textbox);

	return 1;
}

/** \brief Create a new Textarea
 *
 *  \returns Lua userdata containing pointer to Textarea.
 */
int UI_Lua::newTextarea(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( n < 4  )
		return luaL_error(L, "Got %d arguments expected 3, 4, or 5 (x, y, w, h, [text], [name])", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string text = "";
	string name = "";
	if(n>=5) text = luaL_checkstring (L, 5);
	if(n>=6) name = luaL_checkstring (L, 6);

	// Allocate memory for a pointer to object
	Textarea **textarea = (Textarea**)lua_newuserdata(L, sizeof(Textarea**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	*textarea = new Textarea(x, y, w, h, text, name);

	UI::Add(*textarea);

	return 1;
}

/** \brief Create a new Label
 *
 *  \returns Lua userdata containing pointer to Label.
 */
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

	UI::Add(*label);

	return 1;
}

/** \brief Create a new Picture
 *
 *  \returns Lua userdata containing pointer to Picture.
 */
int UI_Lua::newPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ( (n != 3) && (n != 5) && (n != 9))
		return luaL_error(L, "Got %d arguments expected 3, 5, or 9 (x, y, [w, h,] modelname, [red,blue,green,alpha] )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));

	int w, h;
	w = h = 0;
	if(n >= 5){
		w = int(luaL_checknumber (L, 3));
		h = int(luaL_checknumber (L, 4));
	}
	string picname = luaL_checkstring (L, (n >= 5 ? 5 : 3) );
	

	// Get Background Color
	float red,blue,green,alpha;
	red=blue=green=alpha=0.0f; // default is clear black
	if(n==9) {
		red   = float(luaL_checknumber (L, 6));
		blue  = float(luaL_checknumber (L, 7));
		green = float(luaL_checknumber (L, 8));
		alpha = float(luaL_checknumber (L, 9));
	}

	// Allocate memory for a pointer to object
	Picture **pic= (Picture**)lua_newuserdata(L, sizeof(Picture**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	if (w + h > 0)
		*pic = new Picture(x,y,w,h, picname );
	else
		*pic = new Picture(x,y, picname );
	(*pic)->SetColor(red,blue,green,alpha);

	UI::Add(*pic);

	return 1;
}

/** \brief Create a new Checkbox
 *
 *  \returns Lua userdata containing pointer to Checkbox.
 */
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

		UI::Add(*checkbox);
	} else {
		return luaL_error(L, "Got %d arguments expected 4 (x, y, chekced, labeltext)", n);
	}

	return 1;
}

/** \brief Create a new Checkbox
 *
 *  \note Tables of strings will be flattened and added
 *  \returns Lua userdata containing pointer to Checkbox.
 */
int UI_Lua::newDropdown(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n >= 4){
		int x = int(luaL_checknumber (L, 1));
		int y = int(luaL_checknumber (L, 2));
		int w = int(luaL_checknumber (L, 3));
		int h = int(luaL_checknumber (L, 4));
		
		Dropdown **dropdown = (Dropdown**)lua_newuserdata(L, sizeof(Dropdown**));
		luaL_getmetatable(L, EPIAR_UI);
		lua_setmetatable(L, -2);
		*dropdown = new Dropdown(x, y, w, h);
	
		// Collect options
		for(int arg = 5; arg <= n; ++arg){
			if( lua_istable(L, arg) ) {
				// Tables can be lists of strings
				list<string>::iterator i;
				list<string> values = Lua::getStringListField( arg );
				for(i = values.begin(); i != values.end(); ++i) {
					(*dropdown)->AddOption( (*i) );
				}
			} else {
				// Everything else is converted into a string
				string option = lua_tostring(L, arg);
				(*dropdown)->AddOption( option );
			}
		}

		UI::Add(*dropdown);
	} else {
		return luaL_error(L, "Got %d arguments expected at least 4 (x, y, w, h, [options ...])", n);
	}

	return 1;
}

/** \brief Create a new Paragraph
 *
 *  \returns Lua userdata containing pointer to Paragraph.
 */
int UI_Lua::newParagraph(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5)
		return luaL_error(L, "Got %d arguments expected 5 (x, y, w, h, text )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));
	string text = luaL_checkstring (L, 5);

	// Allocate memory for a pointer to object
	Paragraph **p = (Paragraph**)lua_newuserdata(L, sizeof(Paragraph**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	*p = new Paragraph(x, y, w, h, text);

	UI::Add(*p);

	return 1;
}

/** \brief Create a new Map
 *
 *  \returns Lua userdata containing pointer to Map
 */
int UI_Lua::newMap(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 4)
		return luaL_error(L, "Got %d arguments expected 4 (x, y, w, h )", n);

	int x = int(luaL_checknumber (L, 1));
	int y = int(luaL_checknumber (L, 2));
	int w = int(luaL_checknumber (L, 3));
	int h = int(luaL_checknumber (L, 4));

	// Allocate memory for a pointer to object
	Map **p = (Map**)lua_newuserdata(L, sizeof(Map**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);

	Simulation *sim = Simulation_Lua::GetSimulation(L);
	
	*p = new Map(x, y, w, h, Coordinate(0,0), sim->GetSpriteManager() );

	UI::Add(*p);

	return 1;
}


int UI_Lua::newConfirm(lua_State *L)
{
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1){
		return luaL_error(L, "Got %d arguments expected 1 (message)", n);
	}

	string message = luaL_checkstring (L, 1);
	bool choice = Dialogs::Confirm( message.c_str() );
	lua_pushboolean(L, (int) choice );

	return 1;
}

int UI_Lua::newAlert(lua_State *L)
{
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1){
		return luaL_error(L, "Got %d arguments expected 1 (message)", n);
	}

	string message = luaL_checkstring (L, 1);
	Dialogs::Alert( message.c_str() );

	return 0;
}

/** \brief Add widgets to the UI.
 *
 *  \details This accepts multiple Widgets.
 */
int UI_Lua::addWidget(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n == 0){
		return luaL_error(L, "Got %d arguments expected 1 (widgets, ...)", n);
	}

	for(int i=1; i<=n; i++){
		Widget* widget = checkWidget(L,i);
		luaL_argcheck(L, false == UI::IsAttached( widget ), i, "This Widget is already attached to the User Interface.");
		UI::Add(widget);
	}

	return 0;
}

/** \brief Find widgets in the UI
 *
 *  \details This accepts multiple Widgets.
 *  \see UI::search, Container::search
 */
int UI_Lua::search(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1){
		return luaL_error(L, "Got %d arguments expected 1 (query)", n);
	}

	string query = luaL_checkstring (L, 1);
	Widget *result = UI::Search( query );
	if( result == NULL ) {
		return 0;
	}

	Widget **passback = (Widget**)lua_newuserdata(L, sizeof(Widget**));
	luaL_getmetatable(L, EPIAR_UI);
	lua_setmetatable(L, -2);
	*passback = result;

	return 1;
}

/** \brief Change the Image in a Picture Widget
 *
 */
int UI_Lua::setPicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2){
		Picture* pic = (Picture*)checkWidget(L,1);
		luaL_argcheck(L, pic->GetMask() & WIDGET_PICTURE, 1, "`Picture' expected.");
		string picname = luaL_checkstring (L, 2);
		pic->Set( picname );
	
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, picname)", n);
	}
	return 0;
}

/** \brief Change the Background of a Picture Widget
 *
 */
int UI_Lua::setBackground(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if ((n == 4) || (n == 5)){
		Picture* pic = (Picture*)checkWidget(L,1);
		luaL_argcheck(L, pic->GetMask() & WIDGET_PICTURE, 1, "`Picture' expected.");

		float r = luaL_checknumber (L, 2);
		float g = luaL_checknumber (L, 3);
		float b = luaL_checknumber (L, 4);
		float a = pic->GetAlpha();
		if( n == 5 ) {
			a = luaL_checknumber (L, 5);
		}

		pic->SetColor( r, g, b, a );
	
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, picname)", n);
	}
	return 0;
}

/** \brief add Widgets to a Container
 *  \details This accepts multiple Widgets.
 *
 */
int UI_Lua::add(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n >= 2){
		Container* outer = (Container*)checkWidget(L,1);
		luaL_argcheck(L, outer->GetMask() & WIDGET_CONTAINER, 1, "`Container' expected.");

		for(int i=2; i<=n; i++){
			Widget* inner = checkWidget(L,i);
			outer->AddChild( inner );
		}
	} else {
		luaL_error(L, "Got %d arguments expected 2 or more (self, widget [...])", n);
	}
	return 0;
}

/** \brief Resize a Widget
 *
 */
int UI_Lua::move(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 5)
		return luaL_error(L, "Got %d arguments expected 1 (self, x,y,w,h)", n);
	
	// Get the new postition
	Widget* widget = checkWidget(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);

	// Move the widget
	widget->SetX( x );
	widget->SetY( y );
	widget->SetW( w );
	widget->SetH( h );

	return 0;
}

/** \brief Set the Form Button for 'Enter' keys to thie Container
 */
int UI_Lua::setFormButton(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n == 2){
		Container* container = (Container*)checkWidget(L,1);
		luaL_argcheck(L, container->GetMask() & WIDGET_CONTAINER, 1, "`Container' expected.");

		Button* button = (Button*)checkWidget(L,2);
		luaL_argcheck(L, button->GetMask() & WIDGET_BUTTON, 2, "`Button' expected.");

		container->SetFormButton( button );
	} else {
		luaL_error(L, "Got %d arguments expected 2 (self, picname)", n);
	}
	return 0;
}

/** \brief Rotate a Picture Widget
 *
 */
int UI_Lua::rotatePicture(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, angle )", n);

	Picture* pic = (Picture*)checkWidget(L,1);
	luaL_argcheck(L, pic->GetMask() & WIDGET_PICTURE, 1, "`Picture' expected.");

	double angle = luaL_checknumber (L, 2);

	pic->Rotate(angle);

	return 1;
}

/** \brief Set Widget Text
 *  \todo This should support more widget types.
 */
int UI_Lua::setText(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, text)", n);

	Widget *widget = checkWidget(L,1);
	string text = luaL_checkstring(L, 2);

	bool success;
	int mask = widget->GetMask();
	mask &= ~WIDGET_CONTAINER; // Turn off the Container flag.
	switch( mask ) {
		// These Widget types currently support setText
		case WIDGET_LABEL:
			((Label*)(widget))->SetText( text );
			break;
		case WIDGET_TEXTBOX:
			((Textbox*)(widget))->SetText( text );
			break;
		case WIDGET_TEXTAREA:
			((Textarea*)(widget))->SetText( text );
			break;
		case WIDGET_BUTTON:
			((Button*)(widget))->SetText( text );
			break;
		case WIDGET_DROPDOWN:
			success = ((Dropdown*)(widget))->SetText( text );
			if( !success ) {
				return luaL_error(L, "This Dropdown does not have an option '%s'.", text.c_str() );
			}
			break;
		case WIDGET_PARAGRAPH:
			((Paragraph*)(widget))->SetText( text );
			break;
		// TODO These Widget Types do not currently accept setText, but they should.
		case WIDGET_TAB:
		case WIDGET_WINDOW:
			return luaL_error(L, "Epiar does not currently calling setText on Widgets of type '%s'.", (widget)->GetType().c_str() );
			break;

		// These Widget Types can't accept setText.
		case WIDGET_TABS:
		case WIDGET_FRAME:
		case WIDGET_SLIDER:
		case WIDGET_PICTURE:
		case WIDGET_CHECKBOX:
		case WIDGET_SCROLLBAR:
		case WIDGET_CONTAINER:
		default:
			return luaL_error(L, "Cannot setText to Widget of type '%s'.", (widget)->GetType().c_str() );
	}

	return 0;
}

/** \brief Test if a checkbox is checked.
 *  \returns true if the Checkbox is checked.
 */
int UI_Lua::IsChecked(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);

	Checkbox *box = (Checkbox*)checkWidget(L,1);
	luaL_argcheck(L, box->GetMask() & WIDGET_CHECKBOX, 1, "`Checkbox' expected.");

	lua_pushboolean(L, (int) box->IsChecked() );

	return 1;
}

/** \brief Check or Uncheck a checkbox
 */
int UI_Lua::setChecked(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, value)", n);

	Checkbox *box = (Checkbox*)checkWidget(L,1);
	luaL_argcheck(L, box->GetMask() & WIDGET_CHECKBOX, 1, "`Checkbox' expected.");

	bool checked = lua_toboolean(L, 2) != 0;

	box->Set(checked);

	return 0;
}

/** \brief Set a Slider Value
 */
int UI_Lua::setSliderValue(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, value)", n);
	
	Slider *slide = (Slider*)checkWidget(L,1);
	luaL_argcheck(L, slide->GetMask() & WIDGET_SLIDER, 1, "`Checkbox' expected.");

	float percent= (float)lua_tonumber(L, 2);

	slide->SetVal(percent);
	
	return 0;
}

/** \brief Get the Text Value of a Widget
 *  \todo Currently this only supports Labels and Textboxes, but it shouls support more Widget types.
 *  \returns string or nil
 */
int UI_Lua::GetText(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);

	Widget *widget= checkWidget(L, 1);

	int mask = widget->GetMask();
	mask &= ~WIDGET_CONTAINER; // Turn off the Container flag.
	switch( mask ) {
		// These Widget types currently support GetText
		case WIDGET_LABEL:
			lua_pushstring(L, ((Label*)(widget))->GetText().c_str() );
			return 1;
			break;
		case WIDGET_TEXTAREA:
			lua_pushstring(L, ((Textarea*)(widget))->GetText().c_str() );
			return 1;
			break;
		case WIDGET_TEXTBOX:
			lua_pushstring(L, ((Textbox*)(widget))->GetText().c_str() );
			return 1;
			break;
		case WIDGET_DROPDOWN:
			lua_pushstring(L, ((Dropdown*)(widget))->GetText().c_str() );
			return 1;
			break;
		case WIDGET_BUTTON:
			lua_pushstring(L, ((Button*)(widget))->GetText().c_str() );
			return 1;
			break;
		case WIDGET_PARAGRAPH:
			lua_pushstring(L, ((Paragraph*)(widget))->GetText().c_str() );
			return 1;
			break;
		// TODO These Widget Types do not currently accept getText, but they should.
		case WIDGET_TAB:
		case WIDGET_WINDOW:
			return luaL_error(L, "Epiar does not currently calling getText on Widgets of type '%s'.", (widget)->GetType().c_str() );
			break;

		// These Widget Types can't accept GetText.
		case WIDGET_TABS:
		case WIDGET_FRAME:
		case WIDGET_SLIDER:
		case WIDGET_PICTURE:
		case WIDGET_CHECKBOX:
		case WIDGET_SCROLLBAR:
		case WIDGET_CONTAINER:
		default:
			return luaL_error(L, "Cannot getText to Widget of type '%s'.", (widget)->GetType().c_str() );
	}
}

/**\brief Get the Position and size of a Widget
 */
int UI_Lua::GetEdges(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);

	Widget *widget = checkWidget(L,1);
	lua_pushinteger(L, widget->GetX() );
	lua_pushinteger(L, widget->GetY() );
	lua_pushinteger(L, widget->GetW() );
	lua_pushinteger(L, widget->GetH() );

	return 4;
}

/**\brief Get X of a Widget
 */
int UI_Lua::GetX(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	Widget *widget = checkWidget(L,1);
	lua_pushinteger(L, widget->GetX() );
	return 1;
}

/**\brief Get Y of a Widget
 */
int UI_Lua::GetY(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	Widget *widget = checkWidget(L,1);
	lua_pushinteger(L, widget->GetY() );
	return 1;
}



/**\brief Get the Width of a Widget
 */
int UI_Lua::GetW(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	Widget *widget = checkWidget(L,1);
	lua_pushinteger(L, widget->GetW() );
	return 1;
}

/**\brief Get the Height of a Widget
 */
int UI_Lua::GetH(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	Widget *widget = checkWidget(L,1);
	lua_pushinteger(L, widget->GetH() );
	return 1;
}

/**\brief Set X of a Widget
 */
int UI_Lua::SetX(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self,x)", n);
	Widget *widget = checkWidget(L,1);
	int x = luaL_checkinteger(L,2);
	widget->SetX( x );
	return 0;
}

/**\brief Set Y of a Widget
 */
int UI_Lua::SetY(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 1 (self,y)", n);
	Widget *widget = checkWidget(L,1);
	int y = luaL_checkinteger(L,2);
	widget->SetY( y );
	return 0;
}

/**\brief Set the Width of a Widget
 */
int UI_Lua::SetW(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 1 (self,w)", n);
	Widget *widget = checkWidget(L,1);
	int w = luaL_checkinteger(L,2);
	widget->SetW( w );
	return 0;
}

/**\brief Set the Height of a Widget
 */
int UI_Lua::SetH(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 1 (self,h)", n);
	Widget *widget = checkWidget(L,1);
	int h = luaL_checkinteger(L,2);
	widget->SetH( h );
	return 0;
}

/**\brief Append an option to this Dropdown
 * \note Tables of strings will be flattened and added
 */
int UI_Lua::AddOption(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n < 2)
		return luaL_error(L, "Got %d arguments expected at least 2 (self OPTION [OPTION ...])", n);

	Widget *widget = checkWidget(L, 1);
	luaL_argcheck(L, widget->GetMask() & WIDGET_DROPDOWN, 1, "`Dropdown' expected.");
	Dropdown *dropdown = (Dropdown*)widget;

	// Add the options
	for(int arg = 2; arg <= n; ++arg){
		if( lua_istable(L, arg) ) {
			// Tables can be lists of strings
			list<string>::iterator i;
			list<string> values = Lua::getStringListField( arg );
			for(i = values.begin(); i != values.end(); ++i) {
				dropdown->AddOption( (*i) );
			}
		} else {
			// Everything else is converted into a string
			string option = lua_tostring(L, arg);
			dropdown->AddOption( option );
		}
	}

	return 0;
}

int UI_Lua::AddCloseButton(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected at least 1 (self Window)", n);

	Widget *widget = checkWidget(L, 1);
	luaL_argcheck(L, widget->GetMask() & WIDGET_WINDOW, 1, "`Window' expected.");
	Window *window = (Window*)widget;
	window->AddCloseButton();

	return 0;
}

int UI_Lua::getWorldPosition(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if ( !(n == 1 || n == 3) )
		return luaL_error(L, "Got %d arguments expected 1 (self) or 3 (self, x, y)", n);

	// Get the Map Widget
	Widget *widget = checkWidget(L, 1);
	luaL_argcheck(L, widget->GetMask() & WIDGET_MAP, 1, "`Map' expected.");
	Map* map = (Map*)widget;

	// Get the resulting world position
	Coordinate world;
	if( n == 3) {
		int x = int(luaL_checknumber (L, 2));
		int y = int(luaL_checknumber (L, 3));
		world = map->ClickToWorld( Coordinate(x,y) );
	} else {
		world = map->GetCenter();
	}

	// Return World X,Y
	lua_pushinteger(L, world.GetX() );
	lua_pushinteger(L, world.GetY() );
	return 2;
}

int UI_Lua::SetPannable(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if ( n != 2 )
		return luaL_error(L, "Got %d arguments expected 2 (self, pan)", n);

	// Get the Map Widget
	Widget *widget = checkWidget(L, 1);
	luaL_argcheck(L, widget->GetMask() & WIDGET_MAP, 1, "`Map' expected.");
	Map* map = (Map*)widget;

	map->SetPannable( luaL_checkinteger(L, 2) );
	return 0;
}

int UI_Lua::SetZoomable(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if ( n != 2 )
		return luaL_error(L, "Got %d arguments expected 2 (self, zoom)", n);

	// Get the Map Widget
	Widget *widget = checkWidget(L, 1);
	luaL_argcheck(L, widget->GetMask() & WIDGET_MAP, 1, "`Map' expected.");
	Map* map = (Map*)widget;

	map->SetZoomable( luaL_checkinteger(L, 2) );
	return 0;
}
