/**\file			ui_picture.h
 * \author			Matt Zweig
 * \date			Created: Tuesday, November 2, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Widget for displaying Images
 * \details
 */


#ifndef __H_UI_IMAGE
#define __H_UI_IMAGE

#include "Graphics/image.h"
#include "UI/ui.h"

class Picture: public Widget {
	public:
		void Default( int x, int y, int w, int h);
		Picture( int x, int y, int w, int h, string filename );
		Picture( int x, int y, int w, int h, Image *pic );
		~Picture();
		void Draw( int relx, int rely );
		void Rotate( double angle );
		void Set( Image *img );
		void Set( string filename );

		// Widget functions
		void Focus( int x, int y ) {};
		void Unfocus( void ) {};
		// TODO: When Image::resize works, just return w and h
		int GetWidth( void ) {return (bitmap)?bitmap->GetWidth():w;} 
		int GetHeight( void ) {return (bitmap)?bitmap->GetHeight():h;} 
		void MouseDown( int wx, int wy ) {};

		string GetName( void ) {return string("Picture");}
	private:
		int w, h;
		double rotation;
		Image *bitmap;
};

#endif // __H_UI_IMAGE 
