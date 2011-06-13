/**\file			ui_widget.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */


#ifndef __H_UI_WIDGET__
#define __H_UI_WIDGET__

#include "includes.h"
#include "UI/ui_action.h"

/** \addtogroup UI
 * @{
 */

#define WIDGET_LABEL               (0x00000001) ///< Mask for Label
#define WIDGET_BUTTON              (0x00000002) ///< Mask for Button
#define WIDGET_PICTURE             (0x00000004) ///< Mask for PICTURE
#define WIDGET_TEXTBOX             (0x00000008) ///< Mask for Textbox
#define WIDGET_SLIDER              (0x00000010) ///< Mask for Slider
#define WIDGET_DROPDOWN            (0x00000020) ///< Mask for Dropdown
#define WIDGET_CHECKBOX            (0x00000040) ///< Mask for Checkbox
#define WIDGET_SCROLLBAR           (0x00000080) ///< Mask for Scrollbar
#define WIDGET_MAP                 (0x00000100) ///< Mask for Map

#define WIDGET_CONTAINER           (0x00010000) ///< Mask for Container
#define WIDGET_FRAME               (0x00020000) ///< Mask for Frame
#define WIDGET_WINDOW              (0x00040000) ///< Mask for Window
#define WIDGET_TAB                 (0x00080000) ///< Mask for Tab
#define WIDGET_TABS                (0x00100000) ///< Mask for Tabs

#define WIDGET_NONE                (0x00000000) ///< Mask that doesn't match anything
#define WIDGET_ALL                 (0xFFFFFFFF) ///< Mask that matches everything

/** @} */

enum action_type{
	Action_MouseDrag,
	Action_MouseMotion,
	Action_MouseEnter,
	Action_MouseLeave,
	Action_MouseLUp,
	Action_MouseLDown,
	Action_MouseLRelease,
	Action_MouseMUp,
	Action_MouseMDown,
	Action_MouseMRelease,
	Action_MouseRUp,
	Action_MouseRDown,
	Action_MouseRRelease,
	Action_MouseWUp,
	Action_MouseWDown,
	Action_KeyboardEnter,
	Action_KeyboardLeave,
	//Action_KeyPress
	Action_Close,
	
	Action_Last // Always the last action type
}; ///< Actions that can be registered.

class Widget {
	public:
		Widget( void );
		virtual ~Widget( void );
		
		// Function to retrieve dimensions
		virtual int GetX( void ){ return this->x; }
		virtual int GetY( void ){ return this->y; }
		virtual int GetW( void ){ return this->w; }
		virtual int GetH( void ){ return this->h; }

		virtual void SetX( int _x ){ x = _x; }
		virtual void SetY( int _y ){ y = _y; }
		virtual void SetW( int _w ){ w = _w; }
		virtual void SetH( int _h ){ h = _h; }

		virtual int GetAbsX( void );
		virtual int GetAbsY( void );
		
		virtual string GetType( void ) { return string("GenericWidget"); }
		virtual int GetMask( void ) { return WIDGET_NONE; }
		virtual Widget* GetParent( void ) { return parent; }
		string GetName( void ) { return this->name; }
		bool IsActive( void ){return this->keyactivated;}

		virtual void Draw( int relx = 0, int rely = 0 );
		bool Contains( int relx, int rely );

		virtual xmlNodePtr ToNode();

		virtual Widget* RegisterAction( action_type type, Action* action );
		virtual bool Activate( action_type type, int x, int y );

		// Only allow Container to send events
		friend class Container;

	protected:

		// Input events
		virtual bool MouseDrag( int xi,int yi );
		virtual bool MouseMotion( int xi, int yi );
		virtual bool MouseEnter( int xi,int yi );
		virtual bool MouseLeave( void );
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

		string name;            ///< This widget's Name.  Names should be relatively unique.
		bool hovering;          ///< Is the user currently hovering over this widget?
		bool hidden;            ///< Is this widget is hidden?
		bool disabled;          ///< Is this widget is disabled?
		bool keyactivated;		///< Is this widget has keyboard activation.
		int x, y;               ///< The Location of this widget.
		int w, h;               ///< The Width and Height of this widget.
		int dragX, dragY;		///< If dragging, this is the offset from (x,y) to the point of click for the drag
		Widget* parent;         ///< This widget's parent.
		Action *(actions[Action_Last]); ///< Array of potential Actions
};

#endif // __H_UI_WIDGET__
