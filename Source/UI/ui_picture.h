/**\file			ui_picture.h
 * \author			Matt Zweig
 * \date			Created: Tuesday, November 2, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Widget for displaying Images
 * \details
 */


#ifndef __H_UI_PICTURE
#define __H_UI_PICTURE

#include "Graphics/image.h"
#include "Graphics/video.h"
#include "UI/ui.h"

class Picture: public Widget {
	public:
		Picture( int x, int y, int w, int h, string filename );
		Picture( int x, int y, int w, int h, Image *pic, bool allow_stretching = false );
		Picture( int w, int h, string filename );
		Picture( int x, int y, Image *pic );

		void Draw( int relx, int rely );

		void Rotate( double angle );

		void Set( Image *img );
		void Set( string filename );
		void SetColor( float r, float g, float b, float a );

		Color GetColor( void ) { return color; }
		float GetAlpha( void ) { return alpha; }

		string GetType( void ) { return string("Picture"); }
		virtual int GetMask( void ) { return WIDGET_PICTURE; }

		void Center( int x, int y );

	private:
		void Default( int x, int y, int w, int h);

		double rotation;
		Image *bitmap;
		Color color;
		float alpha;
};

#endif // __H_UI_PICTURE

