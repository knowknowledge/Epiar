/*
 * Filename      : ui_label.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Friday, April 25, 2008
 * Last Modified : Friday, April 25, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __H_LABEL__
#define __H_LABEL__

#include "UI/ui.h"

class Label : public Widget {
	public:
		Label( int x, int y, string label );
		~Label();
		
		void Draw(  int relx = 0, int rely = 0 );

		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		
		void setText(string text){label=text;}
	
	private:
		int w, h;
		string label;
};

#endif // __H_LABEL__
