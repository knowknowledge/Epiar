/**\file			ui.h
 * \author			Maoserr
 * \date			Created: Monday, March 22, 2010
 * \date			Modified: Monday, March 22, 2010
 * \brief			UI demo & debugging.
 */

#include "includes.h"
#include "UI/ui.h"
#include "Input/input.h"
#include "Utilities/timer.h"

int test_ui(int argc, char **argv){
	bool quit=false;
	Input inputs;
	Timer::Update();

	Window awin(0,0,200,400,"A Window");
	Button abut(100,100,50,20,"A button");
	awin.AddChild(&abut);
	UI::Add(&awin);
	while( !quit ) {
		quit = inputs.Update();
		
		Timer::Update();

		UI::Run();
		Video::Erase();

		UI::Draw();
		Video::Update();
		Timer::Delay();
	}
	return 0;
}
