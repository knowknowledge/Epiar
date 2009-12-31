/**\file			ui.h
 * \author			Chris Thielen (chris@luethy.net)
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

// Included UI widgets
#include "ui_lua.h"
#include "ui_label.h"
#include "ui_window.h"
#include "ui_button.h"
#include "ui_textbox.h"
#include "ui_checkbox.h"

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
		
		static void RegisterKeyboardFocus( Widget *widget );
		static Widget *DetermineMouseFocus( int x, int y );
		
	private:
		static list<Widget *> children;
		static Widget *mouseFocus, *keyboardFocus; // remembers which widgets last had focus
};

// temporary function to test/develop the UI
void ui_demo( bool in_loop = false );

#endif // __H_UI__
