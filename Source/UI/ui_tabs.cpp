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
	this->x=x;
	this->y=TAB_HEADER;
	this->name=_caption;

	this->hscrollbar = NULL;
	this->vscrollbar = NULL;

	this->h=0;
	this->w=0;

	Rect bounds = SansSerif->BoundingBox( _caption );
	this->capw = static_cast<int>(bounds.w);
}

/**\brief Adds children to the Tab object.
 */
bool Tab::AddChild( Widget *widget ){
	bool success;
	success = Widget::AddChild( widget );
	// Check to see if widget is past the bounds.
	ResetScrollBars();
	return success;
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


/**\brief Move the Scrollbars to the edges.
 */

void Tab::ResetScrollBars(){
	int widget_height,widget_width;
	int max_height,max_width;
	max_height=0;
	max_width=0;

	// It doesn't make sense to add scrollbars for a TAB without a size
	if(this->w==0 || this->h==0 ) return;

	// Find the Max edges
	Widget* widget;
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		widget = *i;
		widget_width = widget->GetX()+widget->GetW();
		widget_height = widget->GetY()+widget->GetH();
		if( widget_height > max_height) max_height=widget_height;
		if( widget_width > max_width) max_width=widget_width;
	}

	// Add a Horizontal ScrollBar if necessary
	if ( max_width > GetW() || this->hscrollbar != NULL ){
		int v_x = SCROLLBAR_PAD;
		int v_y = this->h-SCROLLBAR_THICK-SCROLLBAR_PAD;
		int v_l = this->w-2*SCROLLBAR_PAD;
		// Only add a Scrollbar when it doesn't already exist
		if ( !this->hscrollbar ){
			printf("Adding Horiz ScrollBar to %s: (%d,%d) [%d]\n", GetName().c_str(),v_x,v_y,v_l );
			this->hscrollbar = new Scrollbar(
				v_x, v_y, v_l,
				HORIZONTAL,
				this);
			Widget::AddChild( this->hscrollbar );
		} else {
			printf("Changing Horiz ScrollBar to %s: (%d,%d) [%d]\n", GetName().c_str(),v_x,v_y,v_l );
			this->hscrollbar->SetPosition(v_x,v_y);
			this->hscrollbar->SetSize(v_l);
		}
		this->hscrollbar->maxpos = max_width;
	}

	// Add a Vertical ScrollBar if necessary
	if ( max_height > GetH() || this->vscrollbar != NULL ){
		int v_x = this->w-SCROLLBAR_THICK-SCROLLBAR_PAD;
		int v_y = SCROLLBAR_PAD;
		int v_l = this->h-2*SCROLLBAR_PAD;
		// Only add a Scrollbar when it doesn't already exist
		if ( !this->vscrollbar ){
			printf("Adding Vert ScrollBar to %s: (%d,%d) [%d]\n", GetName().c_str(),v_x,v_y,v_l );
			this->vscrollbar = new Scrollbar(
				v_x, v_y, v_l,
				VERTICAL,
				this);
			Widget::AddChild( this->vscrollbar );
		} else {
			printf("Changing Vert ScrollBar to %s: (%d,%d) [%d]\n", GetName().c_str(),v_x,v_y,v_l );
			this->vscrollbar->SetPosition(v_x,v_y);
			this->vscrollbar->SetSize(v_l);
		}
		this->vscrollbar->maxpos = max_height;
	}
}

/**\brief Constructs a tab collection (no caption), you could add a label though.
 * \param name (not shown) Tabs collection don't really have a caption.
 */
Tabs::Tabs( int x, int y, int _w, int _h, const string& name ):
	activetab( NULL ){
	this->x=x;
	this->y=y;
	this->w=_w;
	this->h=_h;
	this->name=name;
	printf("Creating Tabs at(%d %d) W=%d H=%d\n",x,y,GetW(),GetH());
	
	this->name = name;
}

/**\brief Adds a Tab to the Tabs collection.
 */
bool Tabs::AddChild( Widget *widget ){
	if ( widget->GetType() != "Tab" ){
		Log::Error("Error attempted to add non-Tab widget to Tab container: %s",
				widget->GetName().c_str());
		return false;
	}
	Tab* tabwidget = static_cast<Tab*>( widget );

	// Call generic to add the widget
	Widget::AddChild( widget );

	if ( activetab == NULL)
		this->activetab = tabwidget;

	// Adjust Scrollbars to this container
	tabwidget->w = GetW();
	tabwidget->h = GetH()-TAB_HEADER;
	printf("Attaching TAB to Container H=%d W=%d\n",tabwidget->GetH(),tabwidget->GetW());
	tabwidget->ResetScrollBars();

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
bool Tabs::MouseLDown( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	if ( yr < TAB_HEADER ){
		activetab = this->CheckTabClicked( xr, yr );
	}else{
		// Pass events onto Tab contents
		activetab->MouseLDown( xr,yr );
	}
	return true;
}

/**\brief Pass on mouse up event to active Tab.
 */
bool Tabs::MouseLUp( int x, int y ){
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	if ( yr > TAB_HEADER ){
		// Pass events onto Tab contents
		activetab->MouseLUp( xr,yr );
	}
	return true;
}

/**\brief This passes the mouse motion to the active tab.
 */
bool Tabs::MouseMotion( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - GetX();
	int yr = yi - GetY();

	activetab->MouseMotion( xr,yr );
	return true;
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
