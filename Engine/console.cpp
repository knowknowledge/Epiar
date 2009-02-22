/*
 * Filename      : console.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, February 21, 2009
 * Purpose       : Handles the Heads-Up-Display
 * Notes         :
 */

#include "common.h"
#include "Engine/console.h"
#include "Graphics/video.h"
#include "includes.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"

vector<string> Console::Buffer;
bool Console::enabled = false;
bool Console::initialized = false;

void Console::Initialize() {
	Console::Buffer.push_back("Console initialized.");
	Console::Buffer.push_back("> _");
	Console::initialized = true;
}

void Console::Input( list<InputEvent> & events ) {
	list<InputEvent>::iterator i;

	if(Console::initialized == false) Console::Initialize();

	// look for the bcakquote (`) key to toggle the console
	for( i = events.begin(); i != events.end(); ++i ) {
		switch( i->type ) {
		case KEY:
			if( i->kstate == KEYTYPED ) {
				if( i->key == SDLK_BACKQUOTE) {
					if( enabled ) {
						enabled = false;
					} else {
						enabled = true;
					}
				}
				if( enabled ) {
					if(( i->key >= SDLK_a && i->key <= SDLK_z ) || (i->key == SDLK_SPACE) || (i->key == SDLK_EQUALS) || (i->key == SDLK_COMMA) || (i->key == SDLK_LEFTPAREN) || (i->key == SDLK_RIGHTPAREN) || (i->key == SDLK_RETURN)) {
						string back = Console::Buffer.back();
						back.erase(back.size() - 1);

						switch(i->key) {
						case SDLK_SPACE:
							back += " ";
						break;
						case SDLK_LEFTPAREN:
							back += "(";
						break;
						case SDLK_RIGHTPAREN:
							back += ")";
						break;
						case SDLK_RETURN:
							Console::Buffer.push_back(back);
							back = "> ";
						break;
						default:
							back += SDL_GetKeyName(i->key);
						break;
						}
						back += "_";
						Console::Buffer.pop_back();
						Console::Buffer.push_back(back);

						// remove it from the queue
						events.remove( *i );
						i = events.begin(); // removing elements mid-list upsets iterators
					}
				}
			}
		break;
		default:
		break;
		}
	}
}

void Console::Draw() {
	if( enabled ) {
		// draw bg
		Video::DrawRect(150, 5, 450, 100, .5, .5, .5, .3 );

		int pos = 8;
		for(int i = Console::Buffer.size() - 1; i >= 0; i--) {
			VeraMono10->Render(155, 18 + (pos * 10), (Console::Buffer[i]).c_str());
			pos--;
		}
	}
}

void Console::Update() {
	if( enabled ) {
		
	}
}

