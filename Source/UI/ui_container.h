/**\file			ui_container.h
 * \author			Maoserr
 * \date			Created: Saturday, March 27, 2010
 * \date			Modified: Saturday, March 27, 2010
 * \brief			Container object can contain other widgets.
 */

#ifndef __H_UI_CONTAINER__
#define __H_UI_CONTAINER__

#include "UI/ui.h"
#include "ui_widget.h"
#include "ui_scrollbar.h"
#include "ui_button.h"

class Container : public Widget {
	public:
		Container(string _name = "UnspecifiedContainer", bool _mouseHandled = true );
		virtual ~Container( void );

		virtual string GetType( void ) { return string("Container"); }
		virtual int GetMask( void ) { return WIDGET_CONTAINER; }
		bool IsEmpty( void ){return this->children.empty();}

		virtual Container *AddChild( Widget *widget );
		virtual bool DelChild( Widget *widget );
		virtual void Empty( void );
		virtual void ResetInput( void );
		virtual void ResetScrollBars();

		virtual Container *SetFormButton( Button* button );

		virtual Widget *DetermineMouseFocus( int relx, int rely );

		virtual bool IsAttached( Widget* possible );

		virtual Widget *Search( string full_query);
		virtual Widget *ChildNamed( string _name, int mask = WIDGET_ALL );
		virtual Widget *ChildFromTop( int pos, int mask = WIDGET_ALL );
		virtual Widget *ChildFromBottom( int pos, int mask = WIDGET_ALL );
		virtual Widget *NextChild( Widget* widget, int mask = WIDGET_ALL );
		virtual Widget *PrevChild( Widget* widget, int mask = WIDGET_ALL );

		virtual void Draw( int relx = 0, int rely = 0 );

		xmlNodePtr ToNode();

		// Only allow UI to send events
		friend class UI;

	protected:
		virtual bool Detach( Widget *child );
		// Input events
		virtual bool MouseMotion( int xi, int yi );
		virtual bool MouseLUp( int xi, int yi );
		virtual bool MouseLDown( int xi, int yi );
		virtual bool MouseLRelease( void );
		virtual bool MouseMUp( int xi, int yi );
		virtual bool MouseMDown( int xi, int yi );
		virtual bool MouseMRelease( void );
		virtual bool MouseRUp( int xi, int yi );
		virtual bool MouseRDown( int xi, int yi );
		virtual bool MouseRRelease( void );
		virtual bool MouseWUp( int xi, int yi );
		virtual bool MouseWDown( int xi, int yi );
		virtual bool KeyboardEnter( void );
		virtual bool KeyboardLeave( void );
		virtual bool KeyPress( SDLKey key );

		list<Widget *> children;
		// If mouse input is handled - We default to true
		// On certain occasions we may need to default to false
		bool mouseHandled;

	private:
		Widget *keyboardFocus; ///< Remembers which child last had focus
		Widget *mouseHover; ///< Remember which widget mouse is hovering over
		Widget *lmouseDown; ///< Remember which widget was last L-clicked on.
		Widget *mmouseDown; ///< Remember which widget was last M-clicked on.
		Widget *rmouseDown; ///< Remember which widget was last R-clicked on.

		Scrollbar *vscrollbar; ///< The Vertical Scrollbar widget if it exists.
		Button *formbutton; ///< This Button will is activated on keyboard events.
};

#endif//__H_UI_CONTAINER__
