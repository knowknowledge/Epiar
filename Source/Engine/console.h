/**\file			console.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, February 21, 2009
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */
#ifndef __h_console__
#define __h_console__

#include "Graphics/image.h"
#include "includes.h"
#include "Input/input.h"

class Console {
	public:
		static void Initialize();

		static void HandleInput( list<InputEvent> & events );
		static void Draw();
		static void Update();

		static void InsertResult(string result); // used by lua functions, eg echo

	private:
		static vector<string> Buffer;
		static bool enabled, initialized;
};

#endif // __h_console__

