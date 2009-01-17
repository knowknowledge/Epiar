/*
 * Filename      : ui_button.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, April 25, 2008
 * Last Modified : Friday, April 25, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __H_BUTTON__
#define __H_BUTTON__

#include "Graphics/image.h"
#include "UI/ui.h"

class Button : public Widget {
	public:
		Button( int x, int y, int w, int h, string label );
		~Button();
		
		void Draw( int relx = 0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		
		void Focus( int x, int y );
		void Unfocus( void );
	
	private:
		int w, h;
		string label;
		Image *bitmap, *bitmap_pressed;
};

#endif // __H_BUTTON__
