/**\file			ui_button.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#ifndef __H_BUTTON__
#define __H_BUTTON__

#include "Audio/sound.h"
#include "Graphics/image.h"
#include "UI/ui.h"

class Button : public Widget {
	public:
		Button( int x, int y, int w, int h, string label, string lua_code="");
		Button( int x, int y, int w, int h, string label, void (*clickCallBack)(void*), void *callBackValue = NULL);
		~Button();
		
		void Draw( int relx = 0, int rely = 0 );

		void SetText(string text) { this->name = text; }
		string GetText() { return this->name; }

		virtual string GetType( void ) {return string("Button");}
		virtual int GetMask( void ) { return WIDGET_BUTTON; }

	protected:
		bool MouseLDown( int xi, int yi );
		bool MouseLUp( int xi, int yi );
		bool MouseLRelease( void );
		bool MouseEnter( int xi, int yi );
		bool MouseLeave( void );

	private:
		void Initialize( int x, int y, int w, int h, string label);
		Image *bitmap_current, *bitmap_normal, *bitmap_mouseover, *bitmap_pressed;
		Sound *sound_click, *sound_hover;
};

#endif // __H_BUTTON__
