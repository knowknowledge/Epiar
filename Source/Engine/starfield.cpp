/**\file			starfield.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified : Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/starfield.h"
#include "Graphics/video.h"
#include "Engine/camera.h"

/**\class Starfield
 * \brief Controls the starfield. */

/**\struct Starfield::_stars
 * \brief Contains x,y coordinate as well as the color
 */

/**\brief Initializes the starfield.
 * \param num Number of stars to initialize
 */
Starfield::Starfield( int num ) {
	int i;
	
	// seed the random number generator
	srand(static_cast<unsigned int>( time(NULL) ));

	// allocate space for stars
	stars = (struct _stars *)malloc( sizeof(struct _stars) * num );

	// randomly assign position and color
	for( i = 0; i < num; i++ ) {
		int c;

		stars[i].x = (float)(rand() % (int)(1.3 * Video::GetWidth()));
		stars[i].y = (float)(rand() % (int)(1.4 * Video::GetHeight()));
		c = rand() % 225; // generate greys between 0 and 225
		stars[i].clr = static_cast<float>( c / 256. );
	}

	this->num = num;
}

/**\brief Destroys Starfield
 */
Starfield::~Starfield( void ) {
	free( stars );
}

/**\brief Draws the Starfield
 */
void Starfield::Draw( void ) {
	int i;

	for( i = 0; i < num; i++ ) {
		drawStar(stars[i].x, stars[i].y, stars[i].clr);
	}
}

/**\brief Updates the Starfield
 */
void Starfield::Update( Camera *camera ) {
	int i;
	double dx, dy;
	float w, h;

	camera->GetDelta( &dx, &dy );
	
	w = static_cast<float>(1.3 * Video::GetWidth());
	h = static_cast<float>(1.4 * Video::GetHeight());
	
	for( i = 0; i < num; i++ ) {
		stars[i].x -= (float)dx * stars[i].clr;
		stars[i].y -= (float)dy * stars[i].clr;

		// handle wrapping the stars around if they go offscreen top/left
		while( stars[i].x < 0.0f )
			stars[i].x += w;
		while( stars[i].y < 0.0f )
			stars[i].y += h;

		// handle wrapping the stars around if they go offscreen bottom/right
		while( stars[i].x > w )
			stars[i].x -= w;
		while( stars[i].y > h )
			stars[i].y -= h;
	}
}

/**\brief Draws the stars
 */
void Starfield::drawStar(float x, float y, float  brightness) 
{	
	float drawBrightness;  
	float xcomponent; 
	float ycomponent; 

	//Loop between both X columns 
	for (int xcnt = 0; xcnt <= 1; xcnt++) 
	{ 
		//Check for clipping in the X direction (make sure it fits horizontally) 
		if (((int)x - xcnt)>= 0 && ((int)y - xcnt) < Video::GetWidth()) 
		{ 
			//Loop between both Y rows 
			for (int ycnt = 0; ycnt <= 1; ycnt++) 
			{ 
				//Check for clipping in the Y direction (make sure it fits vertically) 
				if ((int)y - ycnt >= 0 && (int)y - ycnt < Video::GetHeight()) 
				{ 
					//Get the x component of the brightness and divide it by 2 
					xcomponent = (((float)(xcnt - (x - (int)x)) * .5f)); 
					
					//Get the y component of the brightness and divide it by 2 
					ycomponent = (((float)(ycnt - (y - (int)y)) * .5f)); 

					if (ycomponent<0) ycomponent=ycomponent*(-1);
					if (xcomponent<0) xcomponent=xcomponent*(-1);
					
					//Add the x anid y components of the brightness to get the total brightness at that particular location 
					drawBrightness = ((xcomponent + ycomponent) * brightness); 

					Video::DrawPoint((int)x - xcnt, (int)y - ycnt, drawBrightness, drawBrightness, drawBrightness); 
				} 
			} 
		} 
	} 
}
