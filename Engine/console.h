/*
 * Filename      : console.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Saturday, February 21, 2009
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_console__
#define __h_console__

#include "Graphics/image.h"
#include "includes.h"
#include "Input/input.h"

class Console {
	public:
		static void Initialize();

		static void Input( list<InputEvent> & events );
		static void Draw();
		static void Update();

	private:
		static vector<string> Buffer;
		static bool enabled, initialized;
};

#endif // __h_console__

