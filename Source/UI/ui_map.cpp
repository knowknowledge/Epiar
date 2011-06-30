/**\file			ui_map.cpp
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Saturday, May 28, 2011
 * \brief			Map Widget
 * \details
 */

#include "includes.h"
#include "UI/ui_map.h"
#include "Sprites/planets.h"
#include "Sprites/gate.h"
#include "Sprites/effects.h"
#include "Utilities/timer.h"

/** \addtogroup UI
 * @{
 */

/**\class Map
 * \brief Widget for displaying Sprites
 *
 */


#define MAP_ZOOM_RATIO 1.1f ///< The rate at which the Map Zooms in and out.

Font *Map::MapFont = NULL;

/** \brief Map Constructor
 *
 */
Map::Map( int x, int y, int w, int h, Coordinate center, SpriteManager* sprites )
{
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->center = center;
	this->sprites = sprites;

	spriteTypes = ( DRAW_ORDER_PLAYER   |
	                DRAW_ORDER_PLANET   |
	                DRAW_ORDER_GATE_TOP );

	// Show sprites only if this option is set.
	if( OPTION(int,"options/development/ships-worldmap") ) {
		spriteTypes |= DRAW_ORDER_SHIP;
	}

	alpha = 1;

	float size = (h<w) ? h : w; // Max of Height and Width

	// Initially strech the Map so that it covers all QuadTrees
	float north,south,east,west,edge;
	sprites->GetBoundaries(&north,&south,&east,&west);
	// edge is the maximum distance from zero of north,south,east, and west
	edge = (north>-south)?north:-south;
	edge = (edge>-west)?edge:-west;
	edge = (edge>east)?edge:east;
	scale = (size) / ( 2*(edge+QUADRANTSIZE) );

	if( MapFont == NULL )
	{
		MapFont = new Font( SKIN("Skin/HUD/Map/Font") );
		MapFont->SetColor( Color( SKIN("Skin/HUD/Map/Color") ) );
		MapFont->SetSize( convertTo<int>( SKIN("Skin/HUD/Map/Size") ) );
	}
}

/** \brief Map Destructor
 *
 */
Map::~Map()
{
	sprites = NULL;
}

/** \brief Draw Map
 *
 */
void Map::Draw( int relx, int rely )
{
	list<Sprite*> *spriteList;
	list<Sprite*>::iterator iter;

	// These variables are used for almost every sprite symbol
	Coordinate pos, pos2;
	Color col, field, gatePath;

	// Configurable Settings
	gatePath = Color( SKIN("Skin/HUD/Map/GatePath") );

	// The Backdrop
	Video::DrawRect( relx + GetX(), rely + GetY(), w, h, BLACK, alpha);

	Video::SetCropRect( relx + GetX(), rely + GetY(), w, h );

	// TODO: Quadrant lines should be be drawn correctly.

	// The Quadrant Lines
	Coordinate i, top, bottom;
	bottom = ClickToWorld( Coordinate(GetX(),GetY()) );
	bottom = Coordinate( TO_INT(bottom.GetX() / (2*QUADRANTSIZE)),
	                     TO_INT(bottom.GetY() / (2*QUADRANTSIZE)) );
	bottom *= 2*QUADRANTSIZE;
	bottom -= Coordinate(QUADRANTSIZE, QUADRANTSIZE);

	top = ClickToWorld( Coordinate(GetX()+w, GetY()+h) );
	top = Coordinate( TO_INT(top.GetX() / (2*QUADRANTSIZE)),
	                  TO_INT(top.GetY() / (2*QUADRANTSIZE)) );
	top *= 2*QUADRANTSIZE;
	top += Coordinate(QUADRANTSIZE, QUADRANTSIZE);

	for( i = bottom;
	     i.GetX() <= top.GetX() ||
	     i.GetY() <= top.GetY() ;
	     i += Coordinate(2*QUADRANTSIZE, 2*QUADRANTSIZE) )
	{
		Coordinate point = WorldToScreen( i );
		Video::DrawLine( relx + GetX()     , point.GetY(),
						 relx + GetX() + w , point.GetY(), DARKGREY , alpha );
		Video::DrawLine( point.GetX(), rely + GetY()     , 
						 point.GetX(), rely + GetY() + h , DARKGREY , alpha );
	}

	// Draw the Sprites
	spriteList = sprites->GetSprites( spriteTypes );
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		col = (*iter)->GetRadarColor();
		pos = WorldToScreen( (*iter)->GetWorldPosition() );

		switch( (*iter)->GetDrawOrder() ) {
			case DRAW_ORDER_PLAYER:
				Video::DrawFilledCircle( pos, (Timer::GetRealTicks()%5000)/200 + 3, col, alpha - (Timer::GetRealTicks()%5000)/5000.0f );
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;

			case DRAW_ORDER_SHIP:
			case DRAW_ORDER_PROJECTILE:
			case DRAW_ORDER_EFFECT:
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;

			case DRAW_ORDER_PLANET:
				field = ((Planet*)(*iter))->GetAlliance()->GetColor();
				Video::DrawFilledCircle( pos, ((Planet*)(*iter))->GetInfluence()*scale, field, alpha*.5f );
				Video::DrawCircle( pos, 3, 1, col, alpha );
				break;

			case DRAW_ORDER_GATE_TOP:
				Video::DrawCircle( pos, 3, 1, col, alpha );
				if( ((Gate*)(*iter))->GetExit() != NULL ) {
					pos2 = WorldToScreen( ((Gate*)(*iter))->GetExit()->GetWorldPosition() );
					Video::DrawLine( pos, pos2, gatePath, alpha*.5f );
				}
				break;

			case DRAW_ORDER_GATE_BOTTOM:
				// Don't draw these ever, they are invisible.
				break;

			default:
				LogMsg(WARN,"Unknown Sprite type (0x%04X) being drawn in the Map.", (*iter)->GetDrawOrder() );
		}
	}

	// Do a second pass to draw planet Names on top
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		if( (*iter)->GetDrawOrder() == DRAW_ORDER_PLANET )
		{
			pos = WorldToScreen( (*iter)->GetWorldPosition() );
			MapFont->Render( pos.GetX()+5, pos.GetY(), ((Planet*)(*iter))->GetName().c_str() );
		}
	}

	// TODO: Draw Radar Visibility

	Video::UnsetCropRect();

	// Draw Edges
	Video::DrawBox( relx + GetX(), rely + GetY(), w, h, WHITE, alpha);

	delete spriteList;
	spriteList = NULL;
}

