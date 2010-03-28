/**\file			font.h
 * \author			Maoserr
 * \date			Created: Sunday, March 28, 2010
 * \brief			Font redering tests.
 */

#include "includes.h"
#include "common.h"
#include "UI/ui.h"
#include "Input/input.h"
#include "Utilities/timer.h"

int test_font(int argc, char **argv){
	Video::DrawRect(100,100,100,20, .4f, .4f, .4f);
	SansSerif->SetColor(1.f,1.f,1.f);
	SansSerif->Render(100,100,"Testing");
	Video::Update();
	
	SDL_Event event;
	do{
		SDL_WaitEvent(&event);
	}while( event.type != SDL_KEYUP );
	return 0;
}
