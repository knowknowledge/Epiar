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

/** \addtogroup UI
 * @{
 */

#define TAB_HEADER 20

/**\class Tab
 * \brief A single tab.
 */

/**\brief Constructs a single tab with caption.
 */
Tab::Tab( const string& _caption ) {
	this->x = x;
	this->y = TAB_HEADER;
	this->h = 0;
	this->w = 0;
	this->name = _caption;

	active_left = Image::Get("Resources/Skin/active-tab-left.png");
	active_middle = Image::Get("Resources/Skin/active-tab-middle.png");
	active_right = Image::Get("Resources/Skin/active-tab-right.png");

	inactive_left = Image::Get("Resources/Skin/inactive-tab-left.png");
	inactive_middle = Image::Get("Resources/Skin/inactive-tab-middle.png");
	inactive_right = Image::Get("Resources/Skin/inactive-tab-right.png");

	this->capw = UI::font->TextWidth( _caption );
}

Tab::~Tab() {
	active_left = NULL;
	active_middle = NULL;
	active_right = NULL;

	inactive_left = NULL;
	inactive_middle = NULL;
	inactive_right = NULL;
}

/**\brief Adds children to the Tab object.
 */
Tab *Tab::AddChild( Widget *widget ) {
	assert( widget != NULL );
	Container::AddChild( widget );
	return this;
}

void Tab::DrawHandle( int realx, int realy, bool active ) {
	Image* left = active ? active_left : inactive_left;
	Image* middle = active ? active_middle : inactive_middle;
	Image* right = active ? active_right : inactive_right;
	
	left->Draw( realx, realy );
	middle->DrawTiled( realx + left->GetWidth(), realy, capw, middle->GetHeight() );
	right->Draw( realx + left->GetWidth() + capw , realy );

	UI::font->Render( realx + left->GetWidth(), realy, GetName() );
}

int Tab::GetHandleWidth( bool active ) {
	if( active ) {
		return active_left->GetWidth() + capw + active_right->GetWidth();
	} else {
		return inactive_left->GetWidth() + capw + inactive_right->GetWidth();
	}
}

/**\class Tabs
 * \brief A grouping of tabs including a tab selection bar.
 */

/**\brief Constructs a tab collection (no caption), you could add a label though.
 * \param name (not shown) Tabs collection don't really have a caption.
 */
Tabs::Tabs( int x, int y, int _w, int _h, const string& name ):
	activetab( NULL ) {
	background = Color( SKIN( "Skin/UI/Tab/Color/Background" ) );
	edge = Color( SKIN( "Skin/UI/Tab/Color/Edge" ) );

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
	tabwidget->SetW( GetW() );
	tabwidget->SetH( GetH() - TAB_HEADER );
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

	// Draw the Background and Edge
	Video::DrawRect( x, y+TAB_HEADER, w, h-TAB_HEADER, background );
	Video::DrawBox( x, y+TAB_HEADER, w, h-TAB_HEADER, edge );

	int xo = x;
	list<Widget *>::iterator i;
	for( i = Container::children.begin(); i != Container::children.end(); ++i ) {
		Tab* currtab = static_cast<Tab*>(*i);

		currtab->DrawHandle( xo, y, ( currtab == activetab ) );

		// For the Active tab, draw a background colored line to make this tab
		// look like it is attached to the currently visible Container.
		if( currtab == activetab ) {
			Video::DrawLine( xo, y + TAB_HEADER, xo + currtab->GetHandleWidth(true), y + TAB_HEADER, background );
		}

		xo += currtab->GetHandleWidth( currtab == activetab );
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
		if ( xr < xo + currtab->GetHandleWidth(currtab == activetab) )
			return currtab;
		xo += currtab->GetHandleWidth(currtab == activetab);
	}
	// Active Tab didn't change
	return this->activetab;
}


/** \fn Tabs::GetType
 *  \brief For Tabs container, it is always "Tabs"
 */

/** @} */
