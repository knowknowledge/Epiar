/**\file			ui_map.cpp
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Saturday, May 28, 2011
 * \brief			Map Widget
 * \details
 */

#include "UI/ui.h"
#include "UI/ui_map.h"
#include "Sprites/planets.h"
#include "Sprites/gate.h"

#define MAP_ZOOM_RATIO 1.1f ///< The rate at which the Map Zooms in and out.

Font *Map::MapFont = NULL;

Map::Map( int x, int y, int w, int h, Coordinate center, SpriteManager* sprites )
{
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->center = center;
	this->sprites = sprites;

	alpha = .7;

	float size = (h<w) ? h : w; // Max of Height and Width

	// Initially strech the Map so that it covers all QuadTrees
	float north,south,east,west,edge;
	sprites->GetBoundaries(&north,&south,&east,&west);
	// edge is the maximum distance from zero of north,south,east, and west
	edge = (north>-south)?north:-south;
	edge = (edge>-west)?edge:-west;
	edge = (edge>east)?edge:east;
	scale = (size) / ( 2*(edge+QUADRANTSIZE) );

	MapFont = new Font( SKIN("Skin/HUD/Map/Font") );
}

Map::~Map()
{
	sprites = NULL;
}

void Map::Draw( int relx, int rely )
{
	float halfheight, halfwidth;
	list<Sprite*> *spriteList;
	list<Sprite*>::iterator iter;
	int startx, starty;

	// These variables are used for almost every sprite symbol
	Coordinate pos, pos2;
	Color col;
	Color field;
	Color gatePath;

	// Configurable Settings
	halfheight = GetH()/2;
	halfwidth = GetW()/2;
	startx = relx + GetX();
	starty = rely + GetY();
	gatePath = Color( SKIN("Skin/HUD/Map/GatePath") );

	Coordinate start(startx,starty);
	Coordinate widgetCenter(halfwidth,halfheight);

	int retrieveSprites=(
						 DRAW_ORDER_PLAYER	|
						 DRAW_ORDER_PLANET	|
						 DRAW_ORDER_GATE_TOP );

	// Show sprites only if this option is set.
	if( OPTION(int,"options/development/ships-worldmap") ) {
		retrieveSprites = retrieveSprites | DRAW_ORDER_SHIP;
	}

	spriteList = sprites->GetSprites( retrieveSprites );

	// The Backdrop
	Video::DrawRect( startx,starty,w,h,BLACK,alpha);
	Video::DrawBox( startx,starty,w,h,WHITE,alpha);

	Video::SetCropRect( startx, starty, w, h );

	// TODO: Quadrant lines should be be drawn correctly.

	// The Quadrant Lines
	// for( int i=static_cast<int>(QUADRANTSIZE); i<=edge; i+= 2*static_cast<int>(QUADRANTSIZE) )
	// {
	// 	Video::DrawLine( startx                           , starty + int( i*scale+halfheight) , startx + (int)size               , starty + int( i*scale+halfheight) , .3f,.3f,.3f ,alpha );
	// 	Video::DrawLine( startx                           , starty + int(-i*scale+halfheight) , startx + (int)size               , starty + int(-i*scale+halfheight) , .3f,.3f,.3f ,alpha );
	// 	Video::DrawLine( startx + int( i*scale+halfwidth) , starty                            , startx + int( i*scale+halfwidth) , starty + (int)size                , .3f,.3f,.3f ,alpha );
	// 	Video::DrawLine( startx + int(-i*scale+halfwidth) , starty                            , startx + int(-i*scale+halfwidth) , starty + (int)size                , .3f,.3f,.3f ,alpha );
	// }

	// Draw the Sprites
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		col = (*iter)->GetRadarColor();
		pos = start
			+ ((*iter)->GetWorldPosition() - center ) * scale
			+ widgetCenter;

		switch( (*iter)->GetDrawOrder() ) {
			case DRAW_ORDER_PLAYER:
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;
			case DRAW_ORDER_PLANET:
				field = ((Planet*)(*iter))->GetAlliance()->GetColor();
				Video::DrawFilledCircle( pos, ((Planet*)(*iter))->GetInfluence()*scale, field, alpha*.5f );
				Video::DrawCircle( pos, 3, 1, col, alpha );
				break;
			case DRAW_ORDER_SHIP:
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;

			case DRAW_ORDER_GATE_TOP:
				Video::DrawCircle( pos, 3, 1, col, alpha );
				if( ((Gate*)(*iter))->GetExit() != NULL ) {
					pos2 = start
					     + (((Gate*)(*iter))->GetExit()->GetWorldPosition() - center ) * scale
					     + widgetCenter;
					Video::DrawLine( pos, pos2, gatePath, alpha*.5f );
				}
				break;
			default:
				LogMsg(WARN,"Unknown Sprite type being drawn in the Map.");
		}
	}

	// Do a second pass to draw planet Names on top
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		if( (*iter)->GetDrawOrder() == DRAW_ORDER_PLANET )
		{
			pos = start
				+ ((*iter)->GetWorldPosition() - center ) * scale
				+ widgetCenter;
			MapFont->Render( pos.GetX()+5, pos.GetY(), ((Planet*)(*iter))->GetName().c_str() );
		}
	}

	// TODO: Draw Radar Visibility

	Video::UnsetCropRect();

	delete spriteList;
	spriteList = NULL;
}

bool Map::MouseDrag( int xi, int yi )
{
	center -= Coordinate( (xi-x) - dragX , ((yi-y) - dragY) ) / scale;
	Widget::MouseDrag( xi, yi );
	dragX = xi-x;
	dragY = yi-y;
	return true;
}

bool Map::MouseWUp( int xi, int yi )
{
	scale *= MAP_ZOOM_RATIO;
	Widget::MouseWUp( xi, yi );
	return true;
}

bool Map::MouseWDown( int xi, int yi )
{
	scale /= MAP_ZOOM_RATIO;
	Widget::MouseWDown( xi, yi );
	return true;
}
