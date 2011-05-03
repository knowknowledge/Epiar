/**\file			ui_container.cpp
 * \author			Maoserr
 * \date			Created: Saturday, March 27, 2010
 * \date			Modified: Saturday, March 27, 2010
 * \brief			Container object can contain other widgets.
 */

#include "includes.h"
#include "UI/ui.h"
#include "Utilities/xml.h"
#include "UI/ui_container.h"

/** \addtogroup UI
 * @{
 */

/**\class Container
 * \brief Container is a container class for other widgets.
 */

/**\brief Constructor, initializes default values.*/
Container::Container( string _name, bool _mouseHandled ):
	mouseHandled( _mouseHandled ), keyboardFocus( NULL ), mouseHover( NULL ),
	lmouseDown( NULL ), mmouseDown( NULL ), rmouseDown( NULL ),
	vscrollbar( NULL ),
	formbutton( NULL )
{
	name = _name;
}

/**\brief Destructor, destroys all children.
 * \todo Implement a Container::Hide routine that doesn't destroy children.
 * \bug This will cause a segfault on statically allocated widget children
 */
Container::~Container( void ) {
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}

	children.clear();

	keyboardFocus = NULL;
	mouseHover = NULL;
	lmouseDown = NULL;
	mmouseDown = NULL;
	rmouseDown = NULL;
	
	vscrollbar = NULL;
	formbutton = NULL;
}

/**\brief Adds a child to the current container.
 */
Container *Container::AddChild( Widget *widget ) {
	assert( widget != NULL );
	if( widget != NULL ) {
		if( widget->parent ) {
			assert( widget->parent != this );
			((Container*)(widget->parent))->Detach( widget );
		}
		children.push_back( widget );
		widget->parent = this;
		LogMsg(INFO, "Adding %s %s %p to %s", widget->GetType().c_str(), widget->GetName().c_str(), widget, GetName().c_str() );
		// Check to see if widget is past the bounds.
		ResetScrollBars();
	}
	return this;
}

/**\brief Deletes a child from the current container.
 * \details This performs a semi-breadth-first search to find the specified widget.
 * \warn The widget needs to be a correctly allocated.
 * \param[in] A pointer to a valid widget.
 * \returns true if the child was correctly found.
 */
bool Container::DelChild( Widget *widget ){
	bool not_scrollbar;
	list<Widget *>::iterator i;

	not_scrollbar = ( widget->GetMask() != WIDGET_SCROLLBAR );

	// Scan all of the children
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ) {
			// Ticket #96: The below delete had caused memory corruption on MSVC Compilations.
			// The source of this bug has been fixed.
			delete (*i);
			i = children.erase( i );
			ResetInput();

			// Don't reset the Scrollbars when it is a scrollbar being deleted
			// This will cause a stack overflow.
			if( not_scrollbar ) {
				ResetScrollBars();
			}

			return true;
		}
	}

	// Scan all of the children's children
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->GetMask() & WIDGET_CONTAINER ) {
			if( ((Container*)(*i))->DelChild( widget ) ) {
				return true;
			}
		}
	}

	return false;
}

/**\brief Detach a child without deleting it.
 */
bool Container::Detach( Widget *widget ) {
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ) {
			i = children.erase( i );
			return true;
		}
	}
	return false;
}

/**\brief Empties all children.
 */
void Container::Empty( void ){
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		delete (*i);
	}
	children.clear();

	ResetInput();
}

/**\brief Reset focus and events.
 *
 */
void Container::ResetInput( void ){
	list<Widget *>::iterator i;

	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->GetMask() & WIDGET_CONTAINER ) {
			((Container*)(*i))->ResetInput();
		}
	}

	// Forget about these:
	this->keyboardFocus = NULL;
	this->mouseHover = NULL;
	this->lmouseDown = NULL;
	this->mmouseDown = NULL;
	this->rmouseDown = NULL;

	// Don't forget about these:
	//this->vscrollbar = NULL;
	//this->formbutton = NULL;
}

