/**\file			hud.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created  : Sunday, July 23, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Graphics/video.h"
#include "Sprites/player.h"
#include "Sprites/gate.h"
#include "Sprites/spritemanager.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"
#include "Utilities/camera.h"

/* Length of the hull integrity bar (pixels) + 6px (the left+right side imgs) */
#define HULL_INTEGRITY_BAR  65
/* Location on screen of hull integrity bar (x,y) coord is top-left */
#define HULL_INTEGRITY_X     5
#define HULL_INTEGRITY_Y     5
/* Center of radar in px coords. Derrived from hud_radarnav.png */
#define RADAR_MIDDLE_X      61
#define RADAR_MIDDLE_Y      61
/* Width/height of radar. Derrived from hud_radarnav.png */
#define RADAR_WIDTH        122
#define RADAR_HEIGHT       122

list<AlertMessage> Hud::AlertMessages;
StatusBar* Hud::Bars[MAX_STATUS_BARS] = {};
int Hud::targetID = -1;
int Hud::timeTargeted = 0;
int Radar::visibility = 4096;

/**\class AlertMessage
 * \brief Alert/Info messages
 */

/**\var AlertMessage::message
 * \brief The message
 */

/**\var AlertMessage::start
 * \brief The start time (For expiration)
 */

/**\brief Adds an alert to the queue.
 * \param message std:string containing the message
 * \param start Starting time (for expiration)
 */
AlertMessage::AlertMessage( string message, Uint32 start )
{
	this->message = message;
	this->start = start;
}

/**\fn AlertMessage::operator ==(const AlertMessage& other)
 * \brief Compares the start time of two AlertMessage
 */

/**\brief Checks to see if message expired.
 * \param msg Pointer to an AlertMessage object
 * \return true if expired
 */
bool MessageExpired(const AlertMessage& msg){
	return (Timer::GetTicks() - msg.start > OPTION(Uint32,"options/timing/alert-drop"));
}

/**\class StatusBar
 * \brief Status bar
 */

StatusBar::StatusBar(string _title, int _width, QuadPosition _pos, string _name, float _ratio)
	:width(_width)
	,pos(_pos)
	,ratio(_ratio)
{
	strncpy(title,_title.c_str(),40);
	title[39] = '\0';
	name.assign (_name.begin(), _name.end());
	LogMsg (DEBUG4, "Creating a new StatusBar '%s' : Name(%s) / Ratio( %f)\n",title, GetName().c_str(), ratio);
	assert(pos>=0);
	assert(pos<=4);
}

void StatusBar::print ()
{
	//LogMsg (DEBUG4, "PRINTOUT OF STATUSBAR AT 0x%X\n\ttitle = %s\n\twidth=%d\n\tpos=%d\n\tname=%s\n\tratio=%f\n", this, title.c_str(), width, pos, GetName().c_str(), ratio);
}


/**\var StatusBar::im_infobar_left
 * \brief Left side image
 */

/**\var StatusBar::im_infobar_right
 * \brief Right side image
 */

/**\var StatusBar::im_infobar_middle
 * \brief Middle image
 */

/**\brief Draws the StatusBar
 * \param x x-coordinate
 * \param y y-coordinate
 *
 * \bug This can segfault with an empty title.
 */
