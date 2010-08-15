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

class Widget {
	public:
		Widget( void );
		virtual ~Widget( void ){};
		
		// Function to retrieve dimensions
		virtual int GetX( void ){ return this->x; }
		virtual int GetY( void ){ return this->y; }
		virtual int GetW( void ){ return this->w; }
		virtual int GetH( void ){ return this->h; }

		virtual void SetX( int _x ){ x = _x; }
		virtual void SetY( int _y ){ y = _y; }
		virtual void SetW( int _w ){ w = _w; }
		virtual void SetH( int _h ){ h = _h; }
		
		virtual string GetType( void ) { return string("GenericWidget"); }
		string GetName( void ) { return this->name; }
		bool IsActive( void ){return this->keyactivated;}

		virtual void Draw( int relx = 0, int rely = 0 );
		bool Contains( int relx, int rely );

		virtual xmlNodePtr ToNode();

		// Only allow UIContainer to send events
		friend class UIContainer;

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

		bool hovering, hidden,disabled;	// If this widget should be hidden or disabled
		int x, y, w, h;
		int dragX, dragY;		// if dragging, this is the offset from (x,y) to the point of click for the drag
		string name;
		bool keyactivated;		// remember if this widget has keyboard activation
};

#endif // __H_UI_WIDGET__
