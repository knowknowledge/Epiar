/**\file			hud.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created  : Sunday, July 23, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Engine/console.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Graphics/video.h"
#include "Sprites/player.h"
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
list<StatusBar*> Hud::Bars;
int Hud::targetID = -1;
int Hud::timeTargeted = 0;
int Radar::visibility = 7000;

/**\class AlertMessage
 * \brief Alert/Info messages
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

/**\brief Draws the StatusBar
 * \param x x-coordinate
 * \param y y-coordinate
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

	// Draw the Title
	if( title != "") {
		Rect recTitle = BitType->Render( x, y+13, title.c_str() );
		widthRemaining -= static_cast<int>(recTitle.w);
		x += static_cast<int>(recTitle.w) + 5;
	}

	// Draw Name
	if( name != "") {
		Rect recName = BitType->Render( x, y+13, name.c_str() );
		widthRemaining -= static_cast<int>(recName.w);
		x += static_cast<int>(recName.w);
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

Hud *Hud::pInstance = 0; // initialize pointer

/**\class Hud
 * \brief Heads-Up-Display. */

/**\brief Gets or initializes the current instance.
 */
Hud *Hud::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Hud; // create the sold instance
	}
	return( pInstance );
}

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
	Console::Update();
}

void Hud::Draw( void ) {
	Hud::DrawTarget();
	Hud::DrawShieldIntegrity();
	Hud::DrawRadarNav();
	Hud::DrawMessages();
	Console::Draw();
	Hud::DrawFPS();
	Hud::DrawStatusBars();
}


/**\brief Handles Hud related User Input
 * \param events User entered Keyboard and mouse clicks
 */
void Hud::HandleInput( list<InputEvent> & events ) {
	list<InputEvent>::iterator i;
	for(i= events.begin(); i != events.end() ; ++i ) {
		// Mouse Clicks
		if( i->type == MOUSE && i->mstate==MOUSEDOWN) {
			Coordinate screenPos(i->mx, i->my), worldPos;
			Camera::Instance()->TranslateScreenToWorld( screenPos, worldPos );
			// Target any clicked Sprite
			list<Sprite*> *impacts = SpriteManager::Instance()->GetSpritesNear( worldPos, 5 );
			if( impacts->size() > 0) {
				Target( (*(impacts->begin()))->GetID());
			}
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
	for( i= AlertMessages.rbegin(), j=1; i != AlertMessages.rend(); ++i,++j ){
		//printf("[%d] %s\n", j, (*i).message.c_str() );
		age = now - (*i).start;
		if(age > alertFade){
			BitType->SetColor(1.f,1.f,1.f, 1.f - float((age-alertFade))/float(alertDrop-alertFade) );
		} else {
			BitType->SetColor(1.f,1.f,1.f,1.f);
		}
		BitType->Render( 15, Video::GetHeight() - (j*15), (*i).message.c_str() );
	}
}

/**\brief Draw the current framerate (calculated in simulation.cpp).
 */
void Hud::DrawFPS() {
	const char *frameRate[16] = {0};
	memset(frameRate, 0, sizeof(char) * 10);
	BitType->SetColor(1.f,1.f,1.f,1.f);
	snprintf((char *)frameRate, sizeof(frameRate), "%f fps", Simulation::GetFPS());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 15, (const char *)frameRate );

	snprintf((char *)frameRate, sizeof(frameRate), "%d Quadrants", SpriteManager::Instance()->GetNumQuadrants());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 30, (const char *)frameRate );

	snprintf((char *)frameRate, sizeof(frameRate), "%d Sprites", SpriteManager::Instance()->GetNumSprites());
	BitType->Render( Video::GetWidth()-100, Video::GetHeight() - 45, (const char *)frameRate );
}

/**\brief Draws the status bar.
 */
void::Hud::DrawStatusBars() {
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
	// 
	list<StatusBar*>::iterator i;
	for( i= Bars.begin(); i != Bars.end(); ++i ){
		int pos = (*i)->GetPosition();
		(*i)->Draw(static_cast<int>(startCoords[pos].GetX()),static_cast<int>(startCoords[pos].GetY()));
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
	Bars.push_back(bar);
}

/**\brief Deletes a StatusBar.
 */
void Hud::DeleteStatus( StatusBar* bar ) {
	Bars.remove(bar);
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
	if (n != 4)
		return luaL_error(L, "Got %d arguments expected 4 (title, width, postition, [name | ratio] )", n);

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
	if( lua_isnumber(L,4) ) {
		float ratio = (float)(luaL_checknumber(L,4));
		*bar= new StatusBar(title,width,pos,"",ratio);
	} else {
		string name = (string)(luaL_checkstring(L,4));
		*bar= new StatusBar(title,width,pos,name,0.0f);
	}

	// Add the Bar to the Hud
	AddStatus(*bar);
	
	return 1;
}

/**\brief Set's the status (Lua callable)
 */
int Hud::setStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 2)
		return luaL_error(L, "Got %d arguments expected 2 (self, [newName, newRatio])", n);
	StatusBar **bar= (StatusBar**)lua_touserdata(L,1);

	if( lua_isnumber(L,2) ) {
		float ratio = (float)(luaL_checknumber(L,2));
		(*bar)->SetRatio(ratio);
	} else {
		string name = (string)(luaL_checkstring(L,2));
		(*bar)->SetName(name);
	}

	return 0;
}

/**\brief Closes the status (Lua callable).
 */
int Hud::closeStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (self)", n);
	StatusBar **bar= (StatusBar**)lua_touserdata(L,1);
	DeleteStatus(*bar);
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
