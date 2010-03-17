/**\file			ui_scrollbar.h
 * \author			Maoserr
 * \date			Created: Tuesday, March 16, 2010
 * \date			Modified: Tuesday, March 16, 2010
 * \details
 */

#ifndef __H_SCROLLBAR__
#define __H_SCROLLBAR__

#define SCROLLBAR_THICK 15
#define SCROLLBAR_PAD 8
typedef enum{HORIZONTAL,VERTICAL} scrollbarType;

class Scrollbar : public Widget {
	public:
		Scrollbar( int x, int y, int length,
			scrollbarType type, Widget *parent );

		void Draw( int relx=0, int rely = 0 );

		int GetWidth( void ) { return w; }
		int GetHeight( void ) { return h; }

		void MouseLDown( int x, int y );

		int pos,maxpos;

	private:
		int w,h;
		scrollbarType type;
		Widget *parent;

};

#endif // __H_SCROLLBAR__
