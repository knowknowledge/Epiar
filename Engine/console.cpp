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
#include "Utilities/luamanager.h"
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
			if( i->kstate == KEYUP ) {
				if( i->key == SDLK_BACKQUOTE) {
					if( enabled ) {
						enabled = false;
					} else {
						enabled = true;
					}
				}
			} else if( i->kstate == KEYTYPED) {
				if( enabled ) {
					string back = Console::Buffer.back();
					back.erase(back.size() - 1);
					Console::Buffer.pop_back();

					switch(i->key) {
					case '\n':
						Console::Buffer.push_back(back);
						Lua::Run(back.substr(2));
						back = "> ";
					break;
					case '\b':
						if(back.size() > 2) back.erase(back.size() - 1);
					break;
					default:
						back += i->key;
					break;
					}
					back += "_";
					Console::Buffer.push_back(back);

					// remove it from the queue
					events.remove( *i );
					i = events.begin(); // removing elements mid-list upsets iterators
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
		Video::DrawRect(150, 5, 550, 100, .5, .5, .5, .3 );

		int pos = 8;
		for(int i = Console::Buffer.size() - 1; i >= 0; i--) {
			VeraMono10->Render(155, 18 + (pos * 10), (Console::Buffer[i]).c_str());
			pos--;
			if(pos < 0) break;
		}
	}
}

void Console::Update() {
	if( enabled ) {
		
	}
}

// used by lua functions, eg echo
void Console::InsertResult(string result) {
	// get the prompt off the buffer
	//string back = Console::Buffer.back();
	//back.erase(back.size() - 1);
	//Console::Buffer.pop_back();

	// insert result into buffer
	Console::Buffer.push_back(result);

	// insert prompt back into buffer
	//Console::Buffer.push_back(back);
}

