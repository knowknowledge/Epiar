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

#define HUD_MESSAGE_BOTTOM_SPACING 10

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
HudMap Hud::mapDisplay = NoMap;
Font *Hud::AlertFont = NULL;

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

StatusBar::StatusBar(string _title, int _width, QuadPosition _pos, string _updater)
	:width(_width)
	,pos(_pos)
	,ratio(0)
{
	strncpy(title,_title.c_str(),sizeof(title));
	title[sizeof(title)-1] = '\0';
	memset( name, '\0', sizeof(name) );
	lua_updater = _updater;
	LogMsg (DEBUG4, "Creating a new StatusBar '%s' : Name(%s) / Ratio( %f)\n",title, name, ratio);
	assert(pos>=0);
	assert(pos<=4);
}

//---------------experimental code from Dido------
/**\brief Assignment operator for class StatusBar.
 * \return Pointer to StatusBar
 */
StatusBar& StatusBar::operator=(StatusBar& object){
	strcpy( title, object.title );
	strcpy( name, object.name );

	ratio = object.ratio;
	lua_updater = object.lua_updater;

	return * this;
}
//---------------experimental code from Dido------END----


void StatusBar::print ()
{
	//LogMsg (DEBUG4, "PRINTOUT OF STATUSBAR AT 0x%X\n\ttitle = %s\n\twidth=%d\n\tpos=%d\n\tname=%s\n\tratio=%f\n", this, title.c_str(), width, pos, GetName().c_str(), ratio);
}

/**\brief Draws the StatusBar
 * \param x x-coordinate
 * \param y y-coordinate
 *
 * \bug This can segfault with an empty title.
 */
void StatusBar::Draw(int x, int y) {
	int widthRemaining = this->width;

	Image *BackgroundLeft = Image::Get( "Resources/Graphics/hud_bar_left.png" );
	Image *BackgroundMiddle = Image::Get( "Resources/Graphics/hud_bar_middle.png" );
	Image *BackgroundRight= Image::Get( "Resources/Graphics/hud_bar_right.png" );

	if(pos == UPPER_RIGHT || pos == LOWER_RIGHT) {
		x = Video::GetWidth() - BackgroundLeft->GetWidth() - width - BackgroundRight->GetWidth();
	}

	// Draw the Border
	BackgroundLeft->Draw(x,y);
	x += BackgroundLeft->GetWidth();
	BackgroundMiddle->DrawTiled(x, y, width, BackgroundMiddle->GetHeight());
	BackgroundRight->Draw(x + width, y);

	BitType->SetColor(1.f, 1.f, 1.f, 1.f);

	// Draw the Title
	int wTitle = BitType->RenderTight( x, y + BackgroundMiddle->GetHalfHeight(), title, Font::LEFT, Font::MIDDLE );
	widthRemaining -= wTitle;
	x += wTitle + 5;

	// Draw Name
	int wName = BitType->RenderTight( x, y + BackgroundMiddle->GetHalfHeight(), name, Font::LEFT, Font::MIDDLE );
	widthRemaining -= wName;
	x += wName;

	// Draw the Bar
	if ( (int)(ratio*widthRemaining) > 0 ) {
		Image *BarLeft = Image::Get( "Resources/Graphics/hud_hullstr_leftbar.png" );
		Image *BarMiddle = Image::Get( "Resources/Graphics/hud_hullstr_bar.png" );
		Image *BarRight = Image::Get( "Resources/Graphics/hud_hullstr_rightbar.png" );

		int bar_y = y + BackgroundLeft->GetHalfHeight() - BarLeft->GetHalfHeight();
		BarLeft->Draw( x, bar_y );
		x += BarLeft->GetWidth();
		int bar_w = widthRemaining - BarLeft->GetWidth() - BarRight->GetWidth();
		BarMiddle->DrawTiled( x, bar_y,static_cast<int>(bar_w*ratio), BarMiddle->GetHeight() );
		BarRight->Draw( x + static_cast<int>(bar_w*ratio), bar_y );
	}
}

void StatusBar::Update() {
	int returnvals, retpos = -1;
	lua_State* L = Lua::CurrentState();

	// Run the StatusBar Updater
	returnvals = Lua::Run( lua_updater, true );

	// Get the new StatusBar Status
	if (returnvals == 0) {
		SetName( "" );
		SetRatio( 0.0f );
	} else if (lua_isnumber(L, retpos) && ( lua_tonumber(L,retpos)>=0.0 && lua_tonumber(L,retpos)<=1.0) )  {
		SetName( "" );
		SetRatio( TO_FLOAT(lua_tonumber(L, retpos)) );
	} else if (lua_isstring(L, retpos)) {
		SetRatio( 0.0f );
		SetName( string(lua_tostring(L, retpos)) );
	} else {
		LogMsg(ERR,"Error running '%s': %s", lua_updater.c_str(), lua_tostring(L, retpos));
	}
	lua_pop(L,returnvals);
}