void StatusBar::Draw(int x, int y) {
	int widthRemaining = this->width;
	Image *BorderLeft = Image::Get( "Resources/Graphics/hud_bar_left.png" );
	Image *BorderMiddle = Image::Get( "Resources/Graphics/hud_bar_middle.png" );
	Image *BorderRight= Image::Get( "Resources/Graphics/hud_bar_right.png" );

	if(pos==UPPER_RIGHT||pos==LOWER_RIGHT){
		x = Video::GetWidth() - BorderLeft->GetWidth() - width - BorderRight->GetWidth();
	}

	// Draw the Border
	BorderLeft->Draw(x,y);
	x += BorderLeft->GetWidth();
	BorderMiddle->DrawTiled(x,y,width, BorderMiddle->GetHeight());
	BorderRight->Draw(x+width,y);

	BitType->SetColor(1.f,1.f,1.f,1.f);

	// Draw the Title
	int wTitle = BitType->RenderTight( x, y+BorderMiddle->GetHalfHeight(), title,Font::LEFT,Font::MIDDLE );
	widthRemaining -= wTitle;
	x += wTitle + 5;

	// Draw Name
	if( !name.empty() ) {
		int wName = BitType->RenderTight( x, y+BorderMiddle->GetHalfHeight(), GetName() ,Font::LEFT,Font::MIDDLE );
		widthRemaining -= wName;
		x += wName;
	}

	// Draw the Bar
	if ( (int)(ratio*widthRemaining) > 0 ) {
		Image *BarLeft = Image::Get( "Resources/Graphics/hud_hullstr_leftbar.png" );
		Image *BarMiddle = Image::Get( "Resources/Graphics/hud_hullstr_bar.png" );
		Image *BarRight = Image::Get( "Resources/Graphics/hud_hullstr_rightbar.png" );

		int bar_y = y + BorderLeft->GetHalfHeight() - BarLeft->GetHalfHeight();
		BarLeft->Draw( x, bar_y );
		x += BarLeft->GetWidth();
		int bar_w = widthRemaining - BarLeft->GetWidth() - BarRight->GetWidth();
		BarMiddle->DrawTiled( x, bar_y,static_cast<int>(bar_w*ratio), BarMiddle->GetHeight() );
		BarRight->Draw( x + static_cast<int>(bar_w*ratio), bar_y );
	}
}

/**\fn StatusBar::StatusBar(string _title, int _width, QuadPosition _pos, string _name, float _ratio) : title(_title), width(_width), pos(_pos), name(_name), ratio(_ratio)
 * \brief Empty constructor.
 */

/**\fn StatusBar::SetName(string n)
 * \brief Sets the name of the StatusBar
 */
void StatusBar::SetName( string n )
{
    name.assign (n.begin(), n.end());
}

/**\fn StatusBar::GetName
 * \brief Returns the name of the StatusBar
 */

/**\fn StatusBar::SetRatio(float _ratio)
 * \brief Sets ratio (Ratio of the middle to the whole)
 */

/**\fn StatusBar::GetRatio
 * \brief Returns the ratio
 * \sa StatusBar::SetRatio(float _ratio)
 */

/**\fn StatusBar::GetPosition
 * \brief Returns the QuadPosition
 */

//Protect members
/**\var StatusBar::title
 * \brief Title of the StatusBar
 */

/**\var StatusBar::width
 * \brief Width of the StatusBar
 */

/**\var StatusBar::pos
 * \brief QuadPosition of the StatusBar
 */

/**\var StatusBar::name
 * \brief Name of the StatusBar
 */

/**\var StatusBar::ratio
 * \brief Ratio of the StatusBar
 * \sa StatusBar::SetRatio(float _ratio)
 */

/**\class Hud
 * \brief Heads-Up-Display. */

/**\brief Empty constructor
 */
Hud::Hud( void ) {
}

/**\brief Updates the HUD
 */
void Hud::Update( void ) {
	int j;
	list<AlertMessage> toDelete;
	list<AlertMessage>::iterator i;
	for( i= AlertMessages.begin(), j=1; i != AlertMessages.end(); ++i,++j ){
		if(MessageExpired(*i))
			toDelete.push_back(*i);
	}
	for( i= toDelete.begin(); i != toDelete.end(); ++i ){
		AlertMessages.remove(*i);
	}
}

/**\brief Draws the Hud
 */
void Hud::Draw( void ) {
	Hud::DrawTarget();
	Hud::DrawShieldIntegrity();
	Hud::DrawRadarNav();
	Hud::DrawMessages();
	Hud::DrawFPS();
	Hud::DrawStatusBars();
	if( OPTION(int,"options/development/map") )
		Hud::DrawMap();
}


/**\brief Handles Hud related User Input
 * \param events User entered Keyboard and mouse clicks
 */
void Hud::HandleInput( list<InputEvent> & events ) {
	list<InputEvent>::iterator i;
	for(i= events.begin(); i != events.end() ; ++i ) {
		// Mouse Clicks
		if( i->type == MOUSE && i->mstate==MOUSELDOWN) {
			Coordinate screenPos(i->mx, i->my), worldPos;
			Camera::Instance()->TranslateScreenToWorld( screenPos, worldPos );
			// Target any clicked Sprite
			list<Sprite*> *impacts = SpriteManager::Instance()->GetSpritesNear( worldPos, 5 );
			if( impacts->size() > 0) {
				Target( (*(impacts->begin()))->GetID());
			}
			delete impacts;
		}
	}
}


