/**\file			ui_dropdown.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, November 18, 2010
 * \date			Modified: Friday, December 31, 2010
 * \brief
 * \details
 */

#ifndef __H_DROPDOWN__
#define __H_DROPDOWN__

#include "includes.h"
#include "Graphics/image.h"
#include "UI/ui.h"

class Dropdown : public Widget {
	public:
		Dropdown( int x, int y, int w, int baseheight );
		~Dropdown();

		Dropdown* AddOption( string option );

		bool MouseLDown( int xi, int yi );
		bool MouseLUp( int xi, int yi );
		bool MouseLeave( void );
		bool MouseMotion( int xi, int yi );

		void Draw( int relx = 0, int rely = 0 );
	
		virtual string GetType( void ) { return string("Dropdown"); }
		virtual int GetMask( void ) { return WIDGET_DROPDOWN; }

		string GetText() { return options[selected]; }
		void SetText(string text);

	private:
		void open();
		void close();

		bool opened;
		Image* bitmap_normal;
		Image* bitmap_mouseover;
		Image* bitmap_selected;

		vector<string> options;
		int selected;
		int hovered;

		int baseheight;
};

#endif // __H_DROPDOWN__
