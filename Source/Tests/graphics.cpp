/**\file			graphics.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 31, 2009
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Graphics demo & debugging
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/image.h"
#include "Graphics/video.h"

int graphics_demo(int argc, char **argv) {
	Video::Erase();

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
	Image planet2("planet2.png");
	planet2.DrawCentered(400, 300, 0.);

	Image frigate("terran-frigate.png");
	frigate.DrawCentered(400, 300, 45.);

	SansSerif->Render(10, 50, "Hello world how are you why is this text simply not showing up?!");

	Video::Update();

	SDL_Delay(2500);
	return 0;
}

