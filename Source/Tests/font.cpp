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
	int lh=SansSerif->LineHeight();
	int th=SansSerif->TightHeight();
	
	string str1=string("Left Aligned Text");
	string str2=string("Centered Aligned Text");
	string str3=string("Right Aligned Text");
	string str4=string("Top Aligned Text");
	string str5=string("Middle Aligned Text");
	string str6=string("Bottom Aligned Text");
	
	string str7=string("Tight text");
	string str8=string("Larger text");

	Video::DrawRect(100,100,250,lh,.4f, .4f, 1.f);
	Video::DrawRect(350,100,250,lh, 1.f, .4f,.4f);
	SansSerif->Render(100,100,str1);
	SansSerif->Render(350,100,str2,CENTER);
	SansSerif->Render(600,100,str3,RIGHT);
	
	Video::DrawRect(100,200,200,100, .4f, .4f, 1.f);
	Video::DrawRect(100,300,200,100, 1.f, .4f, .4f);
	Video::DrawRect(100,200,200,lh, .4f, 1.f, .4f);
	Video::DrawRect(100,300-lh/2,200,lh, 1.f, .4f, 1.f);
	Video::DrawRect(100,400-lh,200,lh, 1.f, .4f, 1.f);
	SansSerif->Render(200,200,str4,CENTER,TOP);
	SansSerif->Render(200,300,str5,CENTER,MIDDLE);
	SansSerif->Render(200,400,str6,CENTER,BOTTOM);

	Video::DrawRect(450,200,100,th,.3f,.3f,.3f);
	SansSerif->RenderTight(450,200,str7);
	
	SansSerif->SetSize(50);
	Video::DrawRect(350,300,SansSerif->TextWidth(str8),SansSerif->TightHeight(),.4f,.0f,.4f);
	SansSerif->RenderTight(350,300,str8);

	Video::Update();
	SDL_Event event;
	do{
		SDL_WaitEvent(&event);
	}while( event.type != SDL_KEYUP );
	return 0;
}
