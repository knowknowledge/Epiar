/**\file			ui_tabs.cpp
 * \author			Maoserr
 * \date			Created: Sunday, March 14, 2010
 * \date			Modified: Sunday, March 14, 2010
 * \brief			Implements Tab pages
 */

#include "includes.h"
#include "common.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "UI/ui_tabs.h"
#include "Utilities/log.h"

#define TAB_HEADER 20
#define TAB_PAD 5

/**\class Tab
 * \brief A single tab.
 */

/**\brief Constructs a single tab with caption.
 */
Tab::Tab( const string& _caption ):
	caption( string(_caption) ){
	SetX(0);
	SetY(TAB_HEADER);
}

/**\brief Empty destructor.
 */
Tab::~Tab(){
}

/**\brief Constructs a tab collection (no caption), you could add a label though.
 * \param name (not shown) Tabs collection don't really have a caption.
 */
Tabs::Tabs( int x, int y, int w, int h, const string& name ):
	w ( w ), h ( h ),activetab( NULL ){
	SetX( x );
	SetY( y );
	
	this->name = name;
}

/**\brief Adds a Tab to the Tabs collection.
 */
bool Tabs::AddChild( Widget *widget ){
	if ( widget->GetName().find("Tab_") != 0 ){
		Log::Error("Error attempted to add non-Tab widget to Tab container: %s",
				widget->GetName().c_str());
		return false;
	}

	Widget::AddChild( widget );
	if ( activetab == NULL)
		this->activetab = (Tab*) widget;

	((Tab *) widget)->w = this->w;
	((Tab *) widget)->h = this->h-TAB_HEADER;

	return true;
}

/**\brief Goes to next Tab in the container.
 */
void Tabs::TabNext( void ){
	list<Widget *>::iterator i;

	for( i = Widget::children.begin(); i != Widget::children.end(); ++i ) {
		if ( ((Tab*)*i) == activetab ){
			if ( (++i) == Widget::children.end() )
				this->activetab = (Tab*) (Widget::children.front());
			else
				this->activetab = ((Tab*)*i);
			break;
		}
	}
}

/**\brief Empty destructor.
 */
Tabs::~Tabs(){
}

/**\brief Tabs drawing function.
 */
void Tabs::Draw( int relx, int rely ){
	// Draw border first
	int x = GetX() + relx;
	int y = GetY() + rely;

	list<Widget *>::iterator i;

	int xo = 0;
	for( i = Widget::children.begin(); i != Widget::children.end(); ++i ) {
		Rect bounds = SansSerif->BoundingBox( ((Tab*)*i)->caption.c_str() );
		Video::DrawRect( xo+x,y+TAB_HEADER,
			bounds.w+TAB_PAD*2, bounds.h-TAB_PAD*2, 0.15f, 0.15f, 0.15f );
		if ( ((Tab*)*i) == activetab )
			Video::DrawRect( xo+x+1,y+TAB_HEADER-1,
				bounds.w+TAB_PAD*2-2, bounds.h-TAB_PAD*2+2, 0.223f, 0.223f, 0.223f );
		SansSerif->Render(xo+x+TAB_PAD,y+TAB_HEADER-TAB_PAD, ((Tab*)*i)->caption.c_str());
		xo += bounds.w+TAB_PAD*2+2;
	}

	// Draw tabs outline
	Video::DrawRect( x, y+TAB_HEADER, w, h-TAB_HEADER, 0.15f, 0.15f, 0.15f );
	Video::DrawRect( x+1, y+TAB_HEADER+1, w-2, h-TAB_HEADER-2, 0.223f, 0.223f, 0.223f );
	
	if (activetab){
		activetab->Draw( GetX() + relx, GetY() + rely );
	}
}

void Tabs::MouseDown( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	Log::Message("Mouse down detected: %d", yr);
	if ( yr < TAB_HEADER ){
		this->TabNext();
	}else{
		Widget *down_on = DetermineMouseFocus( xr, yr );
		if( down_on ){
			down_on->MouseDown( xr,yr );
		}
	}
}


/**\fn Tabs::GetWidth( )
 *  \brief Returns the width of the Tabs collection.
 * \fn Tabs::GetHeight( )
 *  \brief Returns the height of the Tabs collection.
 */