/**\fn StatusBar::StatusBar(string _title, int _width, QuadPosition _pos, string _name, float _ratio) : title(_title), width(_width), pos(_pos), name(_name), ratio(_ratio)
 * \brief Empty constructor.
 */

/**\fn StatusBar::SetName(string n)
 * \brief Sets the name of the StatusBar
 */
void StatusBar::SetName( string n )
{
	strncpy(name,n.c_str(),sizeof(name));
	name[sizeof(name)-1] = '\0';
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

void Hud::Init( void ) {
	AlertFont = new Font( "Resources/Fonts/FreeSans.ttf" );
	AlertFont->SetSize( 12 );
}

void Hud::Close( void ) {
	delete AlertFont;
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
	for( j = 0; j< MAX_STATUS_BARS; j++){
		if( Bars[j] != NULL ) {
			Bars[j]->Update();
		}
	}
}

/**\brief Draws the Hud
 */
void Hud::Draw( int flags, float fps ) {
	if(flags & HUD_Target)     Hud::DrawTarget();
	if(flags & HUD_Shield)     Hud::DrawShieldIntegrity();
	if(flags & HUD_Radar)      Hud::DrawRadarNav();
	if(flags & HUD_Messages)   Hud::DrawMessages();
	if(flags & HUD_FPS)        Hud::DrawFPS(fps) ;
	if(flags & HUD_StatusBars) Hud::DrawStatusBars();
	if(flags & HUD_Map)        Hud::DrawMap();
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
	
	AlertFont->SetColor(1.f,1.f,1.f,1.f);
	
	for( i= AlertMessages.rbegin(), j=1; i != AlertMessages.rend(); ++i,++j ){
		//printf("[%d] %s\n", j, (*i).message.c_str() );
		age = now - (*i).start;
		if(age > alertFade){
			AlertFont->SetColor(1.f,1.f,1.f, 1.f - float((age-alertFade))/float(alertDrop-alertFade) );
		} else {
			AlertFont->SetColor(1.f,1.f,1.f,1.f);
		}
		AlertFont->Render( 15, Video::GetHeight() - (j * AlertFont->LineHeight()) - HUD_MESSAGE_BOTTOM_SPACING, (*i).message);
	}
}

/**\brief Draw the current framerate (calculated in simulation.cpp).
 */
void Hud::DrawFPS( float fps ) {
	char frameRate[16];
	BitType->SetColor(1.f,1.f,1.f,1.f);
	snprintf(frameRate, sizeof(frameRate), "%f fps", fps );
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
	Video::SetCropRect( Video::GetWidth() - 125, 9, RADAR_WIDTH-8, RADAR_HEIGHT-8 );
	Radar::Draw();
	Video::UnsetCropRect();
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
	switch( mapDisplay ) {
	case UniverseMap:
		Hud::DrawUniverseMap();
		break;
	case QuadrantMap:
		SpriteManager::Instance()->DrawQuadrantMap();
		break;
	case NoMap:
	default:
		break;
	}
}

void Hud::DrawUniverseMap( void ) {
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
				if( ((Gate*)(*iter))->GetExit() != NULL ) {
					posx2 = startx + ((Gate*)(*iter))->GetExit()->GetWorldPosition().GetX() * scale + halfsize;
					posy2 = starty + ((Gate*)(*iter))->GetExit()->GetWorldPosition().GetY() * scale + halfsize;
					Video::DrawLine( posx,posy, posx2, posy2, 0,.6f,0, alpha*.5f );
				}
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
bool Hud::DeleteStatus( StatusBar* bar ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( Bars[i]==bar )
		{
			Bars[i] = NULL;
			delete bar;
			return true;
		}
	}
	return false;
}

/**\brief Deletes a StatusBar by Name.
 */
bool Hud::DeleteStatus( string deleteTitle ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( (Bars[i]!=NULL) && (Bars[i]->GetTitle()==deleteTitle) )
		{
			delete Bars[i];
			Bars[i] = NULL;
			return true;
		}
	}
	LogMsg(ERR, "Could not find the StatusBar named '%s'", deleteTitle.c_str() );
	return false;
}

/**\brief Deletes a StatusBar by pattern
 */
