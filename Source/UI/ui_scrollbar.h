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

class Scrollbar : public Widget {
	public:
		Scrollbar( int x, int y, int length, Widget *parent );

		~Scrollbar();

		void Draw( int relx = 0, int rely = 0 );

		// Use these when the encompassing window size changes
		void SetPosition(int x, int y) { this->x = x; this->y = y; }
		void SetSize(int length);

		bool MouseLDown( int xi, int yi );
		bool MouseDrag( int xi, int yi );

		string GetType( void ) { return string("Scrollbar");}

		void ScrollUp( int pix = SCROLLBAR_SCROLL );
		void ScrollDown( int pix = SCROLLBAR_SCROLL );

		int pos, maxpos;

	private:
		int GetMarkerSize( void );
		int MarkerPosToPixel( void );
		int MarkerPixelToPos( int xr, int yr );
		int CheckPos( int newpos );

		int markersize;
		Widget *parent;

};

#endif // __H_SCROLLBAR__
