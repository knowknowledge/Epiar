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

	Window *awin = new Window(0,0,200,400,"A Window");
	UI::Add(awin);
	Tabs *tabcont = new Tabs(5,25,180,300,"Tabs");
	awin->AddChild(tabcont);
	Tab *tab1 = new Tab("Tab1");
	tabcont->AddChild(tab1);
	Tab *tab2 = new Tab("Tab2");
	tabcont->AddChild(tab2);

	Picture *aimg = new Picture(50,100,50,50,"Resources/Graphics/corvet.png");
	tab1->AddChild(aimg);
	Checkbox *achk = new Checkbox(10,120,true,"Hello");
	tab1->AddChild(achk);
	Button *abut = new Button(100,300,50,20,"A button");
	tab1->AddChild(abut);
	Button *abut2 = new Button(100,340,50,20,"A button2");
	tab1->AddChild(abut2);
	
	Slider *aslid = new Slider(5,20,100,20,"Slider");
	tab2->AddChild(aslid);
	Textbox *atext = new Textbox(5,50,100,1,"Hello","Textbox");
	tab2->AddChild(atext);
	Label *albl = new Label(5,80,"Hello");
	tab2->AddChild(albl);
	
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