/**\brief Checks to see if point is inside a child
 *
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::DetermineMouseFocus( int relx, int rely ) {
	list<Widget *>::reverse_iterator i;

	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if ( ( (*i)->Contains(relx, rely) && ((*i)->GetType() == "Scrollbar") ) // Tabs
		    || (*i)->Contains(relx, rely + yoffset) ) // Non-Tabs
		{
			return (*i);
		}
	}
	return( NULL );
}

/**\brief Check if a Widget exists in this Container
 *
 * \warn Do not run any methods on the possible Widget.  It may not exist.
 *       Even calling possible->GetMask() will Segfault.
 */
bool Container::IsAttached( Widget* possible ) {
	list<Widget *>::iterator i;
	
	// Check the direct children.
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == possible ) {
			LogMsg(INFO, "Found %s %s (%p) in %s", possible->GetType().c_str(), possible->GetName().c_str(), possible, GetName().c_str() );
			return true;
		}
	}
	
	// Check the grandchildren
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->GetMask() & WIDGET_CONTAINER ) {
			if( ((Container*)(*i))->IsAttached( possible ) ) {
				return true;
			}
		}
	}

	// Not found
	return false;
}

/**\brief Search this Container for a Widget
 *
 * \details
 *
 * The Container Search is used for traversing the Widget tree
 * starting at this Container.  The query is a list of Widget
 * descriptions surrounded by slashes.  Each widget description is a
 * collection of tokens that will narrow down which specific widget is
 * being referred to.
 *
 * The form of the Query:
 *  - The query always starts and ends with a slash.
 *  - Between slashes is a widget descriptor.
 *  - Each internal slash tells the search to step down to the described child.
 *  - The widget descriptor is a combinations of one or more widget characteristics.
 *
 * The Tokens:
 *  - TYPE : A Type Name restricts this search to this specific Type.
 *  - [N] : A number inside square brackets designates that this search must be
 *        (N-1)th match for this particular search. Indexes start at zero.
 *  - "NAME" or 'NAME' : This will find a specifically named Widget.  Either
 *        kind of Quote can be used.
 *  - (X,Y) :  This will find the Widget at the relative coordinates (X,Y).
 *  - / : The Slash is used as a boundary between Widget queries.
 *
 * Examples Search Queries:
 *  - /2/ This will find the 3rd child of this Container.
 *  - /Tab/ This will find the first Tab in this Container.
 *  - /"Foobar"/ This will find the first Widget named Foobar.
 *  - /(50,50)/ This will find the first Widget at (50,50).
 *  - /Frame[2]/ This will find the 3rd Frame of this Container.
 *  - /Window[2]/Checkbox/ This will find the first Checkbox in the 3rd
 *                         Window of this Container.
 *
 * \warn Repeating the same same Token type within the same Widget descriptor
 *       will overwrite the previous token.  For example, /Button[0]Textbox/
 *       will find the first Textbox not the first Button.
 * \warn The name cannot contain any of the special-character tokens, or else it will not
 *       be properly captured.
 *
 * \todo The query validation needs to be improved.  /(Foobar,4)/ will attempt
 *       to convert the string "Foobar" to a string.
 *
 * \param[in] full_query A specially formatted string
 * \returns A pointer to the first matching Widget or NULL.
 */