bool Hud::DeleteStatusMatching( string deletePattern ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( (Bars[i]!=NULL) && (  strstr(Bars[i]->GetTitle().c_str(), deletePattern.c_str() ) != NULL ) )
		{
			cout << Bars[i]->GetTitle() << " matches " << deletePattern.c_str() << endl;
			delete Bars[i];
			Bars[i] = NULL;
			return true;
		}
	}
	LogMsg(ERR, "Could not find the StatusBar matching '%s'", deletePattern.c_str() );
	return false;
}


/**\brief Select what kind of Map is displayed
 */
void Hud::SetMapDisplay( HudMap _newMapDisplay ) {
	mapDisplay = _newMapDisplay;
}

/**\brief Register Lua functions for HUD related updates.
 */
void Hud::RegisterHud(lua_State *L) {

	static const luaL_Reg hudFunctions[] = {
		{"setVisibity", &Hud::setVisibity},
		{"newStatus", &Hud::newStatus},
		{"closeStatus", &Hud::closeStatus},
		{"closeStatusMatching", &Hud::closeStatusMatching},
		{"newAlert", &Hud::newAlert},
		{"getTarget", &Hud::getTarget},
		{"setTarget", &Hud::setTarget},
		{"setMapDisplay", &Hud::setMapDisplay},
		{"getMapDisplay", &Hud::getMapDisplay},
		{NULL, NULL}
	};

	luaL_openlib(L, EPIAR_HUD, hudFunctions, 0);

	lua_pop(L,1);
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

	// Create the Status Bar
	string title = (string)luaL_checkstring(L,1);
	int width = (int)(luaL_checkint(L,2));
	QuadPosition pos = (QuadPosition)(luaL_checkint(L,3));
	if(pos<0||pos>3){
		return luaL_error(L, "Invalid Position %d. Valid Options are: UPPER_LEFT=0, UPPER_RIGHT=1, LOWER_LEFT=2, LOWER_RIGHT=3", pos);
	}
	string updater = (string)luaL_checkstring(L,4);
	StatusBar *bar = new StatusBar(title,width,pos,updater);

	// Add the Bar to the Hud
	AddStatus(bar);
	
	return 0;
}


/**\brief Closes the status (Lua callable).
 */
int Hud::closeStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (Title)", n);
	string deleteTitle = luaL_checkstring(L,1);
	bool success = DeleteStatus( deleteTitle );
	if(!success) {
		return luaL_error(L, "closeStatus couldn't find the StatusBar titled '%s'.", deleteTitle.c_str() );
	}
	return 0;
}

/**\brief Closes the status matching a pattern (Lua callable).
 */
int Hud::closeStatusMatching(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (Pattern)", n);
	string deletePattern = luaL_checkstring(L,1);
	bool success = DeleteStatusMatching( deletePattern );
	if(!success) {
		return luaL_error(L, "closeStatus couldn't find the StatusBar matching '%s'.", deletePattern.c_str() );
	}
	return 0;
}

/**\brief Returns the target (Lua callable).
 */
int Hud::getTarget(lua_State *L) {
	lua_pushinteger(L, targetID );
	return 1;
}

/**\brief Sets the target (Lua callable).
 */
int Hud::setTarget(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (ID)", n);
	Target( luaL_checkint(L,1) );
	return 0;
}

/**\brief Set the kind of Hud Display
 */
int Hud::setMapDisplay(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "setMapDisplay got %d arguments expected 1.   Please use one of these 'NONE', 'QUADRANT', or 'UNIVERSE'.", n);
	string maptype = (string)luaL_checkstring(L,1);

	//printf( "Setting Map Display to '%s'\n", maptype.c_str() );

	if( maptype == "NONE" ) {
		SetMapDisplay( NoMap );
	} else if( maptype == "QUADRANT" ) {
		SetMapDisplay( QuadrantMap );
	} else if( maptype == "UNIVERSE" ) {
		SetMapDisplay( UniverseMap );
	} else {
		LogMsg( ERR, "The Hud does not understand the Map Type '%s'.  Please use one of these 'NONE', 'QUADRANT', or 'UNIVERSE'." );
		SetMapDisplay( NoMap );
	}
	return 0;
}

/**\brief Get the kind of Hud Display
 */
int Hud::getMapDisplay(lua_State *L) {
	switch( mapDisplay ) {
	case UniverseMap:
		lua_pushstring(L,"UNIVERSE");
		break;
	case QuadrantMap:
		lua_pushstring(L,"QUADRANT");
		break;
	case NoMap:
		lua_pushstring(L,"NONE");
		break;
	default:
		assert(0); // This should never happen.
		lua_pushstring(L,"NONE");
	}
	return 1;
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
		
		// Use the OpenGL Crop Rectangle to ensure that the blip is on the radar
		
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
