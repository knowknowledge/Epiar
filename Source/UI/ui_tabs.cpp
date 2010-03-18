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
#define TAB_PAD 8

/**\class Tab
 * \brief A single tab.
 */

/**\brief Constructs a single tab with caption.
 */
Tab::Tab( const string& _caption ):
		caption( string(_caption) ){
	SetX(0);
	SetY(TAB_HEADER);

	this->hscrollbar = NULL;
	this->vscrollbar = NULL;

	Rect bounds = SansSerif->BoundingBox( _caption );
	this->capw = bounds.w;
}

/**\brief Adds children to the Tab object.
 */
bool Tab::AddChild( Widget *widget ){
	// Check to see if widget is past the bounds.
	int hbnd = widget->GetX()+widget->GetWidth();
	int vbnd = widget->GetY()+widget->GetHeight();

	if ( hbnd > this->w ){
		if ( !this->hscrollbar ){
			this->hscrollbar = new Scrollbar( SCROLLBAR_PAD,
				this->h-SCROLLBAR_THICK-SCROLLBAR_PAD,
				this->w-2*SCROLLBAR_PAD, HORIZONTAL,
				this);
			Widget::AddChild( this->hscrollbar );
		}
		this->hscrollbar->maxpos = hbnd;
	}

	if ( vbnd > this->h ){
		if ( !this->vscrollbar ){
			this->vscrollbar = new Scrollbar(
				this->w-SCROLLBAR_THICK-SCROLLBAR_PAD,
				SCROLLBAR_PAD,
				this->h-2*SCROLLBAR_PAD
				-SCROLLBAR_THICK, VERTICAL,
				this);
			Widget::AddChild( this->vscrollbar );
		}
		this->vscrollbar->maxpos = vbnd;
	}

	return Widget::AddChild( widget );
}


/**\brief Draws the Tab contents.
 */
void Tab::Draw( int relx, int rely ){
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// Crop when necessary
	if ( this->hscrollbar || this->vscrollbar )
		Video::SetCropRect(x,y,
				this->w-SCROLLBAR_PAD,
				this->h-SCROLLBAR_PAD);
	
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( ((*i)==this->hscrollbar) ||
				((*i)==this->vscrollbar) ){
			(*i)->Draw( x, y );
			continue;
		}
		int xscroll=0;
		int yscroll=0;
		if ( this->hscrollbar )
			xscroll = hscrollbar->pos;
		if ( this->vscrollbar )
			yscroll = vscrollbar->pos;
		(*i)->Draw( x-xscroll,
				y-yscroll );
	}
	
	if ( this->hscrollbar || this->vscrollbar )
		Video::UnsetCropRect();
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
	Tab* tabwidget = static_cast<Tab*>( widget );

	// Call generic to add the widget
	Widget::AddChild( widget );

	if ( activetab == NULL)
		this->activetab = tabwidget;

	tabwidget->w = this->w;
	tabwidget->h = this->h-TAB_HEADER;

	return true;
}

/**\brief Goes to next Tab in the container.
 */
void Tabs::TabNext( void ){
	list<Widget *>::iterator i;

	for( i = Widget::children.begin(); i != Widget::children.end(); ++i ) {
		if ( static_cast<Tab*>(*i) == activetab ){
			if ( (++i) == Widget::children.end() )
				this->activetab = (Tab*) (Widget::children.front());
			else
				this->activetab = ((Tab*)*i);
			break;
		}
	}
}

/**\brief Tabs drawing function.
 */
void Tabs::Draw( int relx, int rely ){
	// Draw border first
	int x = GetX() + relx;
	int y = GetY() + rely;

	// Draw tabs outline
	Video::DrawRect( x, y+TAB_HEADER, w, h-TAB_HEADER, 0.15f, 0.15f, 0.15f );
	Video::DrawRect( x+1, y+TAB_HEADER+1, w-2, h-TAB_HEADER-2, 0.223f, 0.223f, 0.223f );

	list<Widget *>::iterator i;

	int xo = 0;
	for( i = Widget::children.begin(); i != Widget::children.end(); ++i ) {
		Tab* currtab = static_cast<Tab*>(*i);
		Video::DrawRect( xo+x,y,
			currtab->capw+TAB_PAD*2, TAB_HEADER, 0.15f, 0.15f, 0.15f );
		if ( currtab == activetab )
			Video::DrawRect( xo+x+1,y+1,
				currtab->capw+TAB_PAD*2-2, TAB_HEADER, 0.223f, 0.223f, 0.223f );

		SansSerif->RenderCentered(xo+x+TAB_PAD+currtab->capw/2,
				y+TAB_HEADER/2,
				currtab->caption.c_str());
		xo += currtab->capw+TAB_PAD*2+1;
	}

	if (activetab){
		activetab->Draw( x, y );
	}
}

/**\brief First check if clicked on one of the Tab, if not, pass it on.
 */
void Tabs::MouseLDown( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	if ( yr < TAB_HEADER ){
		activetab = this->CheckTabClicked( xr, yr );
	}else{
		// Pass events onto Tab contents
		this->mouseDownOn = activetab;
		Log::Message("Mouse down on tab %s",activetab->GetName().c_str());
		activetab->MouseLDown( xr,yr );
	}
}

/**\brief Pass on mouse up event to active Tab.
 */
void Tabs::MouseLUp( int x, int y ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	if ( yr < TAB_HEADER ){
		this->UnfocusMouse();
	} else {
		// Pass events onto Tab contents
		activetab->MouseLUp( xr,yr );
	}
}

/**\brief This passes the mouse motion to the active tab.
 */
void Tabs::MouseMotion( int x, int y, int dx, int dy ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	activetab->MouseMotion( xr,yr,dx,dy);
}

/**\brief Checks which Tab was clicked.
 */
Tab* Tabs::CheckTabClicked( int xr, int yr ){
	list<Widget *>::iterator i;
	int xo = 0;
	for( i = Widget::children.begin(); i != Widget::children.end(); ++i ) {
		Tab* currtab = static_cast<Tab*>(*i);
		if ( xr < (currtab->capw+xo+TAB_PAD*2) )
			return currtab;
		xo += currtab->capw+TAB_PAD*2+1;
	}
	// Active Tab didn't change
	return this->activetab;
}


/**\fn Tabs::GetWidth( )
 *  \brief Returns the width of the Tabs collection.
 * \fn Tabs::GetHeight( )
 *  \brief Returns the height of the Tabs collection.
 * \fn Tabs::GetName()
 *  \brief Returns the name of the Tabs collection.
 */
