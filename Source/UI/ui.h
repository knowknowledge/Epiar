/*
 * Filename      : ui.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __H_UI__
#define __H_UI__

#include "includes.h"
#include "Input/input.h"
#include "ui_widget.h"

// Included UI widgets
#include "ui_lua.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"
#include "ui_textbox.h"

class UI {
	public:
		UI();
		~UI();
		
		static bool Add( Widget *widget );
		static void Run( void );
		static void Draw( void );
		static void Close( void );
		static void Close( Widget *widget );
		static void HandleInput( list<InputEvent> & events );
		static void ResetInput(void);
		
		static Widget *DetermineMouseFocus( int x, int y );
		
	private:
		static list<Widget *> children;
		static Widget *mouseFocus, *keyboardFocus; // remembers which widgets last had these focuses
};

// temporary function to test/develop the UI
void ui_demo( bool in_loop = false );

#endif // __H_UI__