Widget *Container::Search( string full_query ) {
	int section = 0;
	char token;
	string subquery;
	string tokens = "/[]\"'(,)";
	vector<string> tokenized;
	vector<string>::iterator iter;
	list<Widget *>::iterator i;

	Container *current = this;

	// Temporary query values
	typedef struct {
		union {
			int flags;
			struct {
				int FOUND_COORD :1;
				int FOUND_TYPE  :1;
				int FOUND_NAME  :1;
				int FOUND_INDEX :1;
			};
		};
		int x,y;
		string type;
		string name;
		int index;
	} Query;
	Query query = {{0},0,0,"","",0};

	#define ABSORB() do{\
		++iter;\
		if( iter == tokenized.end() ) {\
			LogMsg(ERR, "Malformed Query %s Unexpected End", full_query.c_str());\
			return NULL;\
		}\
	}while(0)

	#define ABSORB_STR(X) do{\
		if( *(iter) != X ) {\
			LogMsg(ERR, "Malformed Query %s Expected \"" X "\"", full_query.c_str());\
			return NULL;\
		}\
		ABSORB();\
	}while(0)

	// Tokenize the String
	tokenized = TokenizedString( full_query, tokens );

	// Check the Start of the Query
	iter = tokenized.begin();
	if( *iter == "" ) ABSORB_STR( "" );
	if( *iter == "/" ) {
		ABSORB_STR( "/" );
	} else {
		LogMsg(WARN, "Query '%s' did not begin with a '/'", full_query.c_str() );
	}
	
	// Search all the tokens
	//LogMsg(INFO, "QUERY: '%s'", full_query.c_str() );
	for(; iter != tokenized.end(); ++iter ) {
		subquery = (*iter);
		// LogMsg(INFO, "token: '%s'", subquery.c_str() );
		if( subquery == "" ) { continue; }

        // The tokens are always going to be single characters
        assert( subquery.size() >= 1 );
		token = subquery[0];

		// If we're checking a Token, we need to be in a Container
		if( !( (current->GetMask()) & WIDGET_CONTAINER ) ) {
			LogMsg(INFO, "The query '%s' reached a non-container Widget and aborted at section %d.", full_query.c_str(), section );
			return NULL;
		}

		// If this is not a token then it is Widget Type
		if( subquery.find_first_of(tokens) != string::npos) {
			if( subquery.size() != 1 ) {
				LogMsg(ERR, "Malformed Query %s Multi-character token", full_query.c_str());
				return NULL;
			}

			switch( token ) {
				// Boundary: Search the Children
				case '/':
				{
					bool found = false;
					int ind = 0;
					if( 0 == (current->GetMask() & WIDGET_CONTAINER) ) {
						LogMsg(ERR, "Malformed Query %s '/' token on non-container 0x%08X", full_query.c_str());
						return NULL;
					}
					for( i = current->children.begin(); i != current->children.end(); ++i ) {
						// LogMsg(DEBUG1, "Checking %s %s (%d,%d) 0x%08X\n", (*i)->GetName().c_str(), (*i)->GetType().c_str(), (*i)->GetX(), (*i)->GetY(), (*i)->GetMask() );
						if( query.FOUND_NAME && (query.name != (*i)->GetName()) ) {
							continue;
						}
						if( query.FOUND_TYPE && (query.type != (*i)->GetType()) ) {
							continue;
						}
						if( query.FOUND_COORD && ((*i)->Contains(query.x, query.y) == false) ) {
							continue;
						}
						if( query.FOUND_INDEX && (query.index != ind) ) {
							ind++;
							continue;
						}
						// Found a match!
						found = true;
						current = (Container*)(*i);
						// Forget about the old query
						query.flags = 0;
						break;
					}
					
					if( found == false ) {
						LogMsg(INFO, "The query '%s' failed to find a widget at section %d", full_query.c_str(), section );
						return NULL;
					}

					++section;
					break;
				}

				// Bracketed number: Container Index
				case '[':
				{
					query.FOUND_INDEX = 1;
					ABSORB_STR("[");
					// TODO Check that this is a number
					query.index = convertTo<int>( *(iter) );
					ABSORB();
					ABSORB_STR("]");
					break;
				}

				// Quoted String: Widget Name
				case '\'':
				{
					query.FOUND_NAME = 1;
					ABSORB_STR("\'");
					query.name = *(iter);
					ABSORB();
					ABSORB_STR("\'");
					break;
				}

				case '"':
				{
					query.FOUND_NAME = 1;
					ABSORB_STR("\"");
					query.name = *(iter);
					ABSORB();
					ABSORB_STR("\"");
					break;
				}

				// Paren Tuple: Widget's Relative coordinate
				case '(':
				{
					query.FOUND_COORD = 1;
					ABSORB_STR("(");
					// TODO Check that this is a number
					query.x = convertTo<int>( *(iter) );
					ABSORB();
					ABSORB_STR(",");
					// TODO Check that this is a number
					query.y = convertTo<int>( *(iter) );
					ABSORB();
					ABSORB_STR(")");
					break;
				}

				default:
					LogMsg(ERR, "Unexpected token '%c' in query '%s'", token, full_query.c_str() );
					return NULL;
			}
		}

		// Plain String: Widget Type
		else if( subquery.size() > 0 ) {
			query.FOUND_TYPE = 1;
			query.type = subquery;
		}

		// Empty String, skip it.
		else { }
	}

	if( query.flags != 0 ) {
		LogMsg(WARN, "Query '%s' did not end with a '/'", full_query.c_str() );
	}

	#undef ABSORB
	#undef ABSORB_STR

	//LogMsg(DEBUG1, "Found %s %s (%d,%d) 0x%08X\n", (*i)->GetName().c_str(), (*i)->GetType().c_str(), (*i)->GetX(), (*i)->GetY(), (*i)->GetMask() );
	return (Widget*)current;
}

