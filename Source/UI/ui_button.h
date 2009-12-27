/**\file			ui_button.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#ifndef __H_BUTTON__
#define __H_BUTTON__

#include "Graphics/image.h"
#include "UI/ui.h"

class Button : public Widget {
	public:
		Button( int x, int y, int w, int h, string label);
		Button( int x, int y, int w, int h, string label, void (*clickCallBack)());
		Button( int x, int y, int w, int h, string label, string lua_code);
		~Button();
		
		void Draw( int relx = 0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		
		void FocusMouse( int x, int y );
		void UnfocusMouse( void );
	
		void MouseDown( int wx, int wy );
		virtual string GetName( void ) {return string("Button_"+label);}
	private:
		void init( int x, int y, int w, int h, string label);
		int w, h;
		string label;
		Image *bitmap_current, *bitmap_normal, *bitmap_pressed;

		string lua_callback;
		void (*clickCallBack)();
		static void debugClick();
};

#endif // __H_BUTTON__
