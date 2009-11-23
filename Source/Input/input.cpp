/**\file			input.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/console.h"
#include "Input/input.h"
#include "Sprites/player.h"
#include "UI/ui.h"
#include "Utilities/log.h"
#include "Graphics/video.h"

/**\class Input
 * \brief Input handling. */

Input::Input() {
	memset( keyDown, 0, sizeof( bool ) * SDLK_LAST );
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
	
	// the list of sub-input systems that handle events
	UI::HandleInput( events ); // anything the UI doesn't care about will be left in the list for the next subsystem
	Console::Input( events );
	Handle( events ); // default handler. player motion is handled here

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
			keyDown[ event->key.keysym.sym ] = 1;
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
			keyDown[ event->key.keysym.sym ] = 0;
			break;
	}

	return quitSignal;
}

void Input::Handle( list<InputEvent> & events ) {
	Player *player = Player::Instance();

	if( keyDown[ SDLK_UP ] ) player->Accelerate();
	// TODO It shouldn't be possible to rotate in both directions at once
	if( keyDown[ SDLK_LEFT ] ) player->Rotate( 30.0 );
	if( keyDown[ SDLK_RIGHT ] ) player->Rotate( -30.0 );
	if( keyDown[ SDLK_DOWN ] ){ // Rotate in the opposite direction as you're moving
		player->Rotate( player->directionTowards( player->GetMomentum().GetAngle() + 180 ) );
	}
	// DEBUG CODE
	if( keyDown[ 'c' ] ){  // Rotate towards the center of the Universe
		player->Rotate( player->directionTowards( Coordinate(0,0) ) );
	}
}

void Input::PushTypeEvent( list<InputEvent> & events, SDLKey key ) {
	char *word = SDL_GetKeyName(key);
	char letter = 0;

	if((key == SDLK_LSHIFT) || (key == SDLK_RSHIFT)) return; // we don't care about modifiers here
	if(key == SDLK_BACKQUOTE) return;
	if(key == SDLK_LEFT) return;
	if(key == SDLK_RIGHT) return;
	if(key == SDLK_UP) return;
	if(key == SDLK_DOWN) return;

	if(key >= SDLK_a && key <= SDLK_z) {
		letter = word[0];
		if(keyDown[SDLK_LSHIFT] || keyDown[SDLK_RSHIFT]) {
			letter -= 32;
		}
	} else if(key == SDLK_SPACE) {
		letter = ' ';
	} else if(key >= SDLK_0 && key <= SDLK_9 && (keyDown[SDLK_LSHIFT] || keyDown[SDLK_RSHIFT])) {
		switch(key) {
			case SDLK_0:
				letter = ')';
			break;
			case SDLK_1:
				letter = '!';
			break;
			case SDLK_2:
				letter = '@';
			break;
			case SDLK_3:
				letter = '#';
			break;
			case SDLK_4:
				letter = '$';
			break;
			case SDLK_5:
				letter = '%';
			break;
			case SDLK_6:
				letter = '^';
			break;
			case SDLK_7:
				letter = '&';
			break;
			case SDLK_8:
				letter = '*';
			break;
			case SDLK_9:
				letter = '(';
			break;
			default: break; // will never happen, here to avoid compiler warnings
		}
	} else if((key == SDLK_QUOTE) && (keyDown[SDLK_LSHIFT] || keyDown[SDLK_RSHIFT])) {
		letter = '"';
	} else if(key == SDLK_RETURN) {
		letter = '\n';
	} else if((key == SDLK_SEMICOLON) && (keyDown[SDLK_LSHIFT] || keyDown[SDLK_RSHIFT])) {
		letter = ':';
	} else if(key == SDLK_BACKSPACE) {
		letter = '\b';
	} else if(key == SDLK_EQUALS && (keyDown[SDLK_LSHIFT] || keyDown[SDLK_RSHIFT])) {
		letter = '+';
	} else {
		letter = word[0];
	}

	events.push_front( InputEvent( KEY, KEYTYPED, letter ) );
}