/**\brief Draw HUD messages (eg Welcome to Epiar).
 */
void Hud::DrawMessages() {
	int j;
	int now = Timer::GetTicks();
	list<AlertMessage>::reverse_iterator i;
	Uint32 age;
	Uint32 alertFade = OPTION(Uint32,"options/timing/alert-fade");
	Uint32 alertDrop = OPTION(Uint32,"options/timing/alert-drop");
	BitType->SetColor(1.f,1.f,1.f,1.f);
	for( i= AlertMessages.rbegin(), j=1; i != AlertMessages.rend(); ++i,++j ){
		//printf("[%d] %s\n", j, (*i).message.c_str() );
		age = now - (*i).start;
		if(age > alertFade){
			BitType->SetColor(1.f,1.f,1.f, 1.f - float((age-alertFade))/float(alertDrop-alertFade) );
		} else {
			BitType->SetColor(1.f,1.f,1.f,1.f);
		}
		BitType->Render( 15, Video::GetHeight() - (j*BitType->LineHeight()), (*i).message);
	}
}

/**\brief Draw the current framerate (calculated in simulation.cpp).
 */
void Hud::DrawFPS() {
	char frameRate[16];
	BitType->SetColor(1.f,1.f,1.f,1.f);
	snprintf(frameRate, sizeof(frameRate), "%f fps", Simulation::GetFPS());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 15, frameRate );

	snprintf(frameRate, sizeof(frameRate), "%d Quadrants", SpriteManager::Instance()->GetNumQuadrants());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 30, frameRate );

	snprintf(frameRate, sizeof(frameRate), "%d Sprites", SpriteManager::Instance()->GetNumSprites());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 45, frameRate );
}

/**\brief Draws the status bar.
 */
void Hud::DrawStatusBars() {
	// Initialize the starting Coordinates
	int barHeight = Image::Get( "Resources/Graphics/hud_bar_left.png" )->GetHeight()+5;
	Coordinate startCoords[4];
	startCoords[UPPER_LEFT]  = Coordinate(5,Image::Get( "Resources/Graphics/hud_shieldintegrity.png" )->GetHeight()+5);
	startCoords[UPPER_RIGHT] = Coordinate(5,Image::Get( "Resources/Graphics/hud_radarnav.png" )->GetHeight()+5);
	startCoords[LOWER_LEFT]  = Coordinate(5,Video::GetHeight()-barHeight);
	startCoords[LOWER_RIGHT] = Coordinate(5,Video::GetHeight()-barHeight);
	Coordinate offsetCoords[4]= {
		Coordinate(0,barHeight), Coordinate(0,barHeight),
		Coordinate(0,-barHeight), Coordinate(0,-barHeight)};

	BitType->SetColor(1.f,1.f,1.f,1.f);
	int i;
	StatusBar* bar;
	for( i= 0; i < MAX_STATUS_BARS; ++i ){
		bar = Bars[i];
		if( bar == NULL ) continue;
		int pos = bar->GetPosition();
		assert(pos>=0);
		assert(pos<=4);
		int x = static_cast<int>(startCoords[pos].GetX());
		int y = static_cast<int>(startCoords[pos].GetY());
		bar->Draw(x,y);
		startCoords[pos] += offsetCoords[pos];
	}
}

/**\brief Draw the shield bar.
 */
void Hud::DrawShieldIntegrity() {
	Image::Get( "Resources/Graphics/hud_shieldintegrity.png" )->Draw( 35, 5 );
}

/**\brief Draw the radar.
 */
void Hud::DrawRadarNav( void ) {
	Image::Get( "Resources/Graphics/hud_radarnav.png" )->Draw( Video::GetWidth() - 129, 5 );
	
	Radar::Draw();
}

/**\brief Draws the target.
 */
void Hud::DrawTarget( void ) {
	Sprite* target = SpriteManager::Instance()->GetSpriteByID( targetID );
	if(target != NULL) {
		int x = target->GetWorldPosition().GetScreenX();
		int y = target->GetWorldPosition().GetScreenY();
		int r = target->GetRadarSize();
		Color c = target->GetRadarColor();

		if( Timer::GetTicks() - timeTargeted < OPTION(Uint32,"options/timing/target-zoom")) {
			r += Video::GetHalfHeight() - Video::GetHalfHeight()*(Timer::GetTicks()-timeTargeted)/OPTION(Uint32,"options/timing/target-zoom");
		}
	
		Video::DrawTarget(x,y,r,r,5,c.r,c.g,c.b);
	}
}

