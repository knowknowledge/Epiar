/**\file			ui_scrollbar.h
 * \author			Maoserr
 * \date			Created: Tuesday, March 16, 2010
 * \date			Modified: Tuesday, March 16, 2010
 * \details
 */

#ifndef __H_SCROLLBAR__
#define __H_SCROLLBAR__

#include "Graphics/image.h"
#include "UI/ui.h"

#define SCROLLBAR_SCROLL	20	///< Amount of scrolling per click

class Scrollbar : public Widget {
	public:
		Scrollbar( int x, int y, int length, int maxpos );

		~Scrollbar();

		void Draw( int relx = 0, int rely = 0 );

		// Use these when the encompassing window size changes
		void SetPosition(int x, int y) { this->x = x; this->y = y; }
		void SetSize(int length);

		bool MouseLDown( int xi, int yi );
		bool MouseDrag( int xi, int yi );

		string GetType( void ) { return string("Scrollbar");}
		virtual int GetMask( void ) { return WIDGET_SCROLLBAR; }

		void ScrollUp( int pix = SCROLLBAR_SCROLL );
		void ScrollDown( int pix = SCROLLBAR_SCROLL );

		int GetPos() { return pos; }

	private:
		int GetMarkerSize( void );
		int MarkerPosToPixel( void );
		int MarkerPixelToPos( int xr, int yr );
		int CheckPos( int newpos );

		int pos;        ///< The current Marker position
		int maxpos;     ///< The Highest Marker position
		int markersize; ///< The Size of the Marker

		Image* bitmaps[6]; ///< The Scrollbar Images
};

#endif // __H_SCROLLBAR__
