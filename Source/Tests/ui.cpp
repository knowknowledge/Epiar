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
	Tabs *tabcont = new Tabs(0,0,180,300,"Tabs");
	awin.AddChild(tabcont);
	Tab *tab1 = new Tab("Tab1");
	tabcont->AddChild(tab1);
	Tab *tab2 = new Tab("Tab2");
	tabcont->AddChild(tab2);

	Button *abut = new Button(100,100,50,20,"A button");
	tab1->AddChild(abut);
	Slider *aslid = new Slider(0,0,100,20,"Slider");
	tab2->AddChild(aslid);
	Textbox *atext = new Textbox(0,40,100,1,"Hello","Label");
	tab2->AddChild(atext);
	
	
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
