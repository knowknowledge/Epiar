/*
 * Filename      : graphics.cpp
 * Author(s)     : Chris Thielen (chris@epiar.net)
 * Date Created  : Saturday, January 31, 2009
 * Purpose       : Graphics demo & debugging
 * Notes         : 
 */

#include "Debug/graphics.h"
#include "includes.h"
#include "Graphics/image2.h"
#include "Graphics/video.h"

void graphics_demo(void) {
	// draw a grid
	// -- draw the horizontal lines
	for(int j = 50; j < Video::GetHeight(); j += 50) {
		Video::DrawRect(0, j, Video::GetWidth(), 1, .4, .4, .4);
	}
	// -- draw the vertical lines
	for(int i = 50; i < Video::GetWidth(); i += 50) {
		Video::DrawRect(i, 0, 1, Video::GetHeight(), .4, .4, .4);
	}

	// draw four test circles
	Video::DrawCircle(50, 50, 50, 1., 1., 1., 1.);
	Video::DrawCircle(50, 550, 50, 1., 1., 1., 1.);
	Video::DrawCircle(750, 50, 50, 1., 1., 1., 1.);
	Video::DrawCircle(750, 550, 50, 1., 1., 1., 1.);

	// load an image and draw it in the center of the screen
	Image2 planet2("planet2.png");
	planet2.DrawCentered(400, 300, 0.);

	Image2 frigate("terran-frigate.png");
	frigate.DrawCentered(400, 300, 45.);

	Video::Update();

	SDL_Delay(2500);
}

