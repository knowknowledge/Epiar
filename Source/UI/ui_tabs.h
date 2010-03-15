/**\file			ui_tabs.h
 * \author			Maoserr
 * \date			Created: Sunday, March 14, 2010
 * \date			Modified: Sunday, March 14, 2010
 * \brief			Implement Tab pages
 */

#ifndef __H_TABS__
#define __H_TABS__

#include "Utilities/log.h"

class Tab : public Widget {
	public:
		Tab( const string& _caption );
		~Tab();

		int GetWidth( void ) { return w; }
		int GetHeight( void ) { return h; }
		string GetName( void ) {return string("Tab_"+caption);}

		friend class Tabs;

	private:
		int w,h;
		string caption;
};

class Tabs : public Widget {
	public:
		Tabs( int x, int y, int w, int h, const string& name );
		bool AddChild( Widget *widget );
		void TabNext( void );
		~Tabs();

		void Draw( int relx = 0, int rely = 0 );
		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
	
		void MouseDown( int x, int y );

		string GetName( void ) {return string("Tabs_"+name);}

	private:
		int w,h;
		string name;
		Tab* activetab;
};





#endif // __H_TABS__