/**\brief Search for a child named
 *
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildNamed( string _name, int mask ) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( ( (*i)->GetName() == _name ) && ( (*i)->GetMask() & mask ) ) {
			return (*i);
		}
	}
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] pos position of the child
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildFromTop( int pos, int mask) {
	int p = 0;
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i)->GetMask() & mask ) {
			if( pos == p ) {
				return (*i);
			}
			++p;
		}
	}
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] pos position of the child
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::ChildFromBottom( int pos, int mask) {
	int p = 0;
	list<Widget *>::iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i)->GetMask() & mask ) {
			if( pos == p ) {
				return (*i);
			}
			++p;
		}
	}
	return( NULL );
}
/**\brief Search for a child at a specific position
 *
 * pos == 0 is at the top.
 *
 * \param[in] widget don't start checking until this widget is found.
 * \param[in] mask Only consider Widgets of this type.
 * \note This checks the children in the order that they are drawn so that Children 'on top' are searched last.
 */
Widget *Container::NextChild( Widget* widget, int mask) {
	list<Widget *>::iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.begin(); i != children.end(); ++i ) {
		if( (*i) == widget ) {
			// Found the previous widget, now keep going
			++i;
			for(; i != children.end(); ++i ) {
				if( (*i)->GetMask() & mask ) {
					return (*i);
				}
			}
			// Never found another Widget that matched the mask
			return( NULL );
		}
	}
	// Never found the previous Widget
	return( NULL );
}

/**\brief Search for a child at a specific position
 *
 * \param[in] widget don't start checking until this widget is found.
 * \param[in] mask Only consider Widgets of this type.
 * \note This checks the children in the opposite order that they are drawn so that Children 'on top' get focus first.
 */
Widget *Container::PrevChild( Widget* widget, int mask) {
	list<Widget *>::reverse_iterator i;

	// Check children from top (last drawn) to bottom (first drawn).
	for( i = children.rbegin(); i != children.rend(); ++i ) {
		if( (*i) == widget ) {
			// Found the previous widget, now keep going
			++i;
			for(; i != children.rend(); ++i ) {
				if( (*i)->GetMask() & mask ) {
					return (*i);
				}
			}
			// Never found another Widget that matched the mask
			return( NULL );
		}
	}
	// Never found the previous Widget
	return( NULL );
}

/**\brief Draws this widget and all children widgets.
 */
void Container::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;

	// Crop to prevent child widgets from spilling
	Video::SetCropRect(x, y + 2, this->w, this->h);
	
	// Draw any children
	list<Widget *>::iterator i;
	
	for( i = children.begin(); i != children.end(); ++i ) {
		// Skip scrollbars
		if ( (*i) == this->vscrollbar ){
			(*i)->Draw( x, y );
			continue;
		}

		int yscroll = 0;
		if ( this->vscrollbar )
			yscroll = vscrollbar->GetPos();

		(*i)->Draw( x, y - yscroll );
	}
	
	Video::UnsetCropRect();
	
	Widget::Draw(relx, rely);
}

