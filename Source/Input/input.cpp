/**\file			input.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Engine/console.h"
#include "Input/input.h"
#include "Sprites/player.h"
#include "UI/ui.h"
#include "Utilities/log.h"
#include "Graphics/video.h"
#include "Engine/simulation.h"
#include "Engine/hud.h"
#include "Utilities/lua.h"
#include "Utilities/timer.h"

map<InputEvent, string> Input::eventMappings;
Uint32 Input::lastMouseMove= 0;

ostream& operator<<(ostream &out, const InputEvent&e) {
	static const char _mouseMeanings[3] = {'M','U','D'};
	static const char _keyMeanings[4] = {'^','V','P','T'};
	if ( e.type == KEY ) {
		out << "KEY([" << SDL_GetKeyName(e.key) << "] " << e.key << ' ' << _keyMeanings[int( e.kstate )] << ")";
	} else { // Mouse
		out << "MOUSE(" << e.mx<< ',' << e.my << ' ' << _mouseMeanings[int( e.mstate )] << ")";
	}
	return out;
}

Input::Input() {
	memset( heldKeys, 0, sizeof( bool ) * SDLK_LAST );
}

bool Input::Update( void ) {
	SDL_Event event;
	bool quitSignal = false;

	while( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
			case SDL_QUIT:
				quitSignal = true;
				break;
			case SDL_KEYDOWN:
			{
				bool quitSignalUpdate = _UpdateHandleKeyDown( &event );
				if( quitSignalUpdate ) quitSignal = quitSignalUpdate;
				break;
			}
			case SDL_KEYUP:
			{
				bool quitSignalUpdate = _UpdateHandleKeyUp( &event );
				if( quitSignalUpdate ) quitSignal = quitSignalUpdate;
				break;
			}
			case SDL_MOUSEMOTION:
			{
				_UpdateHandleMouseMotion( &event );
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				_UpdateHandleMouseUp( &event );				
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				_UpdateHandleMouseDown( &event );				
				break;
			}
			default:
				break;
		}
	}

	// Constantly emit InputEvent for held down Keys
	for(int k=0;k<SDLK_LAST;k++) {
		if(heldKeys[k])
			events.push_back( InputEvent( KEY, KEYPRESSED, k ) );
	}
	
	// the list of sub-input systems that handle events
	UI::HandleInput( events ); // anything the UI doesn't care about will be left in the list for the next subsystem
	Console::HandleInput( events );
	Hud::HandleInput( events );
	HandleLuaCallBacks( events );

	if((Timer::GetTicks() - lastMouseMove > OPTION(Uint32,"options/timing/mouse-fade")) && !UI::Active()){
		Video::DisableMouse();
	}

	events.clear();
	
	// this could be false - returning quitSignal doesn't imply quitting
	return quitSignal;
}

void Input::_UpdateHandleMouseMotion( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;

	// translate so (0,0) is lower-left of screen
	x = event->motion.x;
	y = event->motion.y;
	
	events.push_front( InputEvent( MOUSE, MOUSEMOTION, x, y ) );
	Video::EnableMouse();
	lastMouseMove = Timer::GetTicks();
}

void Input::_UpdateHandleMouseDown( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;
	
	// translate so (0,0) is lower-left of screen
	x = event->button.x;
	y = event->button.y;

	events.push_front( InputEvent( MOUSE, MOUSEDOWN, x, y ) );
}

void Input::_UpdateHandleMouseUp( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;
	
	// translate so (0,0) is lower-left of screen					
	x = event->button.x;
	y = event->button.y;

	events.push_front( InputEvent( MOUSE, MOUSEUP, x, y ) );
}

bool Input::_UpdateHandleKeyDown( SDL_Event *event ) {
	bool quitSignal = false;
	
	assert( event );

	switch( event->key.keysym.sym ) {
		case SDLK_ESCAPE:
			quitSignal = true;
			break;
		default:
			events.push_front( InputEvent( KEY, KEYDOWN, event->key.keysym.sym ) );
			// typed events go here because SDL will repeat KEYDOWN events for us at the set SDL repeat rate
			PushTypeEvent( events, event->key.keysym.sym );
			heldKeys[ event->key.keysym.sym ] = 1;
			break;
	}

	return quitSignal;
}

bool Input::_UpdateHandleKeyUp( SDL_Event *event ) {
	bool quitSignal = false;
	
	assert( event );

	switch( event->key.keysym.sym ) {
		case SDLK_ESCAPE:
			quitSignal = true;
			break;
		default:
			events.push_front( InputEvent( KEY, KEYUP, event->key.keysym.sym ) );
			heldKeys[ event->key.keysym.sym ] = 0;
			break;
	}

	return quitSignal;
}

void Input::PushTypeEvent( list<InputEvent> & events, SDLKey key ) {
	int letter=key;

	// Convert lower to upper case characters
	// TODO: This assumes that every Epiar user has an American keyboard.
	//       I don't know how to do the keyboard independent character translations.
	if(heldKeys[SDLK_LSHIFT] || heldKeys[SDLK_RSHIFT]) {
		if(key >= SDLK_a && key <= SDLK_z) {
			letter -= 32;
		} else if(key >= SDLK_0 && key <= SDLK_9) {
			letter = ")!@#$%^&*("[key-SDLK_0];
		} else {
			switch(key){
			case SDLK_QUOTE:
				letter = '"'; break;
			case SDLK_SEMICOLON:
				letter = ';'; break;
			case SDLK_BACKQUOTE:
				letter = '~'; break;
			case SDLK_MINUS:
				letter = '_'; break;
			case SDLK_SLASH:
				letter = '?'; break;
			case SDLK_COMMA:
				letter = '<'; break;
			case SDLK_PERIOD:
				letter = '>'; break;
			case SDLK_BACKSLASH:
				letter = '|'; break;
			case SDLK_LEFTBRACKET:
				letter = '{'; break;
			case SDLK_RIGHTBRACKET:
				letter = '}'; break;
			case SDLK_EQUALS:
				letter = '+'; break;
			default:
				break;
			}
		}
	}

	// Keypresses that we want to accept, but turn into something different
	if((key == SDLK_RETURN) || (key == SDLK_KP_ENTER)) {
		letter='\n';
	}

	// DEBUG: Name = int = char -> emitted char
	//cout<<SDL_GetKeyName(key)<<" = "<<key<<" = '"<<char(key)<<"' -> '"<<char(letter)<<"'"<<endl;
	
	events.push_front( InputEvent( KEY, KEYTYPED, letter ) );
}

void Input::HandleLuaCallBacks( list<InputEvent> & events ) {
	for( list<InputEvent>::iterator i = events.begin(); i != events.end(); ++i) {
		//cout << *i << endl; // DEBUG code to check which events are actually being generated
		map<InputEvent,string>::iterator val = eventMappings.find( *i );
		if( val != eventMappings.end() ){
			Lua::Run( val->second );
		}
	}
}

void Input::RegisterCallBack( InputEvent event, string command ) {
	cout<<"Registering: "<<event<<" as "<<command<<endl;
	eventMappings.insert(make_pair(event, command));
}

void Input::UnRegisterCallBack( InputEvent event ) {
	cout<<"Un-Registering: "<<event<<endl;
	eventMappings.erase(event);
}

int Input::RegisterKey(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if(n == 3) {
		int triggerKey;
		if( lua_isnumber(L,1) ) {
			triggerKey = (int)(luaL_checkint(L,1));
		} else {
			triggerKey = (int)(luaL_checkstring(L,1)[0]);
		}
		keyState triggerState = (keyState)(luaL_checkint(L,2));
		string command = (string)luaL_checkstring(L,3);
		RegisterCallBack(InputEvent(KEY, triggerState, triggerKey), command);
	} else {
		luaL_error(L, "Got %d arguments expected 3 (Key, State, Command)", n); 
	}
	return 0;
}

int Input::UnRegisterKey(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if(n == 2) {
		int triggerKey;
		if( lua_isnumber(L,1) ) {
			triggerKey = (int)(luaL_checkint(L,1));
		} else {
			triggerKey = (int)(luaL_checkstring(L,1)[0]);
		}
		keyState triggerState = (keyState)(luaL_checkint(L,2));
		UnRegisterCallBack(InputEvent(KEY, triggerState, triggerKey));
	} else {
		luaL_error(L, "Got %d arguments expected 2 (Key, State)", n); 
	}
	return 0;
}
