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

		int GetWidth( void ) { return w; }
		int GetHeight( void ) { return h; }
		string GetName( void ) {return string("Tab_"+caption);}

		friend class Tabs;

	private:
		int w,h;
		int capw;
		string caption;
};

class Tabs : public Widget {
	public:
		Tabs( int x, int y, int w, int h, const string& name );
		bool AddChild( Widget *widget );
		void TabNext( void );

		void Draw( int relx = 0, int rely = 0 );
		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
	
		void MouseLDown( int x, int y );
		void MouseLUp( int x, int y );
		void MouseMotion( int x, int y, int dx, int dy );

		string GetName( void ) {return string("Tabs_"+name);}

	private:
		Tab* CheckTabClicked( int xr, int yr );

		int w,h;
		string name;
		Tab* activetab;
};





#endif // __H_TABS__