/**\brief Mouse is currently moving over the widget, without button down.
 */
bool Container::MouseMotion( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseMotion(xi,yi);

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if ( this->lmouseDown ){
		// Mouse button is held down, send drag event
		this->lmouseDown->MouseDrag(xr,yr);
	}

	if( !event_on ){
		// Not on a widget
		if( this->mouseHover ){
			// We were on a widget, send leave event
			this->mouseHover->MouseLeave();
			this->mouseHover=NULL;
		}
		return this->mouseHandled;
	}
	if( !this->mouseHover ){
		// We're on a widget, but nothing was hovered on before
		// send enter event only
		event_on->MouseEnter( xr,yr + yoffset );
		this->mouseHover=event_on;
		return true;
	}
	if( this->mouseHover != event_on ){
		// We're on a widget, and leaving another widget
		// send both enter and leave event
		this->mouseHover->MouseLeave();
		event_on->MouseEnter( xr,yr + yoffset );
		this->mouseHover=event_on;
	}

	event_on->MouseMotion( xr, yr + yoffset );

	return true;
}

/**\brief Generic mouse up function.
 */
bool Container::MouseLUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseLUp(xi,yi);

	Widget *event_on = DetermineMouseFocus( xr, yr );

	if( this->lmouseDown ){
		if (this->lmouseDown == event_on) {
			// Mouse up is on the same widget as the mouse down, send up event
			event_on->MouseLUp( xr,yr + yoffset );
			this->lmouseDown = NULL;
			return true;
		}else{
			// Mouse up is on a different widget, send release event to old
			this->lmouseDown->MouseLRelease();
			this->lmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Left up detect in %s.",this->name.c_str());

	return this->mouseHandled;
}

/**\brief Generic mouse down function.
 */
bool Container::MouseLDown( int xi, int yi ) {
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseLDown( xi, yi );

	// update drag coordinates in case this is draggable
	dragX = xr;
	dragY = yr;

	Widget *event_on = DetermineMouseFocus( xr, yr );

	// If Nothing was clicked on
	if( !event_on ){
		//LogMsg(INFO,"Mouse Left down detect in %s.",this->name.c_str());
		if( this->keyboardFocus )
			this->keyboardFocus->KeyboardLeave();
		this->keyboardFocus = NULL;
		return this->mouseHandled;
	}

	// We clicked on a widget
	
	if( event_on == vscrollbar ) {
		event_on->MouseLDown( xr, yr );
	} else {
		event_on->MouseLDown( xr, yr + yoffset );
	}

	this->lmouseDown = event_on;
	if( !this->keyboardFocus )
		// No widget had keyboard focus before
		event_on->KeyboardEnter();
	else if( this->keyboardFocus != event_on ){
		// keyboard focus changed
		this->keyboardFocus->KeyboardLeave();
		event_on->KeyboardEnter();
	}
	this->keyboardFocus = event_on;

	return true;
}

/**\brief Generic mouse release function.
 * \details Unlike the MouseLUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseLRelease( void ){
	Widget::MouseLRelease();

	// Pass event onto children if needed
	if( this->lmouseDown )
		return this->lmouseDown->MouseLRelease();
	//LogMsg(INFO,"Left Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse up function.
 */
bool Container::MouseMUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseMUp( xi, yi );

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->mmouseDown ){
		if ( this->mmouseDown == event_on ){
			// Mouse up is on the same widget as mouse down, send event
			this->mmouseDown = NULL;
			return event_on->MouseMUp( xr, yr + yoffset );
		}else{
			// Mouse up is on a different widget, send release event to old
			this->mmouseDown->MouseMRelease( );
			this->mmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Middle up detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse down function.
 */
bool Container::MouseMDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseMDown( xi, yi );

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		this->mmouseDown=event_on;
		return event_on->MouseMDown( xr, yr + yoffset );
	}
	//LogMsg(INFO,"Mouse Middle down detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic middle mouse release function.
 * \details Unlike the MouseMUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseMRelease( void ){
	Widget::MouseMRelease();

	// Pass event onto children if needed
	if( this->mmouseDown )
		return this->mmouseDown->MouseMRelease();
	//LogMsg(INFO,"Middle Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse up function.
 */
bool Container::MouseRUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseRUp( xi, yi);

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( this->rmouseDown ){
		if ( this->rmouseDown == event_on ){
			// Mouse up is on the same widget as mouse down, send event
			this->rmouseDown = NULL;
			return event_on->MouseRUp( xr, yr + yoffset);
		}else{
			// Mouse up is on a different widget, send release event to old
			this->rmouseDown->MouseRRelease();
			this->rmouseDown = NULL;
		}
	}
	//LogMsg(INFO,"Mouse Right up detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse down function.
 */
bool Container::MouseRDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseRDown( xi, yi );

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on ){
		this->rmouseDown=event_on;
		return event_on->MouseRDown( xr, yr + yoffset );
	}
	//LogMsg(INFO,"Mouse Right down detect in %s.",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic right mouse release function.
 * \details Unlike the MouseRUp function, this is called when the user releases
 * the mouse on a different widget.
 */
bool Container::MouseRRelease( void ){
	Widget::MouseRRelease();

	// Pass event onto children if needed
	if( this->rmouseDown )
		return this->rmouseDown->MouseRRelease();
	//LogMsg(INFO,"Right Mouse released in %s",this->name.c_str());
	return this->mouseHandled;
}

/**\brief Generic mouse wheel up function.
 */
bool Container::MouseWUp( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseWUp( xi, yi );

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on && event_on->MouseWUp( xr,yr + yoffset ) ) {
		return true;
	}
	if( vscrollbar ) {
		vscrollbar->ScrollUp();
		return true;
	}
	//LogMsg(INFO,"Mouse Wheel up detect in %s.",this->name.c_str());
	return false;
}

