/*
 * Filename      : debug.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, Ocotober 4, 2008
 * Last Modified : Saturday, October 4, 2008
 * Purpose       : 
 * Notes         :
 */

#include "includes.h"
#include "Utilities/debug.h"

bool Debug::active = false;

void Debug::Set() {
	Debug::active = true;
}

void Debug::Unset() {
	Debug::active = false;
}

// Displays debugging code only if debugging is set
void Debug::Print(const char *message, ...) {
	va_list args;

	return; // what's this function for? why is it not just Log() ?

	if(Debug::active) {
		va_start( args, message );
		vprintf( message, args );
		va_end( args );

		// Add \n if missing
		if(message[strlen(message) - 1] != '\n') printf("\n");
	}
}

