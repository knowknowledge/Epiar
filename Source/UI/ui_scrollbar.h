/**\file			ui_scrollbar.h
 * \author			Maoserr
 * \date			Created: Tuesday, March 16, 2010
 * \date			Modified: Tuesday, March 16, 2010
 * \details
 */

#ifndef __H_SCROLLBAR__
#define __H_SCROLLBAR__

#define SCROLLBAR_THICK		15
#define SCROLLBAR_PAD		8
#define SCROLLBAR_MINMARK	10	// Minimum marker size
#define SCROLLBAR_BTN		18	// Scrollbar button size
#define SCROLLBAR_SCROLL	20	// Amount of scrolling per click
typedef enum{HORIZONTAL,VERTICAL} scrollbarType;

class Scrollbar : public Widget {
	public:
		Scrollbar( int x, int y, int length,
			scrollbarType type, Widget *parent );

		void Draw( int relx=0, int rely = 0 );

		int GetWidth( void ) { return w; }
		int GetHeight( void ) { return h; }

		// Use these when the encompassing window size changes
		void SetPosition(int x, int y){ SetX(x);SetY(y); }
		void SetSize(int length);

		void MouseLDown( int x, int y );
		void MouseMotion( int xi, int yi, int dx, int dy );

		string GetName( void ) {return string((type==HORIZONTAL)?"Horizontal":"Vertical") + string(" Scrollbar");}

		int pos,maxpos;

	private:
		int GetMarkerSize( void );
		int MarkerPosToPixel( void );
		int MarkerPixelToPos( int xr, int yr );
		int CheckPos( int newpos );

		int w,h;
		int markersize;
		scrollbarType type;
		Widget *parent;

};

#endif // __H_SCROLLBAR__