void Hud::DrawMap( void ) {
	//Video::GetHeight()
	float size, halfsize;
	float scale;
	list<Sprite*> *sprites;
	list<Sprite*>::iterator iter;
	int startx, starty;
	int posx, posy;
	int posx2, posy2;
	Color col;
	Color field;
	int i;
	float alpha;
	float n,s,e,w, edge;

	// Configurable Settings
	size = 700.0f;
	halfsize = size/2;
	startx = Video::GetHalfWidth()-halfsize;
	starty = Video::GetHalfHeight()-halfsize;
	alpha = .7;

	// Strech the Map so that it covers all QuadTrees
	SpriteManager::Instance()->GetBoundaries(&n,&s,&e,&w);
	// edge is the maximum distance from zero of n,s,e, and w
	edge = (n>-s)?n:-s;
	edge = (edge>-w)?edge:-w;
	edge = (edge>e)?edge:e;
	scale = (size) / ( 2*(edge+QUADRANTSIZE) );
	
	int retrieveSprites=(
						 DRAW_ORDER_PLAYER	|
						 DRAW_ORDER_PLANET	|
						 DRAW_ORDER_GATE_TOP );
	
	if( OPTION(int,"options/development/ships-worldmap") )
		retrieveSprites= retrieveSprites | DRAW_ORDER_SHIP;
	
	sprites = SpriteManager::Instance()->GetSprites( retrieveSprites );

	// The Backdrop
	Video::DrawRect( startx,starty,size,size,  0,0,0,alpha);
	Video::DrawLine( startx        , starty        , startx + size , starty        , .3f,.0f,.0f ,alpha ); // Top
	Video::DrawLine( startx        , starty + size , startx + size , starty + size , .3f,.0f,.0f ,alpha );
	Video::DrawLine( startx        , starty        , startx        , starty + size , .3f,.0f,.0f ,alpha );
	Video::DrawLine( startx + size , starty + size , startx + size , starty        , .3f,.0f,.0f ,alpha );

	// The Quadrant Lines
	for( i=static_cast<int>(QUADRANTSIZE); i<=edge; i+= 2*static_cast<int>(QUADRANTSIZE) )
	{
		Video::DrawLine( startx                          , starty + int( i*scale+halfsize) , startx + (int)size              , starty + int( i*scale+halfsize) , .3f,.3f,.3f ,alpha );
		Video::DrawLine( startx                          , starty + int(-i*scale+halfsize) , startx + (int)size              , starty + int(-i*scale+halfsize) , .3f,.3f,.3f ,alpha );
		Video::DrawLine( startx + int( i*scale+halfsize) ,starty                           , startx + int( i*scale+halfsize) , starty + (int)size              , .3f,.3f,.3f ,alpha );
		Video::DrawLine( startx + int(-i*scale+halfsize) ,starty                           , startx + int(-i*scale+halfsize) , starty + (int)size              , .3f,.3f,.3f ,alpha );
	}

	// The Sprites
	for( iter = sprites->begin(); iter != sprites->end(); ++iter )
	{
		col = (*iter)->GetRadarColor();
		posx = startx + (*iter)->GetWorldPosition().GetX() * scale + halfsize;
		posy = starty + (*iter)->GetWorldPosition().GetY() * scale + halfsize;

		switch( (*iter)->GetDrawOrder() ) {
			case DRAW_ORDER_PLAYER:
				Video::DrawFilledCircle( posx, posy, 2, col.r,col.g,col.b, alpha );
				break;
			case DRAW_ORDER_PLANET:
				field = ((Planet*)(*iter))->GetAlliance()->GetColor();
				Video::DrawFilledCircle( posx, posy, ((Planet*)(*iter))->GetInfluence()*scale, field.r, field.g, field.b, alpha*.5f );
				Video::DrawCircle( posx, posy, 3, 1, col.r,col.g,col.b, alpha );
				break;
			case DRAW_ORDER_SHIP:
				Video::DrawFilledCircle( posx, posy, 2, col.r,col.g,col.b, alpha );
				break;

			case DRAW_ORDER_GATE_TOP:
				Video::DrawCircle( posx, posy, 3, 1, col.r,col.g,col.b, alpha );
				posx2 = startx + ((Gate*)(*iter))->GetExit()->GetWorldPosition().GetX() * scale + halfsize;
				posy2 = starty + ((Gate*)(*iter))->GetExit()->GetWorldPosition().GetY() * scale + halfsize;
				Video::DrawLine( posx,posy, posx2, posy2, 0,.6f,0, alpha*.5f );
				break;
			default:
				LogMsg(WARN,"Unknown Sprite type being drawn in the Map.");
		}
	}
	posx = startx + Camera::Instance()->GetFocusCoordinate().GetX() * scale + halfsize;
	posy = starty + Camera::Instance()->GetFocusCoordinate().GetY() * scale + halfsize;
	Video::DrawFilledCircle( posx, posy, Radar::GetVisibility()*scale, 0.9, 0.9, 0.9, alpha*.25 );

	delete sprites;
	sprites = NULL;
}

