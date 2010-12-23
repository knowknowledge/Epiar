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
#define TAB_TEXT_ALIGNMENT 2

Color Tab::inactive = GREY;
Color Tab::active = WHITE;

/**\class Tab
 * \brief A single tab.
 */

/**\brief Constructs a single tab with caption.
 */
Tab::Tab( const string& _caption ) {
	inactive = Color( SKIN( "Skin/UI/Tab/Color/Inactive" ) );
	active   = Color( SKIN( "Skin/UI/Tab/Color/Active" ) );

	this->x = x;
	this->y = TAB_HEADER;
	this->h = 0;
	this->w = 0;
	this->name = _caption;

	this->capw = UI::font->TextWidth( _caption );
}

/**\brief Adds children to the Tab object.
 */
Tab *Tab::AddChild( Widget *widget ) {
	assert( widget != NULL );
	Container::AddChild( widget );
	return this;
}

/**\class Tabs
 * \brief A grouping of tabs including a tab selection bar.
 */

/**\brief Constructs a tab collection (no caption), you could add a label though.
 * \param name (not shown) Tabs collection don't really have a caption.
 */
Tabs::Tabs( int x, int y, int _w, int _h, const string& name ):
	activetab( NULL ) {
	this->x = x;
	this->y = y;
	this->w = _w;
	this->h = _h;
	this->name = name;
}

/**\brief Adds a Tab to the Tabs collection.
 */
Tabs *Tabs::AddChild( Widget *widget ){
	assert( widget != NULL );
	if ( widget->GetType() != "Tab" ){
		LogMsg(ERR,"Error attempted to add non-Tab widget to Tab container: %s",
				widget->GetName().c_str());
		return NULL;
	}
	Tab* tabwidget = static_cast<Tab*>( widget );

	// Call generic to add the widget
	Container::AddChild( widget );

	if ( activetab == NULL)
		this->activetab = tabwidget;

	// Adjust Scrollbars to this container
	tabwidget->w = GetW();
	tabwidget->h = GetH()-TAB_HEADER;
	tabwidget->ResetScrollBars();

	return this;
}

/**\brief This just returns the active tab.
 */
Widget *Tabs::DetermineMouseFocus( int relx, int rely ) {
	return this->activetab;
}


/**\brief Goes to next Tab in the container.
 */
void Tabs::TabNext( void ){
	list<Widget *>::iterator i;

	for( i = Container::children.begin(); i != Container::children.end(); ++i ) {
		if ( static_cast<Tab*>(*i) == activetab ){
			if ( (++i) == Container::children.end() )
				this->activetab = (Tab*) (Container::children.front());
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
	Video::DrawRect( x, y+TAB_HEADER, w, h-TAB_HEADER, Tab::inactive );
	Video::DrawRect( x+1, y+TAB_HEADER+1, w-2, h-TAB_HEADER-2, Tab::active );

	list<Widget *>::iterator i;

	int xo = 0;
	for( i = Container::children.begin(); i != Container::children.end(); ++i ) {
		Tab* currtab = static_cast<Tab*>(*i);
		
		Video::DrawRect( xo + x, y, currtab->capw+TAB_PAD*2, TAB_HEADER, Tab::inactive );
		if ( currtab == activetab ) {
			Video::DrawRect( xo + x + 1, y + 1, currtab->capw+TAB_PAD*2-2, TAB_HEADER, Tab::active );
		}

		UI::font->Render(xo + x + TAB_PAD + currtab->capw / 2, y + TAB_HEADER / 2 - TAB_TEXT_ALIGNMENT, currtab->name,Font::CENTER,Font::MIDDLE);

		xo += currtab->capw+TAB_PAD*2+1;
	}

	if (activetab){
		activetab->Draw( x, y );
	}

	Widget::Draw(relx,rely);
}

/**\brief First check if clicked on one of the Tab, if not, pass it on.
 */
bool Tabs::MouseLDown( int x, int y ) {
	// Relative coordinate - to current widget
	int xr = x - GetX();
	int yr = y - GetY();

	if ( yr < TAB_HEADER ){
		activetab = this->CheckTabClicked( xr, yr );
		return true;
	}
	return Container::MouseLDown( x, y );
}

/**\brief Checks which Tab was clicked.
 */
Tab* Tabs::CheckTabClicked( int xr, int yr ){
	list<Widget *>::iterator i;
	int xo = 0;
	for( i = Container::children.begin(); i != Container::children.end(); ++i ) {
		Tab* currtab = static_cast<Tab*>(*i);
		if ( xr < (currtab->capw+xo+TAB_PAD*2) )
			return currtab;
		xo += currtab->capw+TAB_PAD*2+1;
	}
	// Active Tab didn't change
	return this->activetab;
}


/** \fn Tabs::GetType
 *  \brief For Tabs container, it is always "Tabs"
 */
