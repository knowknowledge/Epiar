/**\file			ui_window.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "UI/ui_window.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Window
 * \brief Window handling. */

/**\brief Creates a new window with specified parameters centered on screen.
 */
Window::Window( int w, int h, string caption ):
		draggable( true )
{

	this->x = Video::GetHalfWidth() - (w / 2);
	this->y = Video::GetHalfHeight() - (h / 2);
	this->w = w;
	this->h = h;
	this->name = caption;

	// Load the bitmaps needed for drawing
	bitmaps[0] = Image::Get( "Resources/Skin/ui_wnd_up_left.png" );
	bitmaps[1] = Image::Get( "Resources/Skin/ui_wnd_up.png" );
	bitmaps[2] = Image::Get( "Resources/Skin/ui_wnd_up_right.png" );
	bitmaps[3] = Image::Get( "Resources/Skin/ui_wnd_left.png" );
	bitmaps[4] = Image::Get( "Resources/Skin/ui_wnd_right.png" );
	bitmaps[5] = Image::Get( "Resources/Skin/ui_wnd_low_left.png" );
	bitmaps[6] = Image::Get( "Resources/Skin/ui_wnd_low.png" );
	bitmaps[7] = Image::Get( "Resources/Skin/ui_wnd_low_right.png" );
	bitmaps[8] = Image::Get( "Resources/Skin/ui_wnd_back.png" );

	closeButton = NULL;
}

/**\brief Creates a new window with specified parameters.
 */
Window::Window( int x, int y, int w, int h, string caption ):
		draggable( true )
{

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->name = caption;

	// Load the bitmaps needed for drawing
	bitmaps[0] = Image::Get( "Resources/Skin/ui_wnd_up_left.png" );
	bitmaps[1] = Image::Get( "Resources/Skin/ui_wnd_up.png" );
	bitmaps[2] = Image::Get( "Resources/Skin/ui_wnd_up_right.png" );
	bitmaps[3] = Image::Get( "Resources/Skin/ui_wnd_left.png" );
	bitmaps[4] = Image::Get( "Resources/Skin/ui_wnd_right.png" );
	bitmaps[5] = Image::Get( "Resources/Skin/ui_wnd_low_left.png" );
	bitmaps[6] = Image::Get( "Resources/Skin/ui_wnd_low.png" );
	bitmaps[7] = Image::Get( "Resources/Skin/ui_wnd_low_right.png" );
	bitmaps[8] = Image::Get( "Resources/Skin/ui_wnd_back.png" );

	closeButton = NULL;
}

Window::~Window() {
	bitmaps[0] = NULL;
	bitmaps[1] = NULL;
	bitmaps[2] = NULL;
	bitmaps[3] = NULL;
	bitmaps[4] = NULL;
	bitmaps[5] = NULL;
	bitmaps[6] = NULL;
	bitmaps[7] = NULL;
	bitmaps[8] = NULL;
	closeButton = NULL; // Let the Container Destructor delete the button
}

/**\brief Adds a widget to the current Window.
 */
Window *Window::AddChild( Widget *widget ){
	assert( widget != NULL );
	Container::AddChild( widget );
	return this;
}

/**\brief Draws the current window.
 */
void Window::Draw( int relx, int rely ) {
	int x, y;
	static float alpha = 0.95f;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// Draw the background
	bitmaps[8]->DrawTiled( x + bitmaps[3]->GetWidth(), y + bitmaps[1]->GetHeight(), w - bitmaps[3]->GetWidth() - bitmaps[4]->GetWidth(), h - bitmaps[1]->GetHeight() - bitmaps[6]->GetHeight(), alpha );
	
	// Draw the top section
	bitmaps[0]->Draw( x, y );
	bitmaps[1]->DrawTiled( x + bitmaps[0]->GetWidth(), y, w - bitmaps[0]->GetWidth() - bitmaps[2]->GetWidth(), bitmaps[1]->GetHeight() );
	bitmaps[2]->Draw( x + w - bitmaps[2]->GetWidth(), y );
	
	// Draw the left and right sections
	bitmaps[3]->DrawTiled( x, y + bitmaps[0]->GetHeight(), bitmaps[3]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight(), alpha );
	bitmaps[4]->DrawTiled( x + w - bitmaps[4]->GetWidth(), y + bitmaps[0]->GetHeight(), bitmaps[4]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight(), alpha );
	
	// Draw the bottom section
	bitmaps[5]->DrawAlpha( x, y + h - bitmaps[5]->GetHeight(), alpha );
	bitmaps[6]->DrawTiled( x + bitmaps[5]->GetWidth(), y + h - bitmaps[6]->GetHeight(), w - bitmaps[5]->GetWidth() - bitmaps[7]->GetWidth(), bitmaps[6]->GetHeight(), alpha );
	bitmaps[7]->DrawAlpha( x + w - bitmaps[7]->GetWidth(), y + h - bitmaps[7]->GetHeight(), alpha );

	// Draw the window title
	UI::font->RenderTight(x + (w / 2), y + bitmaps[1]->GetHalfHeight(), name, Font::CENTER,Font::MIDDLE);

	Container::Draw(relx,rely);
}

bool Window::MouseDrag( int xi, int yi ){
	// Only drag by titlebar
	if ( dragY < bitmaps[1]->GetHeight() && draggable == true ) {
		Widget::MouseDrag( xi, yi );
		x = xi - dragX;
		y = yi - dragY;
	} else {
		// Pass the event onto widget if not handling it.
		Container::MouseDrag( x, y );
	}
	
	return true;
}

/**\brief Toggle on or off the ability to drag this window by the top bar.
 */
bool Window::SetDragability( bool _draggable ){
	draggable = _draggable;
	return true;
}

/**\brief Close the Window
 */
void Window::CloseWindow( void* win)
{
	UI::Close((Window*)win);
}

/**\brief Add a Close Button to this Window
 * \todo On windows that scroll, having the Close Button scroll with the Window is wrong. How to Fix?
 */
void Window::AddCloseButton() {
	closeButton = new Button( this->GetW() - 28, 5, 15, 15, "x", Window::CloseWindow, this );
	AddChild( closeButton );
}

/** @} */
