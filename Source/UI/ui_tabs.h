/**\file			ui_tabs.h
 * \author			Maoserr
 * \date			Created: Sunday, March 14, 2010
 * \date			Modified: Sunday, March 14, 2010
 * \brief			Implement Tab pages
 */

#ifndef __H_TABS__
#define __H_TABS__

#include "Utilities/log.h"

class Tab : public Container {
	public:
		Tab( const string& _caption );
		~Tab();
		Tab *AddChild( Widget *widget );

		void DrawHandle( int realx, int realy, bool active );

		int GetHandleWidth( bool active );

		string GetType( void ) {return string("Tab");}
		virtual int GetMask( void ) { return WIDGET_TAB | WIDGET_CONTAINER; }

	private:
		int capw;

		Image *active_left;
		Image *active_middle;
		Image *active_right;

		Image *inactive_left;
		Image *inactive_middle;
		Image *inactive_right;
};

class Tabs : public Container {
	public:
		Tabs( int x, int y, int w, int h, const string& name );
		Tabs *AddChild( Widget *widget );
		Widget *DetermineMouseFocus( int relx, int rely );

		void Draw( int relx = 0, int rely = 0 );
		bool MouseLDown( int xi, int yi );
		string GetType( void ) {return string("Tabs");}
		virtual int GetMask( void ) { return WIDGET_TABS | WIDGET_CONTAINER; }

		void TabNext( void );
	private:
		Tab* CheckTabClicked( int xr, int yr );
		Tab* activetab;

		Color background;
		Color edge;
};





#endif // __H_TABS__
