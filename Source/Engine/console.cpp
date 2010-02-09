/**\file			console.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Saturday, February 21, 2009
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Engine/console.h"
#include "Graphics/video.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"
#include "Utilities/timer.h"

/**\class Console
 * \brief Handles the Heads-Up-Display. */

vector<string> Console::Buffer;
bool Console::enabled = false;
bool Console::initialized = false;

/**\brief Initialize Console instance.
 */
void Console::Initialize() {
	Console::Buffer.push_back("Console initialized.");
	Console::Buffer.push_back("> _");
	Console::initialized = true;
}

/**\brief Handles a list of Input events.
 * \param events A list of events
 */
void Console::HandleInput( list<InputEvent> & events ) {
	if(Console::initialized == false) Console::Initialize();

	// look for the bcakquote (`) key to toggle the console
	for( list<InputEvent>::iterator i = events.begin(); i != events.end(); ) {
		bool skipIncrement = false;

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
					// Ignore Modifiers
					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
					case SDLK_RMETA:
					case SDLK_LMETA:
					case SDLK_RALT:
					case SDLK_LALT:
					case SDLK_RCTRL:
					case SDLK_LCTRL:
					case SDLK_RSUPER:
					case SDLK_LSUPER:
					// TODO: add cursor movement support
					case SDLK_LEFT:
					case SDLK_RIGHT:
					case SDLK_UP:
					case SDLK_DOWN:
						break;
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
					i = events.erase( i );
					skipIncrement = true;
					
				}
			}
		break;
		default:
		break;
		}

		if(!skipIncrement)
			++i;
	}
}

/**\brief Draws the current console.
 */
void Console::Draw() {
	if( enabled ) {
		// draw bg
		Video::DrawRect(150, 5, 550, 100, 
				static_cast<float>(.5), static_cast<float>(.5),
				static_cast<float>(.5), static_cast<float>(.3) );

		int pos = 8;
		for(int i = Console::Buffer.size() - 1; i >= 0; i--) {
			Mono->Render(155, 18 + (pos * 10), (Console::Buffer[i]).c_str());
			pos--;
			if(pos < 0) break;
		}
	}
}

/**\brief Console update function.
 */
void Console::Update() {
	if( enabled ) {
		
	}
}


/**\brief Used by lua functions, eg echo.
 */
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