/**\brief Generic mouse wheel down function.
 */
bool Container::MouseWDown( int xi, int yi ){
	// Relative coordinate - to current widget
	int xr = xi - this->x;
	int yr = yi - this->y;
	int yoffset = this->vscrollbar ? this->vscrollbar->GetPos() : 0;

	Widget::MouseWDown( xi, yi );

	Widget *event_on = DetermineMouseFocus( xr, yr );
	if( event_on && event_on->MouseWDown( xr,yr + yoffset ) ) {
		return true;
	}
	if( vscrollbar ) {
		vscrollbar->ScrollDown();
		return true;
	}
	//LogMsg(INFO,"Mouse Wheel down detect in %s.",this->name.c_str());
	return false;
}

/**\brief Generic keyboard focus function.
 */
bool Container::KeyboardEnter( void ){
	Widget::KeyboardEnter();
	this->keyactivated=true;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardEnter();
	//LogMsg(INFO,"Keyboard enter detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard unfocus function.
 */
bool Container::KeyboardLeave( void ){
	Widget::KeyboardLeave();
	this->keyactivated=false;
	if( this->keyboardFocus )
		return this->keyboardFocus->KeyboardLeave();
	//LogMsg(INFO,"Keyboard leave detect in %s.",this->name.c_str());
	return true;
}

/**\brief Generic keyboard key press function.
 */
bool Container::KeyPress( SDLKey key ) {
	Widget *next;
	if( keyboardFocus ) {
		
		// If this key is a TAB and the keyboard is currently focused on a Textbox,
		// then move to the next textbox
		if( (key == SDLK_TAB) && ( keyboardFocus->GetMask() & (WIDGET_TEXTBOX) ) ) {
			
			next = NextChild( keyboardFocus, WIDGET_TEXTBOX );
			if( next == NULL ) {
				// Wrap around to the top
				next = ChildFromBottom( 0, WIDGET_TEXTBOX );
			}
			if( next ) {
				keyboardFocus->KeyboardLeave();
				keyboardFocus = next;
				keyboardFocus->KeyboardEnter();
			}
			return true;
		}

		// Otherwise, pass the key Press normally
		if( keyboardFocus->KeyPress( key ) ) {
			return true;
		}
	}

	// If the key is an ENTER then activate the form button when it has been assigned
	if( (key == '\n') && (formbutton != NULL) ) {
		formbutton->Activate(Widget::Action_MouseLUp);
	}

	//LogMsg(INFO,"Key press detect in %s.",this->name.c_str());
	return false;
}

/**\brief Move the Scrollbars to the edges.
 * \details This will always delete the current Scrollbars even if the size of
 * the container hasn't fundamentally changed.  This is so that the Scrollbars
 * are always the last Widgets in the Container.
 */

void Container::ResetScrollBars() {
	bool has_vscrollbar;
	int widget_height, widget_width;
	int max_height, max_width;
	max_height = 0;
	max_width = 0;

	// It doesn't make sense to add scrollbars for a Container without a size
	if(this->w == 0 || this->h == 0 ) return;

	// Remove the Current Scrollbar
	has_vscrollbar = (vscrollbar != NULL);
	if( this->vscrollbar != NULL ) {
		Container::DelChild( this->vscrollbar );
		this->vscrollbar = NULL;
	}

	// Find the Max edges
	Widget* widget;
	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		widget = *i;
		widget_width  = widget->GetX() + widget->GetW();
		widget_height = widget->GetY() + widget->GetH();
		if( widget_height > max_height) max_height = widget_height;
		if( widget_width > max_width) max_width = widget_width;
	}

	// Add a Vertical ScrollBar if necessary
	if ( max_height > GetH() ){
		int v_x = this->w;
		int v_y = 0;
		int v_l = this->h;

		// Don't Log extra messages when the ScrollBar is simply being replaced.
		if( !has_vscrollbar ) {
			LogMsg(INFO, "Adding Vert ScrollBar to %s: (%d,%d) [%d]", GetName().c_str(),v_x,v_y,v_l );
		}

		this->vscrollbar = new Scrollbar(v_x, v_y, v_l, max_height);

		children.push_back( this->vscrollbar );
	} else if ( has_vscrollbar ) {
		LogMsg(INFO, "Removing Vert ScrollBar to %s", GetName().c_str() );
	}
}

