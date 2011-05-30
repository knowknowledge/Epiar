/**\file			input.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef __h_input__
#define __h_input__

#include "includes.h"
#include "Utilities/lua.h"

// Stores information about an input event.
// We use this to build up a list of events and allow multiple sub-input systems
// to dispatch the events at their discretion.

// KEYUP and KEYDOWN are the physical events. KEYPRESSED refers to a key simply being down
// and should fire off every input loop that the key is down (this may not be programmed yet).
// KEYTYPED refers to a key being typed, which is fired off the moment the key is pressed down
// and then, after a delay, fired off at an even interal for a key repeat rate (e.g. holding
// down a key in your editor would produce a series of KEYTYPED events)
typedef enum {KEYUP, KEYDOWN, KEYPRESSED, KEYTYPED} keyState;
typedef enum {	UNHANDLED=0,
				MOUSEMOTION,
				MOUSELUP,		// Left mouse
				MOUSELDOWN,
				MOUSEMUP,		// Middle mouse
				MOUSEMDOWN,
				MOUSERUP,		// Right mouse
				MOUSERDOWN,
// Since wheel scrolls fire off two events consecutively,
// We'll only need to handle one.
				MOUSEWUP,		// Wheel
				MOUSEWDOWN
			} mouseState;
typedef enum {KEY, MOUSE} eventType;

class InputEvent {
	public:
		friend bool operator==(const InputEvent& e1, const InputEvent& e2 ) {
			if( e1.type != e2.type ) return( false );
			
			if( e1.type == KEY ) {
				if( e1.key != e2.key ) return( false );
				if( e1.kstate != e2.kstate ) return( false );
			} else {
				if( e1.mstate != e2.mstate ) return( false );
				if( e1.mx != e2.mx ) return( false );
				if( e1.my != e2.my ) return( false );
			}
			
			return( true );
		}

		friend bool operator <(const InputEvent& e1, const InputEvent& e2 ) {
			if( e1.type != e2.type ) return( e1.type < e2.type );
			if( e1.type == KEY ) {
				if( e1.key != e2.key ) return( e1.key < e2.key );
				if( e1.kstate != e2.kstate ) return( e1.kstate < e2.kstate );
			} else {
				if( e1.mstate != e2.mstate ) return( e1.mstate < e2.mstate  );
				if( e1.mx != e2.mx ) return( e1.mx < e2.mx );
				if( e1.my != e2.my ) return(e1.my < e2.my );
			}
			return false;
		}

		InputEvent( eventType type, keyState kstate, SDLKey key ) {
			this->type = type;
			this->key = key;
			this->kstate = kstate;
		}
		InputEvent( eventType type, keyState kstate, int key ) {
			this->type = type;
			this->key = (SDLKey)key;
			this->kstate = kstate;
		}
		InputEvent( eventType type, mouseState mstate, int mx, int my ) {
			this->type = type;
			this->mstate = mstate;
			this->mx = mx;
			this->my = my;
		}

		eventType type; // keyboard or mouse event?
		SDLKey key; // which key?
		keyState kstate; // just went up (fires once), just went down (fires once), or is pressed down currently (fires many times)
		mouseState mstate;
		int mx, my; // mouse coords
};

ostream& operator<<(ostream &out, const InputEvent&e);

class Input {
	public:
		Input();
		list<InputEvent> Update( void );

		void HandleLuaCallBacks( list<InputEvent> & events );
		void RegisterCallBack( InputEvent key, string command );
		void UnRegisterCallBack( InputEvent key );

		static bool SearchSpecificEvent( list<InputEvent> & events, InputEvent key );
		static bool HandleSpecificEvent( list<InputEvent> & events, InputEvent key );
		static void PrintEvents( string title, list<InputEvent> & events );
	
	private:
		mouseState _CheckMouseState( Uint8 button, bool up );
		void _UpdateHandleSignal( SDL_Event *event );
		void _UpdateHandleKeyDown( SDL_Event *event );
		void _UpdateHandleKeyUp( SDL_Event *event );
		void _UpdateHandleMouseDown( SDL_Event *event );
		void _UpdateHandleMouseUp( SDL_Event *event );
		void _UpdateHandleMouseMotion( SDL_Event *event );

		void PushTypeEvent( list<InputEvent> & events, SDLKey key );

		bool heldKeys[SDLK_LAST]; // set to true as long as a key is held down
		list<InputEvent> events; // a list of all the events that occurred for this loop. we pass this list around to various sub-input systems
		map<InputEvent,string> eventMappings; // Lua callbacks mapped to a key
		Uint32 lastMouseMove;
};

#endif // __h_input__
