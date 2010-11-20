/**\file			ui_button.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Audio/sound.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/debug.h"
#include "Utilities/log.h"
#include "Utilities/lua.h"

/**\class Button
 * \brief UI button. */

/**\brief Convenience function to initialize the button, used to prevent code repetition.
 */
void Button::Initialize( int x, int y, int w, int h, string label ) {
	// This is the main Button Constructor
	// This cuts down on code duplication so it can be called by multiple constructors.
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	
	this->name = label;
	
	// Load the bitmaps needed for drawing
	bitmap_normal = Image::Get( "Resources/Graphics/ui_button.png" );
	bitmap_mouseover = Image::Get( "Resources/Graphics/ui_button_mouseover.png" );
	bitmap_pressed = Image::Get( "Resources/Graphics/ui_button_pressed.png" );
	bitmap_current = bitmap_normal;

	assert(bitmap_normal);
	assert(bitmap_mouseover);
	
	// Load sounds
	this->sound_click = Sound::Get( "Resources/Audio/Interface/28853__junggle__btn043.ogg" );
	this->sound_hover = Sound::Get( "Resources/Audio/Interface/28820__junggle__btn010.ogg" );

	this->clickCallBack = NULL;
	this->lua_callback = "";
}

/**\brief Constructs a button with a C++ callback.*/
Button::Button( int x, int y, int w, int h, string label, void (*function)(void)) {
	Initialize( x, y, w, h, label );
	this->clickCallBack = function;
}

/**\brief Constructs a button with a Lua callback.*/
Button::Button( int x, int y, int w, int h, string label, string lua_code) {
	Initialize( x, y, w, h, label );
	this->lua_callback = lua_code;
}

Button::~Button() {
//	DO NOT DO THIS - it's just a pointer, never allocated - delete bitmap_current;
	//delete bitmap_normal;
	//delete bitmap_pressed;
	//delete sound_click;
	//delete sound_hover;

	bitmap_normal = bitmap_pressed = NULL;
	sound_click = sound_hover = NULL;
}

/**\brief Draws the button.*/
void Button::Draw( int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;
	
	Video::DrawRect( x, y, this->w, this->h, 1., 1., 1. );

	assert(bitmap_current);

	// draw the button (loaded image is simply scaled)
	bitmap_current->DrawStretch( x, y, this->w, this->h );

	// draw the label
	Video::SetCropRect(x + 1, y + 1, this->w - 2, this->h - 2); // constants adjust for the 1px border
	SansSerif->SetColor( 1., 1., 1. );
	SansSerif->RenderTight( x + (w / 2), y + (h / 2), this->name, Font::CENTER,Font::MIDDLE );
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}

/**\brief When Left mouse is down on the button.*/
bool Button::MouseLDown( int xi, int yi ) {
	if(OPTION(int, "options/sound/buttons")) this->sound_click->Play();

	bitmap_current = bitmap_pressed;

	return true;
}

/**\brief When left mouse is back up on the button.*/
bool Button::MouseLUp( int xi, int yi ) {
	bitmap_current = bitmap_mouseover;

	if( clickCallBack ){
		LogMsg(INFO, "Clicked on: '%s'.", this->name.c_str() );
		clickCallBack();
	} else if("" != lua_callback){
		LogMsg(INFO,"Clicked on '%s'. Running '%s'", this->name.c_str(), (char *)lua_callback.c_str() );
		Lua::Run(lua_callback);
	} else {
		LogMsg(WARN, "Clicked on: '%s' but there was no function to call.", this->name.c_str() );
	}
	return true;
}

bool Button::MouseLRelease( void ){
	bitmap_current = bitmap_normal;
	return true;
}

/**\brief Event is triggered on mouse enter.
 */
bool Button::MouseEnter( int xi, int yi ){
	bitmap_current = bitmap_mouseover;
	hovering = true;
	LogMsg(INFO,"Mouse enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Event is triggered on mouse leave.
 */
bool Button::MouseLeave( void ){
	bitmap_current = bitmap_normal;
	hovering = false;
	LogMsg(INFO,"Mouse leave detect in %s.",this->name.c_str());
	return true;
}
