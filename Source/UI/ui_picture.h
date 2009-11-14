/*
 * Filename      : ui_picture.h
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, November 2, 2009
 * Last Modified : Friday, November 14, 2009
 * Purpose       : Widget for displaying Images
 * Notes         : 
 */


#ifndef __H_UI_IMAGE
#define __H_UI_IMAGE

#include "Graphics/image.h"
#include "UI/ui.h"

class Picture: public Widget {
	public:
		Picture( int x, int y, int w, int h, string filename );
		~Picture();
		void Draw( int relx, int rely );
		void Rotate( double angle );
		void Set( Image *img );
		void Set( string filename );

		// Widget functions
		void Focus( int x, int y ) {};
		void Unfocus( void ) {};
		int GetWidth( void ) {return w;}
		int GetHeight( void ) {return h;}
		void MouseDown( int wx, int wy ) {};
	private:
		int w, h;
		double rotation;
		Image *bitmap;
};

#endif // __H_UI_IMAGE 