/** \brief Convert click coordinates to World Coordinates
 */
Coordinate Map::ClickToWorld( Coordinate click )
{
	Coordinate world = click;
	world -= Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	world -= Coordinate( w/2, h/2 ); // Offset by the center of this widget
	world /= scale; // Descale the click
	world += center;
	return world;
}

/** \brief Convert world coordinates to relative click Coordinates
 */
Coordinate Map::WorldToClick( Coordinate world )
{
	Coordinate click = world;
	click -= center;
	click *= scale; // Descale the click
	click += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	click += Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	return click;
}

/** \brief Convert world coordinates to screen Coordinates
 */
Coordinate Map::WorldToScreen( Coordinate world )
{
	Coordinate screen = world;
	screen -= center;
	screen *= scale; // Descale the screen
	screen += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	screen += Coordinate( GetAbsX(), GetAbsY() ); // Offset by the absolute screen coordinate
	return screen;
}

bool Map::MouseLUp( int xi, int yi )
{
	Widget::MouseLUp( xi, yi );
	return false;
}

bool Map::MouseLDown( int xi, int yi )
{
	Coordinate worldPos = ClickToWorld( Coordinate(xi,yi) );
	// Use a dummy Sprite to use for searching the SpriteManager
	Effect dummy( worldPos, "", 0);
	Sprite *nearest = sprites->GetNearestSprite( &dummy, QUADRANTSIZE );
	if( nearest )
	{
		printf("Clicked on Sprite %d\n", nearest->GetID() );
	}
	Widget::MouseLDown( xi, yi );
	return false;
}

/** \brief Pan the Map
 */
bool Map::MouseDrag( int xi, int yi )
{
	center -= Coordinate( (xi-x) - dragX , ((yi-y) - dragY) ) / scale;
	Widget::MouseDrag( xi, yi );
	dragX = xi-x;
	dragY = yi-y;
	return true;
}

/** \brief Zoom the map in
 */
bool Map::MouseWUp( int xi, int yi )
{
	//cout << "MouseWUp" << ClickToWorld( Coordinate(xi,yi) ) <<endl;
	scale *= MAP_ZOOM_RATIO;
	Widget::MouseWUp( xi, yi );
	return true;
}

/** \brief Zoom the map out
 */
bool Map::MouseWDown( int xi, int yi )
{
	//cout << "MouseWDown" << ClickToWorld( Coordinate(xi,yi) ) <<endl;
	scale /= MAP_ZOOM_RATIO;
	Widget::MouseWDown( xi, yi );
	return true;
}

/** @} */

