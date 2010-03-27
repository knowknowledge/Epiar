/**\file			ui_widget.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, December 27, 2009
 * \brief
 * \details
 */


#ifndef __H_UI_WIDGET__
#define __H_UI_WIDGET__

typedef enum{LEFT,MIDDLE,RIGHT} mouseBtn;

class Widget {
	public:
		Widget( void );
		virtual ~Widget( void );
		
		// Function to retrieve dimensions
		virtual int GetX( void ){ return this->x; }
		virtual int GetY( void ){ return this->y; }
		virtual int GetW( void ){ return this->w; }
		virtual int GetH( void ){ return this->h; }
		
		virtual void Update( void ){};
		virtual string GetType( void ) { return string("GenericWidget"); }
		string GetName( void ) { return this->name; }
		bool IsEmpty( void ){return this->children.empty();}
		bool IsActive( void ){return this->keyactivated;}

		virtual bool AddChild( Widget *widget );
		virtual bool DelChild( Widget *widget );
		virtual bool Empty( void );
		virtual bool Reset( void );
		virtual bool Contains( int relx, int rely );
		virtual Widget *DetermineMouseFocus( int relx, int rely );
		virtual void Draw( int relx = 0, int rely = 0 );

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

		bool hidden,disabled;	// If this widget should be hidden or disabled
	protected:
		int x, y, w, h;
		int dragX, dragY;		// if dragging, this is the offset from (x,y) to the point of click for the drag
		list<Widget *> children;
		string name;
		bool keyactivated;				// remember if this widget has keyboard activation

	private:
		Widget *keyboardFocus;			// remembers which child last had focus
		Widget *mouseHover;				// remember which widget mouse is hovering over
		Widget *lmouseDown,
			*mmouseDown,*rmouseDown;	// remember which widget was clicked on
};

#endif // __H_UI_WIDGET__
