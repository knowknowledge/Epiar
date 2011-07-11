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
		Dropdown* AddOptions( list<string> options );

		void Draw( int relx = 0, int rely = 0 );
	
		virtual string GetType( void ) { return string("Dropdown"); }
		virtual int GetMask( void ) { return WIDGET_DROPDOWN; }

		string GetText();
		bool SetText(string text);

	protected:
		bool MouseMotion( int xi, int yi );
		bool MouseLUp( int xi, int yi );
		bool MouseLDown( int xi, int yi );
		bool MouseLeave( void );

	private:
		void open();
		void close();

		bool opened; ///< Is the Dropdown currently Dropped down?
		Image* bitmap_normal; ///< The closed form.
		Image* bitmap_open; ///< The form of default options when open.
		Image* bitmap_mouseover; ///< The currently hovered option.
		Image* bitmap_selected; ///< The last selected option.

		vector<string> options; ///< The selectable Options.
		unsigned int selected; ///< The option that is currently selected.
		unsigned int hovered; ///< The option that is currently being hovered over.

		int baseheight; ///< The height of each selectable option.
		int xoffset; ///< X-offset to clearly mark open from closed Dropdowns
		int yoffset; ///< Y-offset to clearly mark open from closed Dropdowns
};

#endif // __H_DROPDOWN__
