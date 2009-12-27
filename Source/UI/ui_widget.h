/**\file			ui_widget.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */


#ifndef __H_UI_WIDGET__
#define __H_UI_WIDGET__

class Widget {
	public:
		Widget() { keyboardFocus = NULL; };
		virtual ~Widget() { };
		
		bool AddChild( Widget *widget );
		virtual void Draw( int relx = 0, int rely = 0 );
		virtual void Update( void );

		int GetX( void ) { return x; }
		int GetY( void ) { return y; }
		void SetX( int x ) { this->x = x; }
		void SetY( int y ) { this->y = y; }

		// This is called when a widget obtains focus (in case it wants to highlight itself or something)
		virtual void FocusMouse( int x, int y );
		virtual void UnfocusMouse( void );
		virtual void FocusKeyboard( void ) { };
		virtual void UnfocusKeyboard( void );
		int GetDragX( void ) { return dragX; }
		int GetDragY( void ) { return dragY; }
		
		virtual int GetWidth( void ) = 0;
		virtual int GetHeight( void ) = 0;

		virtual string GetName( void ) { return string("GenericWidget"); }
	
		virtual bool Contains( int relx, int rely );
		virtual Widget *DetermineMouseFocus( int relx, int rely );
		virtual void MouseDown( int wx, int wy ); // coords of mouse down, relative to widget's upper left (0,0)
		
		virtual void KeyPress( SDLKey key );
	
	private:
		int x, y;
		int dragX, dragY; // if dragging, this is the offset from (x,y) to the point of click for the drag
		list<Widget *> children;
		
		Widget *keyboardFocus; // we have to track keyboardFocus within children, unlike mouseFocus
};

#endif // __H_UI_WIDGET__
