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

	Window *awin = static_cast<Window*>(UI::Add(new Window(0,0,200,400,"A Window")));
	Tabs *tabcont = static_cast<Tabs*>(awin->AddChild(new Tabs(5,25,180,300,"Tabs")));
	Tab *tab1 = static_cast<Tab*>(tabcont->AddChild(new Tab("Tab1")));
	Tab *tab2 = static_cast<Tab*>(tabcont->AddChild(new Tab("Tab2")));

	Widget *aimg = tab1->AddChild(new Picture(50,100,50,50,"Resources/Graphics/corvet.png"));
	Widget *achk = tab1->AddChild(new Checkbox(10,120,true,"Hello"));
	Widget *abut = tab1->AddChild(new Button(100,300,50,20,"A button"));
	Widget *abut2 = tab1->AddChild(new Button(100,340,50,20,"A button2"));
	
	Widget *aslid = tab2->AddChild(new Slider(5,20,100,20,"Slider"));
	Widget *atext = tab2->AddChild(new Textbox(5,50,100,1,"Hello","Textbox"));
	Widget *albl = tab2->AddChild(new Label(5,80,"Hello"));
	
	while( !quit ) {
		quit = inputs.Update();
		
		Timer::Update();

		Video::Erase();

		UI::Draw();
		Video::Update();
		Timer::Delay();
	}
	return 0;
}