/**\brief Set the button to activate when the user hits ENTER in thie Container
 */
Container* Container::SetFormButton( Button* button ) {
	if( button == NULL ) {
		LogMsg(INFO, "Clearing the Form Button for %s %s", GetType().c_str(), GetName().c_str() );
		formbutton = NULL;
	} else if( ! IsAttached( button ) ) {
		LogMsg(INFO, "Cannot set %s to the Form Button for %s %s because it is not a childe of the Container.", button->GetName().c_str(), GetType().c_str(), GetName().c_str() );
	} else {
		LogMsg(INFO, "Setting %s to the Form Button for %s %s", button->GetName().c_str(), GetType().c_str(), GetName().c_str() );
		assert( button->GetMask() & WIDGET_BUTTON );
		formbutton = button;
	}
	ResetInput();
	return this;
}

/**\brief Generate an XML Node of this Container and it's children
 */
xmlNodePtr Container::ToNode() {
	xmlNodePtr thisNode;
	char buff[256];

	thisNode = xmlNewNode(NULL, BAD_CAST this->GetType().c_str() );
	xmlSetProp( thisNode, BAD_CAST "name", BAD_CAST this->GetName().c_str() );

	snprintf(buff, sizeof(buff), "%d", this->GetX() );
	xmlSetProp( thisNode, BAD_CAST "x", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetY() );
	xmlSetProp( thisNode, BAD_CAST "y", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetW() );
	xmlSetProp( thisNode, BAD_CAST "w", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetH() );
	xmlSetProp( thisNode, BAD_CAST "h", BAD_CAST buff );

	list<Widget *>::iterator i;
	for( i = children.begin(); i != children.end(); ++i ) {
		xmlAddChild(thisNode, (*i)->ToNode()  );
	}
	return thisNode;

}

/** @} */