/**\brief Adds a new AlertMessage.
 * \param message C string to message
 */
void Hud::Alert( const char *message, ... )
{
	va_list args;
	char msgBuffer[ 4096 ] = {0};

	va_start( args, message );

	vsnprintf( msgBuffer, 4095, message, args );

	va_end( args );

	AlertMessages.push_back( AlertMessage( msgBuffer, Timer::GetTicks() ) );
}


/**\brief Changes the Hud target
 * \param id Unique Sprite id number
 */
void Hud::Target(int id) {
	targetID = id;
	timeTargeted = Timer::GetTicks();
}

/**\brief Adds a new StatusBar.
 * \param bar Pointer to a new StatusBar
 */
void Hud::AddStatus( StatusBar* bar) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( Bars[i]== NULL )
		{
			Bars[i] = bar;
			break;
		}
	}
}

/**\brief Deletes a StatusBar.
 */
void Hud::DeleteStatus( StatusBar* bar ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( Bars[i]==bar )
		{
			Bars[i] = NULL;
			delete bar;
		}
	}
}

/**\brief Register Lua functions for HUD related updates.
 */
void Hud::RegisterHud(lua_State *L) {

	static const luaL_Reg hudFunctions[] = {
		{"setVisibity", &Hud::setVisibity},
		{"newStatus", &Hud::newStatus},
		{"newAlert", &Hud::newAlert},
		{"getTarget", &Hud::getTarget},
		{"setTarget", &Hud::setTarget},
		{NULL, NULL}
	};

	static const luaL_Reg hudMethods[] = {
		{"setStatus", &Hud::setStatus},
		{"closeStatus", &Hud::closeStatus},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_HUD);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, NULL, hudMethods, 0);

	luaL_openlib(L, EPIAR_HUD, hudFunctions, 0);
}

/**\brief Set's the visibility of the target (Lua callable)
 */
int Hud::setVisibity(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (visibility)", n);
	int visibility = (int)(luaL_checkint(L,1));
	Radar::SetVisibility(visibility);
	return 0;
}

/**\brief Creates a new Alert (Lua callable).
 */
int Hud::newAlert(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (message)", n);
	const char* msg = luaL_checkstring(L,1);
	Alert(msg);
	return 0;
}

/**\brief Creates a new Status (Lua callable).
 */
int Hud::newStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if ( (n < 3) )
		return luaL_error(L, "Got %d arguments expected 4 (title, width, postition )", n);

	// Allocate memory for a pointer to object
	StatusBar **bar = (StatusBar**)lua_newuserdata(L, sizeof(StatusBar**));
	luaL_getmetatable(L, EPIAR_HUD);
	lua_setmetatable(L, -2);

	// Create the Status Bar
	string title = (string)luaL_checkstring(L,1);
	int width = (int)(luaL_checkint(L,2));
	QuadPosition pos = (QuadPosition)(luaL_checkint(L,3));
	if(pos<0||pos>3){
		return luaL_error(L, "Invalid Position %d. Valid Options are: UPPER_LEFT=0, UPPER_RIGHT=1, LOWER_LEFT=2, LOWER_RIGHT=3", pos);
	}
	*bar= new StatusBar(title,width,pos,"",0.0f);

	// Add the Bar to the Hud
	AddStatus(*bar);
	
	return 1;
}


