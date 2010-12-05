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
		Console();

		void HandleInput( list<InputEvent> & events );
		void Draw();
		void Update();

	private:
		void RunCommand();
		void InsertResult(string result);

		vector<string> Buffer;
		bool enabled;
		string command;
		unsigned int cursor;
		static Font* consoleFont;
};

#endif // __h_console__

