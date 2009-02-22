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

Console::Console( void ) {
	enabled = false;
}

void Console::Input( list<InputEvent> & events ) {
	if( enabled ) {

	}
}

void Console::Draw() {

}

void Console::Update() {

}

