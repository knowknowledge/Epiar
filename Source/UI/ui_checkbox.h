/**\file			ui_checkbox.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, December 28, 2009
 * \brief
 * \details
 */

#ifndef __H_CHECKBOX__
#define __H_CHECKBOX__

#include "Graphics/image.h"
#include "UI/ui.h"

class Checkbox : public Widget {
	public:
		Checkbox( int x, int y, bool checked, string label);
		~Checkbox() { };
		
		void Draw( int relx = 0, int rely = 0 );

		bool IsChecked() {return checked;}
		void Set(bool val) {checked = val;}
	
		string GetType( void ) { return string("Checkbox"); }
		virtual int GetMask( void ) { return WIDGET_CHECKBOX; }

	protected:
		bool MouseLUp( int xi, int yi );

	private:
		string label;
		bool checked;

		Image *check_box;
		Image *blank_box;
};

#endif // __H_CHECKBOX__
