/**\file			ui_label.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#ifndef __H_LABEL__
#define __H_LABEL__

#include "UI/ui.h"

class Label : public Widget {
	public:
		Label( int x, int y, string label, bool centered=false );
		
		void Draw(  int relx = 0, int rely = 0 );

		void setText(string text){label=text;}
	
		string GetType( void ) {return string("Label");}
	
	private:
		string label;
		bool centered;
};

#endif // __H_LABEL__