StatusBar* Hud::checkStatus(lua_State *L, int index) {
	StatusBar *bar = NULL;
	list<StatusBar*>::iterator result;
	StatusBar **barptr = (StatusBar**)luaL_checkudata(L, index, EPIAR_HUD);
	luaL_argcheck(L, barptr != NULL, index, "`EPIAR_HUD' expected. Got NULL Pointer");
	luaL_argcheck(L, *barptr != NULL, index, "`EPIAR_HUD' expected. Got pointer to NULL.");
	bar = *barptr;
	return bar;
}

/**\brief Set's the status (Lua callable)
 */
int Hud::setStatus(lua_State *L) {
	//LogMsg (DEBUG4, "setStatus called");
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, [newName, newRatio])", n);
	StatusBar *bar= checkStatus(L,1);
	//LogMsg (DEBUG4, "after call to checkStatus, address = 0x%X", bar);
	//LogMsg (DEBUG4, "    GetName = %s, GetRatio = %f", bar->GetName().c_str(), bar->GetRatio());
	
	if( lua_isnumber(L,2) ) {
		float ratio = (float)(luaL_checknumber(L,2));
		bar->SetRatio(ratio);
	} else {
		string name = (string)(luaL_checkstring(L,2));
		bar->SetName(name);
	}
	//LogMsg (DEBUG4, "End of setStatus");

	return 0;
}

/**\brief Closes the status (Lua callable).
 */
int Hud::closeStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	StatusBar *bar= checkStatus(L,1);
	DeleteStatus(bar);
	assert(0);
	return 0;
}

/**\brief Returns the target (Lua callable).
 */
int Hud::getTarget(lua_State *L) {
	lua_pushinteger(L, targetID );
	return 1;
}

/**\brief Set's the target (Lua callable).
 */
int Hud::setTarget(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (ID)", n);
	Target( luaL_checkint(L,1) );
	return 0;
}

/**\class Radar
 * \brief Hud Element that displays nearby objects. */

/**\brief Empty constructor.
 */
Radar::Radar( void ) {
}

/**\brief Sets the visibility.
 * \param visibility true or false
 */
void Radar::SetVisibility( int visibility ) {
	Radar::visibility = visibility;
}

/**\brief Draws the radar.
 */
void Radar::Draw( void ) {
	short int radar_mid_x = RADAR_MIDDLE_X + Video::GetWidth() - 129;
	short int radar_mid_y = RADAR_MIDDLE_Y + 5;
	int radarSize;

	list<Sprite*> *spriteList = SpriteManager::Instance()->GetSpritesNear(Camera::Instance()->GetFocusCoordinate(), (float)visibility);
	for( list<Sprite*>::const_iterator iter = spriteList->begin(); iter != spriteList->end(); iter++)
	{
		Coordinate blip;
		Sprite *sprite = *iter;
		
		//if( sprite->GetDrawOrder() == DRAW_ORDER_PLAYER ) continue;
		
		// Calculate the blip coordinate for this sprite
		Coordinate wpos = sprite->GetWorldPosition();
		WorldToBlip( wpos, blip );
		
		if( blip.ViolatesBoundary( -(RADAR_HEIGHT / 2.0), (RADAR_WIDTH / 2.0), (RADAR_HEIGHT / 2.0), -(RADAR_WIDTH / 2.0) ) == false ) {
			/* blip is on the radar */
			
			/* Convert to screen coords */
			blip.SetX( blip.GetX() + radar_mid_x );
			blip.SetY( blip.GetY() + radar_mid_y );

			radarSize = int((sprite->GetRadarSize() / float(visibility)) * (RADAR_HEIGHT/4.0));
			
			
			if( radarSize >= 1 ) {
				Video::DrawCircle( blip, radarSize, 1, sprite->GetRadarColor() );
			} else {
				Video::DrawPoint( blip, sprite->GetRadarColor() );
			}
		}
	}
	delete spriteList;
}

/**\brief Gets the radar position based on world coordinate
 * \param w Pointer to world coordinate
 * \retval b Pointer to radar coordinate
 */
void Radar::WorldToBlip( Coordinate &w, Coordinate &b ) {
	Coordinate focus = Camera::Instance()->GetFocusCoordinate();
	
	b.SetX( ( ( w.GetX() - focus.GetX() ) / float(visibility) ) * ( RADAR_WIDTH / 2.0 ) );
	b.SetY( ( ( w.GetY() - focus.GetY() ) / float(visibility) ) * ( RADAR_HEIGHT / 2.0 ) );
}
