/**\file			ui_scrollbar.cpp
 * \author			Maoserr
 * \date			Created: Tuesday, March 16, 2010
 * \date			Modified: Tuesday, March 16, 2010
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/log.h"

#define SCROLLBAR_MINMARK	10	///< Minimum marker size

/**\class Scrollbar
 * \brief Implements a scrollbar that can be attached to certain widgets.
 */

/**\brief Attaches Scrollbar to the widget specified.
 * \note Unlike all other widgets, the x value here represents the right side of the widget.
 *       This is used to align the Scrollbar with the edge of a Container.
 */
Scrollbar::Scrollbar( int x, int y, int length, int maxpos):
		pos( 0 ),
		maxpos( maxpos )
{
	this->x = x;
	this->y = y;
	this->name = "Vertical";

	assert(maxpos != 0);

	// This is the Scrollbar Background Image.
	bitmaps[0] = Image::Get("Resources/Graphics/ui_scrollbar_bg.png");

	// These are the Up and down Arrows
	bitmaps[1] = Image::Get("Resources/Graphics/ui_scrollbar_up.png");
	bitmaps[2] = Image::Get("Resources/Graphics/ui_scrollbar_down.png");

	// These Describe the the Marker / Handle
	bitmaps[3] = Image::Get("Resources/Graphics/ui_scrollbar_handle_up.png");
	bitmaps[4] = Image::Get("Resources/Graphics/ui_scrollbar_handle_bg.png");
	bitmaps[5] = Image::Get("Resources/Graphics/ui_scrollbar_handle_down.png");

	this->x -= bitmaps[0]->GetWidth();

	SetSize(length);
}

/**\brief Scrollbar Destructor
 */
Scrollbar::~Scrollbar() {
	// Set these bitmaps to NULL, but don't delete them.
	bitmaps[0] = NULL;
	bitmaps[1] = NULL;
	bitmaps[2] = NULL;
	bitmaps[3] = NULL;
	bitmaps[4] = NULL;
	bitmaps[5] = NULL;
}

/**\brief Set the Length and Height of a Scrollbar.
 */

void Scrollbar::SetSize(int length) {
	this->w = bitmaps[0]->GetWidth();
	this->h = length;
}

/**\brief Draws the scrollbar.
 */
void Scrollbar::Draw( int relx, int rely ){
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	// Draw the Background
	bitmaps[0] -> DrawTiled( x, y, w, h );

	// Draw the Arrows
	bitmaps[1] -> Draw( x, y );
	bitmaps[2] -> Draw( x, y + h - bitmaps[2]->GetHeight() );

	// Draw the Marker
	bitmaps[3] -> Draw( x, y + MarkerPosToPixel() );
	bitmaps[4] -> DrawTiled( x, y + MarkerPosToPixel() + bitmaps[3]->GetHeight(), w, GetMarkerSize() - bitmaps[3]->GetHeight() - bitmaps[5]->GetHeight()  );
	bitmaps[5] -> Draw( x, y + MarkerPosToPixel() + GetMarkerSize() - bitmaps[5]->GetHeight() );

	Widget::Draw( relx, rely );
}

/**\brief Scroll to position on mouse down.
 */
bool Scrollbar::MouseLDown( int x, int y ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	int newpos;

	if( yr < bitmaps[1]->GetHeight() ) {
		// Click on the UP arrow
		newpos = 0; //pos - SCROLLBAR_SCROLL;
	} else if ( yr > (h - bitmaps[2]->GetHeight()) ) {
		// Click on the Down arrow
		newpos = maxpos; // pos + SCROLLBAR_SCROLL;
	} else {
		// Click somewhere in the middle
		newpos = MarkerPixelToPos( xr, yr );
	}

	pos = CheckPos( newpos );
	return true;
}

/**\brief Scroll to position on mouse motion.
 */
bool Scrollbar::MouseDrag( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - GetX();
	int yr = yi - GetY();

	this->pos = this->CheckPos(this->MarkerPixelToPos( xr, yr ));
	return true;
}

/**\brief Scroll the scrollbar up.*/
void Scrollbar::ScrollUp( int pix ){
	int newpos = pos-pix;
	this->pos = this->CheckPos( newpos );
}

/**\brief Scroll the scrollbar down.*/
void Scrollbar::ScrollDown( int pix ){
	int newpos = pos+pix;
	this->pos = this->CheckPos( newpos );
}

/**\brief Calculates marker size based on current dimensions.
 */
int Scrollbar::GetMarkerSize( void ){
	int visiblelen;

	visiblelen = this->h;

	// Calculate the size of the marker
	int markersize = this->h * visiblelen / maxpos;

	// Don't have super small marker size
	if ( markersize < SCROLLBAR_MINMARK )
		markersize = SCROLLBAR_MINMARK;

	return markersize;
}

/**\brief Calculates the marker position in pixel offset.
 */
int Scrollbar::MarkerPosToPixel( void ){
	int markerpos;
	int effectivelen;
	float posratio;		// 0 - 1 ratio of marker position

	effectivelen = this->h - GetMarkerSize() - bitmaps[1]->GetHeight() - bitmaps[2]->GetHeight();
	posratio = TO_FLOAT(pos) / TO_FLOAT(maxpos - (this-> h ));

	markerpos = bitmaps[1]->GetHeight() + static_cast<int>(effectivelen* posratio);

	return markerpos;
}

/**\brief Calculates the marker position from pixel offset.
 */
int Scrollbar::MarkerPixelToPos( int xr, int yr ){
	int effectivelen;
	int newpos;
	int marksize = this->GetMarkerSize();
	int effectivestart = bitmaps[2]->GetHeight() + marksize / 2;

	effectivelen = this->h - marksize - bitmaps[1]->GetHeight() - bitmaps[2]->GetHeight();
	newpos = TO_INT(TO_FLOAT(yr - effectivestart) / TO_FLOAT(effectivelen) * (maxpos - h));

	return newpos;
}


/**\brief Checks that the pos value is valid.
 */
int Scrollbar::CheckPos( int newpos ){
	if ( newpos <= 0 )
		newpos = 0;
	else if ( newpos > (maxpos - h) )
		newpos = (maxpos - h);

	return newpos;
}
