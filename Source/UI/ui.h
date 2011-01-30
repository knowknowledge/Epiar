/**\file			ui.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef __H_UI__
#define __H_UI__

#include "includes.h"
#include "Input/input.h"
#include "ui_widget.h"
#include "ui_container.h"

// Included UI widgets
#include "ui_lua.h"
#include "ui_label.h"
#include "ui_scrollbar.h"
#include "ui_window.h"
#include "ui_frame.h"
#include "ui_button.h"
#include "ui_textbox.h"
#include "ui_picture.h"
#include "ui_checkbox.h"
#include "ui_tabs.h"
#include "ui_slider.h"
#include "ui_dropdown.h"

// Macro to check if xi, yi point is inside a rectangle x,y,w,h
#define WITHIN_BOUNDS(xi,yi,x,y,w,h) ( (xi > x) && (yi > y) && (xi < (x+w) ) && (yi < (y+h)) )

class UI {
	public:
		UI() {}
		~UI();

		static bool Initialize( string screenName );
		
		static Widget *Add( Widget *widget );
		static void Draw( void );
		static void Close( void );
		static void Close( Widget *widget );
		static void HandleInput( list<InputEvent> & events );
		static bool Active(void);
		
		static void RegisterKeyboardFocus( Widget *widget );
		static Widget *DetermineMouseFocus( int x, int y );

		static Widget *Search( string query );

		static bool IsAttached( Widget* possible );

		static void Save( void );

		static void SwapScreens(string newname, Image* oldBackground, Image* newBackground );

		static Font* font;
		
	private:
		static Container* NewScreen( string name );
		static bool HandleKeyboard( InputEvent i );
		static bool HandleMouse( InputEvent i );

		// Use a currentScreen widget to handle events,
		// so we don't need to duplicate code.
		static Container *currentScreen;
		static map<string,Container*> screens;
};

void UI_Test();

#endif // __H_UI__
